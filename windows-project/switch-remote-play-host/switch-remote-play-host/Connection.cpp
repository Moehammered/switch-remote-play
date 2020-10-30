#include "Connection.h"
#include <iostream>
#include <WS2tcpip.h>

Connection::Connection(uint16_t port)
    :   listeningSocket{INVALID_SOCKET},
        connectedSocket{INVALID_SOCKET},
        serverAddr{ 0 }, clientAddr{ 0 }, portNo{ port }
{
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    ZeroMemory(&clientAddr, sizeof(clientAddr));

    int result = 0;

    auto serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (serverSocket == INVALID_SOCKET)
        std::cout << "Failed to create socket for server: " << WSAGetLastError() << std::endl;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    result = bind(serverSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        std::cout << "Failed to bind socket: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
    }

    result = listen(serverSocket, 1);
    if (result == SOCKET_ERROR)
    {
        std::cout << "Failed to set the socket into listening mode: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
    }

    listeningSocket = serverSocket;
}

bool Connection::Ready() const
{
    return listeningSocket != INVALID_SOCKET;
}

/// <summary>
/// Waits to accept connections on its listening socket port. Configured to default setting, so it will block until a connection is made.
/// </summary>
/// <returns></returns>
bool Connection::WaitForConnection()
{
    std::cout << "waiting for connection to server..." << std::endl << std::endl;

    sockaddr_in clientInfo;
    int infoSize = sizeof(clientInfo);

    ZeroMemory(&clientInfo, infoSize);

    auto connectionSocket = accept(listeningSocket, (sockaddr*)&clientInfo, &infoSize);
    if (connectionSocket == INVALID_SOCKET)
    {
        std::cout << "Failed to accept connection: " << WSAGetLastError() << std::endl;
        return false;
    }

    connectedSocket = connectionSocket;
    clientAddr = clientInfo;

    return true;
}

bool Connection::ConnectTo(std::string ip)
{
    auto connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (connectionSocket == INVALID_SOCKET)
    {
        std::cout << "Failed to create a connection socket to connect to: " << WSAGetLastError() << std::endl;
        return false;
    }
    else
    {
        sockaddr_in clientInfo = {};
        int infoSize = sizeof(clientInfo);
        ZeroMemory(&clientInfo, infoSize);

        clientInfo.sin_family = AF_INET;
        clientInfo.sin_port = htons(portNo);
        inet_pton(AF_INET, ip.c_str(), &clientInfo.sin_addr.S_un.S_addr);

        auto result = connect(connectionSocket, (const sockaddr*)&clientInfo, infoSize);
        if (result == SOCKET_ERROR)
        {
            std::cout << "Failed to connect: " << WSAGetLastError() << std::endl;
            closesocket(connectionSocket);
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

bool Connection::Close()
{
    if (connectedSocket != INVALID_SOCKET)
    {
        auto connecterClosed = closesocket(connectedSocket);
        if (connecterClosed != SOCKET_ERROR)
            connectedSocket = INVALID_SOCKET;
        else
            return false;
    }
    if (listeningSocket != INVALID_SOCKET)
    {
        auto listenerClosed = closesocket(listeningSocket);
        if (listenerClosed != SOCKET_ERROR)
            listeningSocket = INVALID_SOCKET;
        else
            return false;
    }

    return connectedSocket == INVALID_SOCKET && listeningSocket == INVALID_SOCKET;
}

bool Connection::Shutdown()
{
    if (connectedSocket != INVALID_SOCKET)
    {
        auto socketShut = shutdown(connectedSocket, SD_BOTH);
        if (socketShut != SOCKET_ERROR)
            connectedSocket = INVALID_SOCKET;
        else
            return false;
    }
    if (listeningSocket != INVALID_SOCKET)
    {
        auto socketShut = shutdown(listeningSocket, SD_BOTH);
        if (socketShut != SOCKET_ERROR)
            listeningSocket = INVALID_SOCKET;
        else
            return false;
    }

    return connectedSocket == INVALID_SOCKET && listeningSocket == INVALID_SOCKET;
}
