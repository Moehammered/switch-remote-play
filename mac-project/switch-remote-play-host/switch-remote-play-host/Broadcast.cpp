#include "Broadcast.h"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
//#include <ws2tcpip.h>

constexpr int INVALID_SOCKET = -1;

Broadcast::Broadcast(std::string subnet, uint16_t port)
    : broadcastSocket{ INVALID_SOCKET }, receiverSocket{ INVALID_SOCKET },
      broadcastAddr{0}, receiveAddr{0}
{
    sockaddr_in addr{ 0 };
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    // need to find a way to automatically get the (wireless)local area network IP and subnet mask to make the broadcast address
    inet_pton(AF_INET, subnet.c_str(), &addr.sin_addr.s_addr);
    
    // broadcast addr for all network IPs (from local network, to ISP network)
    //broadcastAddr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST); 

    auto broadcastSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    bool socketOption = true;
    int sizeofOption = sizeof(socketOption);
    //must setup the socket to explicitly want to broadcast
    auto result = setsockopt(broadcastSock, SOL_SOCKET, SO_BROADCAST, (char*)&socketOption, sizeofOption);
    if (result < 0)
    {
//        std::cout << "Failed to set socket option to broadcast: " << WSAGetLastError() << std::endl;
//        closesocket(broadcastSock);
        close(broadcastSock);
    }
    else
    {
        broadcastSocket = broadcastSock;
        broadcastAddr = addr;
    }

    auto receiveSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (receiveSock >= 0)
    {
        sockaddr_in recvAddr{ 0 };
        recvAddr.sin_port = htons(port);
        recvAddr.sin_family = AF_INET;
        recvAddr.sin_addr.s_addr = INADDR_ANY;

        auto bindRes = bind(receiveSock, (const sockaddr*)&recvAddr, sizeof(recvAddr));
        if (bindRes < 0)
        {
//            std::cout << "Failed to bind receiving socket: " << WSAGetLastError() << std::endl;
//            closesocket(receiveSock);
            close(receiveSock);
        }
        else
        {
            receiverSocket = receiveSock;
        }
    }
}

bool Broadcast::ReadyToSend()
{
    return broadcastSocket > INVALID_SOCKET;
}

bool Broadcast::ReadyToRecv()
{
    return receiverSocket > INVALID_SOCKET;
}

bool Broadcast::Send(char const* const data, int size)
{
	if (data == nullptr || size <= 0)
		return false;

	auto result = sendto(broadcastSocket, data, size, 0,
		(const sockaddr*)&broadcastAddr, ADDR_SIZE);

    return result >= 0;
}

bool Broadcast::Recv(std::string& data)
{
    char buffer[255]{};
    for (auto& c : buffer)
        c = 0;
    socklen_t size = sizeof(receiveAddr);
    auto res = recvfrom(receiverSocket, buffer, 255, 0, (sockaddr*)&receiveAddr, &size);

    if (res >= 0)
    {
        for (auto i{ res }; i < 255; ++i)
            buffer[i] = 0;
        data = std::string{ buffer };
        return true;
    }
    else
    {
        data = "";
        return false;
    }
}

std::string const Broadcast::ReceivedIP() const
{
    char ipBuffer[25];
    inet_ntop(AF_INET, &receiveAddr.sin_addr, ipBuffer, 25);
    return std::string(ipBuffer);
}

bool Broadcast::Close()
{
    if (broadcastSocket != INVALID_SOCKET)
    {
        auto socketClosed = close(broadcastSocket);
        if (socketClosed >= 0)
            broadcastSocket = INVALID_SOCKET;
        else
            return false;
    }
    if (receiverSocket != INVALID_SOCKET)
    {
        auto socketClosed = close(receiverSocket);
        if (socketClosed >= 0)
            receiverSocket = INVALID_SOCKET;
        else
            return false;
    }

    return broadcastSocket == INVALID_SOCKET && receiverSocket == INVALID_SOCKET;
}

bool Broadcast::Shutdown()
{
    if (broadcastSocket != INVALID_SOCKET)
    {
        auto socketShut = shutdown(broadcastSocket, SHUT_RDWR);
        if (socketShut >= 0)
            broadcastSocket = INVALID_SOCKET;
        else
            return false;
    }
    if (receiverSocket != INVALID_SOCKET)
    {
        auto socketShut = shutdown(receiverSocket, SHUT_RDWR);
        if (socketShut >= 0)
            receiverSocket = INVALID_SOCKET;
        else
            return false;
    }

    return broadcastSocket == INVALID_SOCKET && receiverSocket == INVALID_SOCKET;
}
