#include "AudioStream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include <switch.h>

#include <netdb.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>

#include "../InputThread.h"

auto constexpr SAMPLERATE = 48000;
auto constexpr CHANNELCOUNT = 2;
auto constexpr FRAMERATE = (1000 / 30);
auto constexpr SAMPLECOUNT = (SAMPLERATE / FRAMERATE);
auto constexpr BYTESPERSAMPLE = 2;

auto constexpr MIN_LEFT_SOCK = 2000; // TODO: Tweak those
auto constexpr MAX_LEFT_SOCK = 5000;
char bs_buf[MAX_LEFT_SOCK];


auto constexpr BUF_COUNT = 5;

auto constexpr data_size = 1920; //(SAMPLECOUNT * CHANNELCOUNT * BYTESPERSAMPLE);

auto constexpr inrate = 16000;
auto constexpr outrate = 48000;

auto constexpr fact = outrate/inrate;
auto constexpr in_buf_size = data_size/fact;
auto constexpr buffer_size = (data_size + 0xfff) & ~0xfff;

AudioOutBuffer audiobuf[BUF_COUNT];
u8 *buf_data[BUF_COUNT];
int curBuf = 0;

AudioOutBuffer *audout_released_buf;
int audout_filled = 0;

int constexpr swapBuffer(int const currentIndex, int const max)
{
    return (currentIndex + 1) % max;
}

void diep(const char *s)
{
    perror(s);
    while(1);
}

int setup_socket(uint16_t port)
{
    struct sockaddr_in si_me;
    int s;

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        diep("socket");

    memset((char *)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, (sockaddr*)&si_me, sizeof(si_me)) == -1)
        diep("bind");
    return s;
}

void play_buf(int buffer_size, int data_size)
{
    // have enough in the audio buffer to play some sound
    if (audout_filled >= BUF_COUNT)
    {
        u32 released_count;
        audoutWaitPlayFinish(&audout_released_buf, &released_count, UINT64_MAX);
        audout_filled -= released_count;
        if(audout_filled < 0)
            audout_filled = 0;
    }

    audiobuf[curBuf].buffer = buf_data[curBuf];
    audiobuf[curBuf].buffer_size = buffer_size;
    audiobuf[curBuf].data_size = data_size;
    audoutAppendAudioOutBuffer(&audiobuf[curBuf]);

    audout_filled++;

    curBuf = swapBuffer(curBuf, BUF_COUNT);
}

// out_buf has to be in_buf_size*fact*2 big
void resample(unsigned short const * const in_buf, int const in_buf_size, unsigned short *out_buf, int const fact)
{
    auto constexpr channels = 2;
    auto const bufferStride = in_buf_size / sizeof(unsigned short);

    for (auto i = 0U; i < bufferStride; i += channels)
    {
        int out_base = i * fact;

        for (int j = 0; j < fact; j++)
        {
            for (int c = 0; c < channels; c++)
                out_buf[out_base++] = in_buf[i + c];
        }
    }
}

void audioHandlerLoop(std::atomic_int32_t& state, int const & audioSocket)
{
    curBuf = 0;
    char in_buf[in_buf_size];

    for (int ind = 0; ind < BUF_COUNT; ind++)
        buf_data[ind] = (u8*)memalign(0x1000, buffer_size);

    for(auto& audioBuffer : audiobuf)
    {
        audioBuffer.next = nullptr;
        audioBuffer.buffer = nullptr;
        audioBuffer.buffer_size = 0;
        audioBuffer.data_size = 0;
        audioBuffer.data_offset = 0;
    }

    while (appletMainLoop() && audioSocket >= 0 && state.load() == StreamState::ACTIVE)
    {
        struct sockaddr_in si_other;
        socklen_t slen = sizeof(si_other);
        int ret = recvfrom(audioSocket, in_buf, sizeof(in_buf), 0, (sockaddr*)&si_other, &slen);

        if (ret < 0)
        {
            // perror("recv failed:");
            continue;
        }
        if (ret != sizeof(in_buf))
        {
            // printf("Bad input %d\n", ret);
            // This may or may not be bad

            //printf("not %d big! %d\n", data_size, ret);
            //continue;
        }

        resample((unsigned short*) in_buf, sizeof(in_buf), (unsigned short*) buf_data[curBuf], fact);
        play_buf(buffer_size, data_size);
    }

    // flush the audio buffers
    auto released_count = 0U;
    audoutWaitPlayFinish(&audout_released_buf, &released_count, UINT64_MAX);
    for (int ind = 0; ind < BUF_COUNT; ind++)
        free(buf_data[ind]);
	
    close(audioSocket);
    shutdown(audioSocket, SHUT_RDWR);

    // reset globals
    curBuf = 0;
    audout_filled = 0;
    for(auto& audioBuffer : audiobuf)
    {
        audioBuffer.next = nullptr;
        audioBuffer.buffer = nullptr;
        audioBuffer.buffer_size = 0;
        audioBuffer.data_size = 0;
        audioBuffer.data_offset = 0;
    }
}


AudioStream::AudioStream(uint16_t port)
    : udpSocket{0}, inputBuffer(in_buf_size, 0), 
    resampleBuffers(BUF_COUNT, std::vector<char>(buffer_size, 0)),
    audioBuffers{BUF_COUNT},
    audioBufferIndex{0}
{
    udpSocket = setup_socket(port);
}

bool AudioStream::Ready() const
{
    return false;
}

void AudioStream::Start()
{

}

void AudioStream::Shutdown()
{

}