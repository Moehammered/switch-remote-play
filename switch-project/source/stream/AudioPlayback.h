#ifndef __AUDIOPLAYBACK_H__
#define __AUDIOPLAYBACK_H__

#include "../network/NetworkData.h"
#include <vector>
extern "C"
{
    #include <switch/services/audout.h>
}

struct alignas(16) AudioConfig
{
    uint32_t    sampleRate;
    uint32_t    framerate;
    uint32_t    channelCount;
    uint32_t    bitrate;
};

auto constexpr audioConfigSize = {sizeof(AudioConfig)};

class AudioPlayback
{
public:
    AudioPlayback();

    AudioPlayback(AudioConfig config, uint32_t swapBuffers);

    int32_t ReadPackets(int32_t const & udpSocket);

    void Play();

    void Cleanup();

private:
    AudioConfig settings;
    uint32_t packetsPerFrame;
    uint32_t bufferCount;
    uint32_t bufferIndex;
    std::vector<char*> packetBuffers;
    std::vector<AudioOutBuffer> switchAudioBuffers;
    AudioOutBuffer* releaseBuffer;

    static uint32_t constexpr audioBufferAlignment {0x1000};
};

#endif
