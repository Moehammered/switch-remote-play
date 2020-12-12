#include "PcmStream.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#include <iostream>
#include <atomic>
#include "AudioPlayback.h"

std::atomic_bool audioStreamOn {false};

void ReadAudioStream(std::atomic_bool& running, int const & audioSocket)
{
    AudioPlayback audioStream{};
    while(running && audioSocket > 0)
    {
        auto result = audioStream.ReadPackets(audioSocket);
        if(result > 0)
            audioStream.Play();
        else
            running = false;
    }

    std::cout << "Audio stream stopped - socket ID: " << audioSocket << "\n";
    audioStream.Cleanup();
}

PcmStream::PcmStream(uint16_t port)
 : audioSocket{0}, audioPort{port},
   audioThread{}
{ }

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
    if(!audioStreamOn)
    {
        audioSocket = CreateSocket(2224);
        auto processStream = [&]{
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
    audioSocket = 0;

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
            std::cout << "Failed to bind UDP socket - " << strerror(errno) << "\n";
            close(sock);
            return -1;
        }
        else
            return sock;
    }
    else
    {
        std::cout << "Failed to create UDP Socket - " << strerror(errno) << "\n";
        return -1;
    }
}