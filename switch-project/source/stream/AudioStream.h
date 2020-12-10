#ifndef __AUDIOSTREAM_H__
#define __AUDIOSTREAM_H__

#include <atomic>
#include <vector>
#include <thread>
extern "C"
{
    #include <switch/services/audout.h>
}

class AudioStream
{
public:
    AudioStream(uint16_t port);

    bool Ready() const;

    bool Running() const;

    void Start();

    void Shutdown();

    int udpSocket;
    void Play(int const & socket, std::atomic_bool& running);

private:
    std::vector<uint8_t> inputBuffer;
    std::vector<std::vector<uint16_t>> resampleBuffers alignas(0x1000);
    std::vector<AudioOutBuffer> audioBuffers;
    uint32_t audioBufferIndex;
    std::thread playbackThread;

    std::vector<uint16_t> ResampleInput(std::vector<uint8_t> const & packetData, int factor);

    void AppendAudioOut(int bufferLocation);

};

void RunAudioStream(AudioStream& stream, std::atomic_bool& running);

int constexpr swapBuffer(int const currentIndex, int const max);

void diep(const char *s);

int setup_socket(uint16_t port);

void play_buf(int buffer_size, int data_size);

// out_buf has to be in_buf_size*fact*2 big
void resample(unsigned short const * const in_buf, int const in_buf_size, unsigned short *out_buf, int const fact);

void audioHandlerLoop(std::atomic_int32_t& state, int const & audioSocket);

#endif
