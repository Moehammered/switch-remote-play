#include "PcmStream.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <atomic>
#include <string.h>
#include <malloc.h>
#include <vector>

extern "C"
{
    #include <switch/services/audout.h>
}

auto constexpr SampleRate {48000};
auto constexpr framerate {100};
auto constexpr ChannelCount {2}; // stereo
auto constexpr BitRate {16};
auto constexpr SamplesPerFrame {SampleRate / framerate};
auto constexpr PacketsPerFrame {SamplesPerFrame * BitRate/8 * ChannelCount};

std::atomic_bool audioStreamOn {false};

auto constexpr BufferCount {3};

// char packets[1024];

std::vector<AudioOutBuffer> switchAudioBuffers {};
AudioOutBuffer *releaseBuffer {nullptr};
std::vector<char*> audioBuffers {};
auto constexpr BufferAlignment {0x1000};
int bufferIndex {0};

void SetupBuffers()
{
    if(audioBuffers.size() > 0)
    {
        for(auto b : audioBuffers)
            free(b);
        
        audioBuffers.clear();
    }

    switchAudioBuffers.clear();

    for(auto i = 0; i < BufferCount; ++i)
    {
        audioBuffers.push_back((char*)memalign(BufferAlignment, PacketsPerFrame));
        switchAudioBuffers.push_back(
            AudioOutBuffer
            {
                .next = nullptr, .buffer = nullptr, 
                .buffer_size = 0, .data_size = 0, .data_offset = 0
            }
        );
    }

    bufferIndex = 0;
}

int PlayBuffer(int buffer_size, int data_size, void* buffer, int bufferInd)
{
    switchAudioBuffers[bufferInd].buffer = buffer;
    switchAudioBuffers[bufferInd].buffer_size = buffer_size;
    switchAudioBuffers[bufferInd].data_size = data_size;
    audoutPlayBuffer(&switchAudioBuffers[bufferInd], &releaseBuffer);

    if(++bufferInd >= BufferCount)
        bufferInd = 0;

    return bufferInd;
}

void Cleanup()
{
    auto flushed = false;
    audoutFlushAudioOutBuffers(&flushed);
    for(auto b : audioBuffers)
        free(b);
    
    audioBuffers.clear();
    switchAudioBuffers.clear();
}

void ReadAudioStream(std::atomic_bool& running, int const & audioSocket)
{
    socklen_t slen = sizeof(sockaddr_in);
    while(running && audioSocket > 0)
    {
        auto totalRead = 0;
        do
        {
            sockaddr_in si_other;
            auto dst = audioBuffers[bufferIndex] + totalRead;
            auto lastRead = recvfrom(audioSocket, dst, PacketsPerFrame-totalRead, 0, (sockaddr*)&si_other, &slen);
            if(lastRead > 0)
                totalRead += lastRead;
            else if(lastRead == 0)
            {
                totalRead = -1;
                break;
            }
        } while (totalRead < PacketsPerFrame);

        if(totalRead != -1)
            bufferIndex = PlayBuffer(PacketsPerFrame, PacketsPerFrame, audioBuffers[bufferIndex], bufferIndex);
        else
        {
            // stream dead?
            running = false;
        }
    }

    Cleanup();
}

PcmStream::PcmStream(uint16_t port)
 : audioSocket{-1}, audioThread{}
{
    audioSocket = CreateSocket(port);
}

bool PcmStream::Ready() const
{
    return audioSocket >= 0;
}

bool PcmStream::Running() const
{
    return audioStreamOn.load(std::memory_order_relaxed);
}

void PcmStream::Start()
{
    if(!audioStreamOn && audioSocket != -1)
    {
        auto processStream = [&]{
            SetupBuffers();
            audioStreamOn = true;
            ReadAudioStream(audioStreamOn, audioSocket);
        };

        if(audioThread.joinable())
            audioThread.join();

        audioThread = std::thread(processStream);
    }
}

void PcmStream::Shutdown()
{
    audioStreamOn.store(false, std::memory_order_release);
    shutdown(audioSocket, SHUT_RDWR);
    close(audioSocket);
    audioSocket = -1;

    if(audioThread.joinable())
        audioThread.join();
}

void PcmStream::Stop()
{
    audioStreamOn = false;
    if(audioThread.joinable())
        audioThread.join();
}

int PcmStream::CreateSocket(uint16_t port)
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
            std::cout << "Failed to bind UDP socket - " << strerror(errno) << std::endl;
            close(sock);
            return -1;
        }
        else
            return sock;
    }
    else
    {
        std::cout << "Failed to create UDP Socket - " << strerror(errno) << std::endl;
        return -1;
    }
}