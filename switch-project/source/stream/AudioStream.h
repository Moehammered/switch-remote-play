#ifndef __AUDIOSTREAM_H__
#define __AUDIOSTREAM_H__

#include <atomic>
#include <vector>
extern "C"
{
    #include <switch/services/audout.h>
}

class AudioStream
{
public:
    AudioStream(uint16_t port);

    bool Ready() const;

    void Start();

    void Shutdown();

private:
    int udpSocket;
    std::vector<char> inputBuffer;
    std::vector<std::vector<char>> resampleBuffers;
    std::vector<AudioOutBuffer> audioBuffers;
    int audioBufferIndex;
};

int constexpr swapBuffer(int const currentIndex, int const max);

void diep(const char *s);

int setup_socket(uint16_t port);

void play_buf(int buffer_size, int data_size);

// out_buf has to be in_buf_size*fact*2 big
void resample(unsigned short const * const in_buf, int const in_buf_size, unsigned short *out_buf, int const fact);

void audioHandlerLoop(std::atomic_int32_t& state, int const & audioSocket);

#endif
