#include "Connection.h"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
//#include <WS2tcpip.h>
constexpr int INVALID_SOCKET = -1;

void ZeroMemory(void* data, size_t size)
{
    memset(data, 0, size);
}

Connection::Connection(uint16_t port)
    :   listeningSocket{INVALID_SOCKET},
        connectedSocket{INVALID_SOCKET},
        serverAddr{ 0 }, clientAddr{ 0 }, portNo{ port }
{
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    ZeroMemory(&clientAddr, sizeof(clientAddr));

    int result = 0;

    auto serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
//    if (serverSocket == INVALID_SOCKET)
//        std::cout << "Failed to create socket for server: " << WSAGetLastError() << std::endl;
    if(serverSocket < 0)
        std::cout << "Failed to create socket for server: " << strerror(errno) << std::endl;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    
    int tr = 1;
    setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&tr,sizeof(int));
    result = bind(serverSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result < 0)
    {
//      std::cout << "Failed to bind socket: " << WSAGetLastError() << std::endl;
        std::cout << "Failed to bind socket: " << strerror(errno) << std::endl;
//        closesocket(serverSocket);
        close(serverSocket);
    }

    result = listen(serverSocket, 1);
    if (result < 0)
    {
        close(serverSocket);
    }

    listeningSocket = serverSocket;
}

bool Connection::Ready() const
{
    return listeningSocket >= 0;
}

/// <summary>
/// Waits to accept connections on its listening socket port. Configured to default setting, so it will block until a connection is made.
/// </summary>
/// <returns></returns>
bool Connection::WaitForConnection()
{
    std::cout << "waiting for connection to server..." << std::endl << std::endl;

    sockaddr_in clientInfo;
    socklen_t infoSize = sizeof(clientInfo);

    ZeroMemory(&clientInfo, infoSize);

    auto connectionSocket = accept(listeningSocket, (sockaddr*)&clientInfo, &infoSize);
    if (connectionSocket < 0)
    {
//        std::cout << "Failed to accept connection: " << WSAGetLastError() << std::endl;
        std::cout << "Failed to accept connection: " << strerror(errno) << std::endl;
        return false;
    }

    connectedSocket = connectionSocket;
    clientAddr = clientInfo;

    return true;
}

bool Connection::ConnectTo(std::string ip)
{
    auto connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (connectionSocket < 0)
    {
//        std::cout << "Failed to create a connection socket to connect to: " << WSAGetLastError() << std::endl;
        return false;
    }
    else
    {
        sockaddr_in clientInfo = {};
        int infoSize = sizeof(clientInfo);
        ZeroMemory(&clientInfo, infoSize);

        clientInfo.sin_family = AF_INET;
        clientInfo.sin_port = htons(portNo);
        inet_pton(AF_INET, ip.c_str(), &clientInfo.sin_addr.s_addr);

        auto result = connect(connectionSocket, (const sockaddr*)&clientInfo, infoSize);
        if (result < 0)
        {
//            std::cout << "Failed to connect: " << WSAGetLastError() << std::endl;
//            closesocket(connectionSocket);
            close(connectionSocket);
            return false;
        }
        else
        {
            clientAddr = clientInfo;
            connectedSocket = connectionSocket;
            return true;
        }
    }
}

SOCKET const& Connection::ConnectedSocket() const
{
    return connectedSocket;
}

SOCKET const& Connection::ListeningSocket() const
{
    return listeningSocket;
}

std::string const Connection::ConnectedIP() const
{
    char ipBuffer[25];
    inet_ntop(AF_INET, &clientAddr.sin_addr, ipBuffer, 25);
    return std::string(ipBuffer);
}

bool Connection::Disconnect()
{
    if (connectedSocket != INVALID_SOCKET)
    {
        auto connecterClosed = close(connectedSocket);
        if (connecterClosed >= 0)
            connectedSocket = INVALID_SOCKET;
        else
            return false;
    }

    return connectedSocket == INVALID_SOCKET;
}

bool Connection::Close()
{
    if (connectedSocket >= 0)
    {
        std::cout << "closing connected socket\n\n";
        auto connecterClosed = close(connectedSocket);
        if (connecterClosed >= 0)
            connectedSocket = INVALID_SOCKET;
        else
            return false;
    }
    if (listeningSocket >= 0)
    {
        std::cout << "closing listener socket\n\n";
        auto listenerClosed = close(listeningSocket);
        if (listenerClosed >= 0)
            listeningSocket = INVALID_SOCKET;
        else
        {
            std::cout << "Failed to close listener socket: " << strerror(errno) << std::endl;
            return false;
        }
    }

    return connectedSocket == INVALID_SOCKET && listeningSocket == INVALID_SOCKET;
}

bool Connection::Shutdown()
{
    if (connectedSocket != INVALID_SOCKET)
    {
        auto socketShut = shutdown(connectedSocket, SHUT_RDWR);
        if (socketShut >= 0)
            connectedSocket = INVALID_SOCKET;
        else
            return false;
    }
    if (listeningSocket != INVALID_SOCKET)
    {
        auto socketShut = shutdown(listeningSocket, SHUT_RDWR);
        if (socketShut >= 0)
            listeningSocket = INVALID_SOCKET;
        else
            return false;
    }

    return connectedSocket == INVALID_SOCKET && listeningSocket == INVALID_SOCKET;
}
