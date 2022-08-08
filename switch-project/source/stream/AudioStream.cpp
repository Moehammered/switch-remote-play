#include "AudioStream.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <SDL2/SDL_audio.h>

namespace
{
    auto audioDeviceID = SDL_AudioDeviceID{0};

    SDL_AudioDeviceID initialiseAudioDevice(SDL_AudioSpec const desiredAudioSettings)
    {
        auto availableAudio = SDL_AudioSpec{};
        auto id = SDL_OpenAudioDevice(nullptr, 0, &desiredAudioSettings, &availableAudio, SDL_AUDIO_ALLOW_ANY_CHANGE);
        if(id == 0)
            std::cout << "Failed to create audio device: " << SDL_GetError() << "\n";
        else
        {
            if(desiredAudioSettings.format != availableAudio.format)
                std::cout << "Wanted format (" << desiredAudioSettings.format << ") - received (" << availableAudio.format << ")\n";
            std::cout << "Freq: " << availableAudio.freq << "\n";
            std::cout << "ch: " << (int)availableAudio.channels << "\n";
            std::cout << "format: " << availableAudio.format << "\n";
            std::cout << "samples: " << availableAudio.samples << "\n";
            std::cout << "silence: " << (int)availableAudio.silence << "\n";
            std::cout << "size: " << availableAudio.size << "\n";
        }

        return id;
    }

    void readAudioStream(uint32_t const handle, uint8_t * buffer, int const length)
    {
        //do stream read here
        if(handle <= 0 || buffer == nullptr || length <= 0)
            return;

        auto totalRead = 0U;
        do
        {
            auto destination = buffer + totalRead;
            auto const remainingDataLength = length - totalRead;
            auto const lastRead = recvfrom(handle, destination, remainingDataLength, 0, nullptr, nullptr);
            if(lastRead > 0)
                totalRead += lastRead;
            else
            {
                std::cout << "Audio Stream - Received err result: " << lastRead << "\n";
                std::cout << strerror(errno) << "\n";
                break;
            }
        } while (totalRead < length && handle > 0);
    }

    void fillAudioBuffer(void * userdata, uint8_t * stream, int len)
    {
        //SDL doesn't gaurantee buffer is properly initialised
        // memset(stream, 0, len);

        //userdata contains the audioSocket handle
        auto const& socketHandle = *reinterpret_cast<uint32_t*>(userdata);

        //call socket read function here
        readAudioStream(socketHandle, stream, len);
    }
}

AudioStream::AudioStream()
    : audioSocket{0}
{}

bool AudioStream::Running() const
{ 
    return audioSocket > 0 && audioDeviceID > 0;
}

bool AudioStream::Start(uint16_t const port)
{
    if(Running())
        Shutdown();
        
    audioSocket = createSocket(port);
    if(audioSocket > 0)
    {
        auto desiredAudio = SDL_AudioSpec{};
        //https://www.vocitec.com/docs-tools/blog/sampling-rates-sample-depths-and-bit-rates-basic-audio-concepts
        //should read up on relationship with sample rate and freq.
        desiredAudio.freq = 44100;
        desiredAudio.channels = 2;
        desiredAudio.format = AUDIO_S16LSB;
        desiredAudio.samples = 1024;
        desiredAudio.callback = fillAudioBuffer;
        desiredAudio.userdata = &audioSocket;
        audioDeviceID = initialiseAudioDevice(desiredAudio);
        if(audioDeviceID > 0)
        {
            //started up now, ready for audio stream
            SDL_PauseAudioDevice(audioDeviceID, 0);
            return true;
        }
        else
        {
            close(audioSocket);
            audioSocket = 0;
            return false;
        }
    }
    else
        return false;
}

void AudioStream::Shutdown()
{
    if(audioSocket <= 0)
        return;

    shutdown(audioSocket, SHUT_RDWR);
    close(audioSocket);
    SDL_CloseAudioDevice(audioDeviceID);
    audioDeviceID = 0;
    audioSocket = 0;
}

int32_t AudioStream::createSocket(uint16_t const port)
{
    auto sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(sock >= 0)
    {
        sockaddr_in addr {0};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        auto bindCheck = bind(sock, (sockaddr*)&addr, sizeof(addr));
        if(bindCheck < 0)
        {
            std::cout << "Failed to bind UDP socket - " << strerror(errno) << "\n";
            close(sock);
            return -1;
        }
        else
            return sock;
    }
    else
    {
        std::cout << "Failed to create UDP Socket - " << strerror(errno) << "\n";
        return -1;
    }
}