#include "AudioPlayback.h"
#include <malloc.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <iomanip>

namespace
{
    AudioConfig constexpr defaultAudioSettings
    {
        .sampleRate {48000},
        .framerate {100},
        .channelCount {2},
        .bitrate {16}
    };
}

AudioPlayback::AudioPlayback()
    : settings{defaultAudioSettings}, 
      packetsPerFrame {settings.sampleRate/settings.framerate * settings.bitrate/8 * settings.channelCount},
      bufferCount {3}, bufferIndex {0},
      packetBuffers{}, switchAudioBuffers{},
      releaseBuffer {nullptr}
{
    for(auto i = 0U; i < bufferCount; ++i)
    {
        AudioOutBuffer defaultBuffer
        {
            .next {nullptr}, .buffer {nullptr},
            .buffer_size {0}, .data_size {0}, .data_offset {44}
        };
        switchAudioBuffers.push_back(defaultBuffer);

        auto buffer = (char*)memalign(audioBufferAlignment, packetsPerFrame);
        memset(buffer, 0, packetsPerFrame);
        packetBuffers.push_back(buffer);
    }

    std::cout << "AudioBufferAlignment: 0x" << std::setbase(16) << audioBufferAlignment << "\n";
    std::cout << std::setbase(10);
    std::cout << "Packets Per Frame: " << packetsPerFrame << "\n";
    std::cout << "Buffer Count: " << bufferCount << "\n";
}

AudioPlayback::AudioPlayback(AudioConfig config, uint32_t swapBuffers)
    : settings{config}, 
      packetsPerFrame {settings.sampleRate/settings.framerate * settings.bitrate/8 * settings.channelCount},
      bufferCount {swapBuffers}, bufferIndex {0},
      packetBuffers{}, switchAudioBuffers{},
      releaseBuffer {nullptr}
{
    for(auto i = 0U; i < bufferCount; ++i)
    {
        AudioOutBuffer defaultBuffer
        {
            .next {nullptr}, .buffer {nullptr},
            .buffer_size {0}, .data_size {0}, .data_offset {44}
        };
        switchAudioBuffers.push_back(defaultBuffer);

        auto buffer = (char*)memalign(audioBufferAlignment, packetsPerFrame);
        packetBuffers.push_back(buffer);
    }
}

int32_t AudioPlayback::ReadPackets(int32_t const & udpSocket)
{
    auto totalRead = 0U;
    socklen_t slen = sizeof(sockaddr_in);
    do
    {
        sockaddr_in si_other;
        auto dst = packetBuffers[bufferIndex] + totalRead;
        auto lastRead = recvfrom(udpSocket, dst, packetsPerFrame-totalRead, 0, (sockaddr*)&si_other, &slen);
        if(lastRead > 0)
            totalRead += lastRead;
        else if(lastRead <= 0)
        {
            std::cout << "Audio Stream - Received Error Res: " << lastRead << "\n";
            return -1;
        }
    } while (totalRead < packetsPerFrame && udpSocket > 0);

    // for(auto i = 0; i < 44; ++i)
    //     packetBuffers[bufferIndex][i] = 0;

    return totalRead;
}

void AudioPlayback::Play()
{
    switchAudioBuffers[bufferIndex].buffer = packetBuffers[bufferIndex];
    switchAudioBuffers[bufferIndex].buffer_size = packetsPerFrame;
    switchAudioBuffers[bufferIndex].data_size = packetsPerFrame;
    // switchAudioBuffers[bufferIndex].data_offset = 44;
    audoutPlayBuffer(&switchAudioBuffers[bufferIndex], &releaseBuffer);
    // std::cout << "ds = " << releaseBuffer->data_size << "\n";
    // std::cout << "bs = " << releaseBuffer->buffer_size << "\n";
    // std::cout << "next = " << releaseBuffer->next << "\n\n\n";
    if(++bufferIndex >= bufferCount)
        bufferIndex = 0;
}

void AudioPlayback::Cleanup()
{
    auto flushed = false;
    audoutFlushAudioOutBuffers(&flushed);
    for(auto b : packetBuffers)
        free(b);
    
    packetBuffers.clear();
    switchAudioBuffers.clear();
}