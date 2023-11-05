#include "AudioStream.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <SDL2/SDL_audio.h>
#include <thread>
#include <atomic>
#include <algorithm>
extern "C"
{    
    #include <libswresample/swresample.h>
    #include <libavutil/opt.h>
    #include <libavutil/error.h>
}
#include "../utils/SDLHelper.h"
#include "audio/FfmpegAudioStream.h"

namespace
{
    auto audioDeviceID = SDL_AudioDeviceID{0};
    std::atomic_flag queueThreadRunning{};
    std::thread audioQueueThread{};

    AVSampleFormat SdlFormatToAvFormat(SDL_AudioFormat const form)
    {
        switch(form)
        {
            default:
            case AUDIO_S16:
            case AUDIO_U16:
                return AVSampleFormat::AV_SAMPLE_FMT_S16;

            case AUDIO_U8:
            case AUDIO_S8:
                return AVSampleFormat::AV_SAMPLE_FMT_U8;

            case AUDIO_S32:
                return AVSampleFormat::AV_SAMPLE_FMT_S32;

            case AUDIO_F32:
                return AVSampleFormat::AV_SAMPLE_FMT_FLT;
        }
    }

    SwrContext* createResampler(SDL_AudioSpec const& spec, AVCodecContext const * const codec)
    {
        auto resampler = swr_alloc();

        av_opt_set_int(resampler, "in_channel_layout", codec->channel_layout, 0);
        av_opt_set_int(resampler, "out_channel_layout", av_get_default_channel_layout(spec.channels), 0);
        av_opt_set_int(resampler, "in_sample_rate", codec->sample_rate, 0);
        av_opt_set_int(resampler, "out_sample_rate", spec.freq, 0);
        av_opt_set_sample_fmt(resampler, "in_sample_fmt", codec->sample_fmt, 0);
        av_opt_set_sample_fmt(resampler, "out_sample_fmt", SdlFormatToAvFormat(spec.format), 0);

        auto const initCode = swr_init(resampler);
        if(initCode < 0)
        {
            std::cout << "Failed to initialise audio resampler with error code:\n";
            std::cout << "  " << ffmpeg::avErrorToString(initCode) << "\n";
            swr_free(&resampler);
        }
        return resampler;
    }

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
            std::cout << utils::to_string(availableAudio) << "\n";
        }

        return id;
    }

    std::thread startAudioThread(SDL_AudioSpec const settings, ffmpeg::AudioStream& stream) 
    {
        queueThreadRunning.test_and_set();

        return std::thread([&, settings = settings] {
            auto resampler = createResampler(settings, stream.CodecContext());

            auto const sampleToByteSize = [&](uint32_t const sampleCount) {
                return sampleCount * settings.channels * sizeof(int16_t);
            };

            auto const maxSampleSize = sampleToByteSize(settings.samples * 4);
            auto resampleBuffer = std::vector<uint8_t>(maxSampleSize, 0);
            auto sampleBufferRef = resampleBuffer.data();

            auto queueAudio = [&] (AVFrame const * const frame) {
                auto const samplesConverted = swr_convert(resampler, &sampleBufferRef, resampleBuffer.size(), (const uint8_t**)frame->extended_data, frame->nb_samples);
                if(samplesConverted > 0)
                {
                    auto const convertedByteSize = sampleToByteSize(samplesConverted);
                    auto const queueSize = std::min(convertedByteSize, maxSampleSize);
                    SDL_QueueAudio(audioDeviceID, sampleBufferRef, queueSize);
                }
            };

            auto const maxQueueSize = resampleBuffer.size() * 10;
            auto trimAudioQueue = [&] {
                if(SDL_GetQueuedAudioSize(audioDeviceID) > maxQueueSize)
                    SDL_ClearQueuedAudio(audioDeviceID);
            };

            SDL_PauseAudioDevice(audioDeviceID, 0);
            while(queueThreadRunning.test_and_set() && stream.nextPacket())
            {
                //check if queue is huge
                trimAudioQueue();
                //read audio data
                auto const audioFrame = stream.nextFrame();
                if(audioFrame != nullptr)
                    queueAudio(audioFrame);
            }

            std::cout << "Clearing SDL Audio Queue\n\n";
            SDL_ClearQueuedAudio(audioDeviceID);
        });
    }
}

AudioStream::AudioStream()
    : ffmpegStream{}
{}

std::thread listenerThread{};
bool AudioStream::Running() const
{ 
    return (audioDeviceID > 0 && ffmpegStream.isOpen()) || listenerThread.joinable();
}


bool AudioStream::Start(audio::AudioConfig const audioSettings, uint16_t const port)
{
    if(Running())
        Shutdown();

    // if(listenerThread.joinable())
    //     listenerThread.join();

    listenerThread = std::thread([&, audioData = audioSettings, p = port] {
        // audioSocket = createUdpSocket(port);
        auto const uri = "tcp://0.0.0.0:" + std::to_string(port);

        if(ffmpegStream.openStream(uri))
        {
            auto desiredAudio = SDL_AudioSpec{};
            //https://www.vocitec.com/docs-tools/blog/sampling-rates-sample-depths-and-bit-rates-basic-audio-concepts
            //should read up on relationship with sample rate and freq.
            desiredAudio.freq = audioSettings.sampleRateFrequency;
            desiredAudio.channels = audioSettings.channelCount;
            desiredAudio.format = AUDIO_S16LSB;
            desiredAudio.samples = audioSettings.sampleCount;
            desiredAudio.callback = nullptr;
            desiredAudio.userdata = nullptr;
            audioDeviceID = initialiseAudioDevice(desiredAudio);
            if(audioDeviceID > 0)
            {
                audioQueueThread = startAudioThread(desiredAudio, ffmpegStream);
                //started up now, ready for audio stream
                SDL_PauseAudioDevice(audioDeviceID, 0);
                return true;
            }
            else
            {
                queueThreadRunning.clear();
                ffmpegStream.closeStream();
                return false;
            }
        }
        else
            return false;
    });

    // audioSocket = createUdpSocket(port);
    auto const uri = "tcp://0.0.0.0:" + std::to_string(port);

    // if(ffmpegStream.openStream(uri))
    // {
    //     auto desiredAudio = SDL_AudioSpec{};
    //     //https://www.vocitec.com/docs-tools/blog/sampling-rates-sample-depths-and-bit-rates-basic-audio-concepts
    //     //should read up on relationship with sample rate and freq.
    //     desiredAudio.freq = audioSettings.sampleRateFrequency;
    //     desiredAudio.channels = audioSettings.channelCount;
    //     desiredAudio.format = AUDIO_S16LSB;
    //     desiredAudio.samples = audioSettings.sampleCount;
    //     desiredAudio.callback = nullptr;
    //     desiredAudio.userdata = nullptr;
    //     audioDeviceID = initialiseAudioDevice(desiredAudio);
    //     if(audioDeviceID > 0)
    //     {
    //         audioQueueThread = startAudioThread(desiredAudio, ffmpegStream);
    //         //started up now, ready for audio stream
    //         SDL_PauseAudioDevice(audioDeviceID, 0);
    //         return true;
    //     }
    //     else
    //     {
    //         queueThreadRunning.clear();
    //         ffmpegStream.closeStream();
    //         return false;
    //     }
    // }
    // else
    //     return false;

    return true;
}

void AudioStream::Shutdown()
{
    if(!ffmpegStream.isOpen())
        return;

    ffmpegStream.closeStream();

    queueThreadRunning.clear();
    audioQueueThread.join();
    if(listenerThread.joinable())
        listenerThread.join();
        
    std::cout << "Pausing audioDeviceID " << audioDeviceID << "...\n\n";
    SDL_PauseAudioDevice(audioDeviceID, 1);
    SDL_CloseAudioDevice(audioDeviceID);
    audioDeviceID = 0;

    std::cout << "AudioStream shutdown...\n\n";
}

// int32_t AudioStream::createUdpSocket(uint16_t const port)
// {
//     auto sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

//     if(sock >= 0)
//     {
//         sockaddr_in addr {0};
//         addr.sin_family = AF_INET;
//         addr.sin_port = htons(port);
//         addr.sin_addr.s_addr = INADDR_ANY;

//         auto bindCheck = bind(sock, (sockaddr*)&addr, sizeof(addr));
//         if(bindCheck < 0)
//         {
//             std::cout << "Failed to bind UDP socket - " << strerror(errno) << "\n";
//             close(sock);
//             return -1;
//         }
//         else
//             return sock;
//     }
//     else
//     {
//         std::cout << "Failed to create UDP Socket - " << strerror(errno) << "\n";
//         return -1;
//     }
// }