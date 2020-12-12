#include "AudioPlayback.h"
#include <malloc.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>

AudioConfig constexpr DefaultAudioSettings{
    .sampleRate {48000},
    .framerate {100},
    .channelCount {2},
    .bitrate {16}
};

AudioPlayback::AudioPlayback()
    : settings{DefaultAudioSettings}, 
      packetsPerFrame {settings.sampleRate/settings.framerate * settings.bitrate/8 * settings.channelCount},
      bufferCount {5}, bufferIndex {0},
      packetBuffers{}, switchAudioBuffers{},
      releaseBuffer {nullptr}
{
    for(auto i = 0U; i < bufferCount; ++i)
    {
        AudioOutBuffer defaultBuffer
        {
            .next {nullptr}, .buffer {nullptr},
            .buffer_size {0}, .data_size {0}, .data_offset {0}
        };
        switchAudioBuffers.push_back(defaultBuffer);

        auto buffer = (char*)memalign(AudioBufferAlignment, packetsPerFrame);
        memset(buffer, 0, packetsPerFrame);
        packetBuffers.push_back(buffer);
    }
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
            .buffer_size {0}, .data_size {0}, .data_offset {0}
        };
        switchAudioBuffers.push_back(defaultBuffer);

        auto buffer = (char*)memalign(AudioBufferAlignment, packetsPerFrame);
        packetBuffers.push_back(buffer);
    }
}

int32_t AudioPlayback::ReadPackets(int32_t const & udpSocket)
{
    auto totalRead = 0;
    socklen_t slen = sizeof(sockaddr_in);
    do
    {
        sockaddr_in si_other;
        auto dst = packetBuffers[bufferIndex] + totalRead;
        auto lastRead = recvfrom(udpSocket, dst, packetsPerFrame-totalRead, 0, (sockaddr*)&si_other, &slen);
        if(lastRead > 0)
            totalRead += lastRead;
        else if(udpSocket <= 0)
        {
            std::cout << "Audio Stream - Received Error Res: " << lastRead << "\n";
            return -1;
        }
    } while (totalRead < packetsPerFrame && udpSocket > 0);

    return totalRead;
}

void AudioPlayback::Play()
{
    switchAudioBuffers[bufferIndex].buffer = packetBuffers[bufferIndex];
    switchAudioBuffers[bufferIndex].buffer_size = packetsPerFrame;
    switchAudioBuffers[bufferIndex].data_size = packetsPerFrame;
    audoutPlayBuffer(&switchAudioBuffers[bufferIndex], &releaseBuffer);

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