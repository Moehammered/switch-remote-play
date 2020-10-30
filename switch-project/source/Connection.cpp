#include "Connection.h"
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

Connection::Connection(uint16_t port)
    :   listeningSocket{-1},
        connectedSocket{-1},
        serverAddr{0}, clientAddr{0}, portNo{port}
{
    int result = 0;

    auto serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (serverSocket < 0)
        std::cout << "Failed to create socket for server: " << strerror(errno) << std::endl;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    result = bind(serverSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result < 0)
    {
        std::cout << "Failed to bind socket: " << strerror(errno) << std::endl;
        close(serverSocket);
    }

    result = listen(serverSocket, 1);
    if (result < 0)
    {
        std::cout << "Failed to set the socket into listening mode: " << strerror(errno) << std::endl;
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

    sockaddr_in clientInfo{0};
    socklen_t infoSize = sizeof(clientInfo);

    auto connectionSocket = accept(listeningSocket, (sockaddr*)&clientInfo, &infoSize);
    if (connectionSocket < 0)
    {
        std::cout << "Failed to accept connection: " << strerror(errno) << std::endl;
        return false;
    }

    connectedSocket = connectionSocket;
    clientAddr = clientInfo;

    return true;
}

int const& Connection::ConnectedSocket() const
{
    return connectedSocket;
}

int const& Connection::ListeningSocket() const
{
    return listeningSocket;
}

std::string const Connection::ConnectedIP() const
{
    char ipBuffer[25];
    inet_ntop(AF_INET, &clientAddr.sin_addr, ipBuffer, 25);
    return std::string(ipBuffer);
}

bool Connection::Close()
{
    if (connectedSocket >= 0)
    {
        auto connecterClosed = close(connectedSocket);
        if (connecterClosed >= 0)
            connectedSocket = -1;
        else
            return false;
    }
    if (listeningSocket >= 0)
    {
        auto listenerClosed = close(listeningSocket);
        if (listenerClosed >= 0)
            listeningSocket = -1;
        else
            return false;
    }

    return connectedSocket < 0 && listeningSocket < 0;
}

bool Connection::Shutdown()
{
    if (connectedSocket >= 0)
    {
        auto socketShut = shutdown(connectedSocket, SHUT_RDWR);
        if (socketShut >= 0)
            connectedSocket = -1;
        else
            return false;
    }
    if (listeningSocket >= 0)
    {
        auto socketShut = shutdown(listeningSocket, SHUT_RDWR);
        if (socketShut >= 0)
            listeningSocket = -1;
        else
            return false;
    }

    return connectedSocket < 0 && listeningSocket < 0;
}

