#ifndef __AUDIOPLAYBACK_H__
#define __AUDIOPLAYBACK_H__

#include <vector>
#include "../network/NetworkData.h"
extern "C"
{
    #include <switch/services/audout.h>
}

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

    static uint32_t constexpr AudioBufferAlignment {0x1000};
};

#endif
