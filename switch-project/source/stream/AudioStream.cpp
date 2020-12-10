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
#include <chrono>
#include "../InputThread.h"

auto constexpr SAMPLERATE = 48000;
auto constexpr CHANNELCOUNT = 2;
auto constexpr FRAMERATE = (1000 / 30);
auto constexpr SAMPLECOUNT = (SAMPLERATE / FRAMERATE);
auto constexpr BYTESPERSAMPLE = 2;

// auto constexpr MIN_LEFT_SOCK = 2000; // TODO: Tweak those
// auto constexpr MAX_LEFT_SOCK = 5000;
// char bs_buf[MAX_LEFT_SOCK];

auto constexpr inrate = 22100;
auto constexpr outrate = 48000;
auto constexpr framerate = 60;
auto constexpr channels = 2; // stereo audio
auto constexpr inputSampleCount = inrate / framerate;

auto constexpr BUF_COUNT = 5;

auto constexpr data_size = inputSampleCount * channels * BYTESPERSAMPLE;
//auto constexpr data_size = 1920;

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
    
    printf("Setup udp socket: %d\n", s);
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
    {
        buf_data[ind] = (u8*)memalign(0x1000, buffer_size);
        // make sure to clean up the memory just in case
        for(auto i = 0; i < buffer_size; ++i)
            buf_data[ind][i] = 0;
    }

    for(auto& audioBuffer : audiobuf)
    {
        audioBuffer.next = nullptr;
        audioBuffer.buffer = nullptr;
        audioBuffer.buffer_size = 0;
        audioBuffer.data_size = 0;
        audioBuffer.data_offset = 0;
    }

    auto constexpr framerateSamples = 120;
    auto fpsIndex = 0U;
    std::chrono::milliseconds framerates[framerateSamples];
    
    auto now = std::chrono::high_resolution_clock::now();
    auto previous = now;

    auto const errorCountPrint = 60;
    auto errorCounter = errorCountPrint;
    // auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - previous);
    while (appletMainLoop() && audioSocket >= 0 && state.load() == StreamState::ACTIVE)
    {
        now = std::chrono::high_resolution_clock::now();
        framerates[fpsIndex++] = std::chrono::duration_cast<std::chrono::milliseconds>(now - previous);
        previous = now;

        if(fpsIndex > framerateSamples)
        {
            fpsIndex = 0;
            auto totalTime = 0.0;
            for(auto& t : framerates)
                totalTime += t.count();
            
            totalTime = totalTime / framerateSamples;
            // printf("Avg frame time over 120 frames: %fms\n", totalTime);
            printf("Avg FPS over 120 frames: %f fps\n", 1000.0/totalTime);
        }

        struct sockaddr_in si_other;
        socklen_t slen = sizeof(si_other);
        int ret = recvfrom(audioSocket, in_buf, sizeof(in_buf), 0, (sockaddr*)&si_other, &slen);

        if (ret < 0)
        {
            // perror("recv failed:");
            continue;
        }
        if (ret != in_buf_size)
        {
            if(--errorCounter == 0)
            {
                printf("Bad sound packet size: %d -- expected: %d\n", ret, in_buf_size);
                errorCounter = errorCountPrint;
            }
            if(ret < in_buf_size)
            {
                for(int i = ret; i < in_buf_size; ++i)
                    in_buf[i] = 0;
            }
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

/// object implementation starts here

std::atomic_bool started;

AudioStream::AudioStream(uint16_t port)
    : udpSocket{-1}, inputBuffer(in_buf_size, 0), 
    resampleBuffers(BUF_COUNT, std::vector<uint16_t>(buffer_size/2, 0)),
    audioBuffers{BUF_COUNT},
    audioBufferIndex{0}, playbackThread{}
{
    udpSocket = setup_socket(port);
    started = false;
    for(auto& audioBuffer : audioBuffers)
    {
        audioBuffer.next = nullptr;
        audioBuffer.buffer = nullptr;
        audioBuffer.buffer_size = 0;
        audioBuffer.data_size = 0;
        audioBuffer.data_offset = 0;
    }
}

bool AudioStream::Ready() const
{
    return udpSocket >= 0 && !started.load(std::memory_order_relaxed);
}

bool AudioStream::Running() const
{
    return udpSocket >= 0 && started.load(std::memory_order_relaxed);
}

void AudioStream::Start()
{
    if(!started)
    {
        started = true;
        playbackThread = std::thread(RunAudioStream, std::ref(*this), std::ref(started));
    }
}

void AudioStream::Shutdown()
{
    if(udpSocket >= 0)
    {
        shutdown(udpSocket, SHUT_RDWR);
        close(udpSocket);
        udpSocket = -1;
    }
    started = false;
    if(playbackThread.joinable())
        playbackThread.join();
}

std::vector<uint16_t> AudioStream::ResampleInput(std::vector<uint8_t> const & packetData, int factor)
{
    auto constexpr channels = 2;
    auto resampled = std::vector<uint16_t>(factor*packetData.size()/2*channels, 0);

    auto const bufferStride = packetData.size() / sizeof(unsigned short);

    auto samplePointer = (uint16_t*)packetData.data();

    for (auto i = 0U; i < bufferStride; i += channels)
    {
        int resamplePoint = i * fact;

        for (int j = 0; j < fact; j++)
        {
            for (int c = 0; c < channels; c++)
                resampled[resamplePoint++] = samplePointer[i + c];
        }
    }

    return resampled;
}

void AudioStream::AppendAudioOut(int bufferLocation)
{
    audioBuffers[bufferLocation].buffer = resampleBuffers[bufferLocation].data();
    audioBuffers[bufferLocation].buffer_size = resampleBuffers[bufferLocation].size()*2;
    audioBuffers[bufferLocation].data_size = data_size;
    audoutAppendAudioOutBuffer(&audioBuffers[bufferLocation]);
    // audoutAppendAudioOutBuffer(audioBuffers.data() + bufferLocation);
}

void AudioStream::Play(int const & socket, std::atomic_bool& running)
{
    while (appletMainLoop() && socket >= 0 && running.load(std::memory_order_relaxed))
    {
        struct sockaddr_in si_other;
        socklen_t slen = sizeof(si_other);
        auto packetData = std::vector<uint8_t>(in_buf_size, 0);
        auto dataSize = recvfrom(socket, packetData.data(), packetData.size(), 0, (sockaddr*)&si_other, &slen);

        if (dataSize == packetData.size() || dataSize >= 0)
        {
            resampleBuffers[audioBufferIndex] = ResampleInput(packetData, fact);

            AppendAudioOut(audioBufferIndex);

            ++audioBufferIndex;
            if(audioBufferIndex >= resampleBuffers.size())
            {
                AudioOutBuffer* releaseBuffer = nullptr;
                uint32_t released_count;
                audoutWaitPlayFinish(&releaseBuffer, &released_count, UINT64_MAX);
                // if(released_count != audioBuffers.size())
                //     printf("Uh-oh");
                audioBufferIndex = 0;
            }
        }
        
        // if (dataSize != packetData.size())
        // {
        //     // ffmpeg might be sampling at a different expected rate
        // }
    }

    // flush the audio buffers
    AudioOutBuffer* releaseBuffer = nullptr;
    uint32_t released_count;
    audoutWaitPlayFinish(&releaseBuffer, &released_count, UINT64_MAX);
    // for (int ind = 0; ind < BUF_COUNT; ind++)
    //     free(buf_data[ind]);
	
    // Shutdown();
}


void RunAudioStream(AudioStream& stream, std::atomic_bool& running)
{
    stream.Play(stream.udpSocket, running);
    if(running)
        stream.Shutdown();
}

