#include "NetworkBroadcast.h"

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <string.h>
#include <iostream>

bool NetworkBroadcast::FindConnectionIP(std::string key, std::string& ip)
{
    ip = "";
    connectionFound = false;

    auto bsocket = CreateBroadcastSocket();
    if(bsocket < 0)
    {
        std::cout << "Error creating socket" << std::endl;
        return false;
    }
    std::cout << "Broadcast socket ready" << std::endl;

    std::string localIP;
    auto lsocket = CreateListenerSocket(localIP);
    if(lsocket < 0)
    {
        std::cout << "Error creating listener socket" << std::endl;
        close(bsocket);
        return false;
    }

    std::cout << "Listener socket ready (Connect to " << localIP << ")" << std::endl;

    std::cout << "Broadcasting '" << key << "' and listening for response" << std::endl;
    int attempts = 1;
    int count = 1;
    while(count <= attempts && !connectionFound)
    {
        std::cout << "Attempt " << count << " Broadcasting... ";
        auto result = Broadcast(bsocket, key);
        if(result < 0)
        {
            std::cout << "Broadcast failed: " << strerror(errno) << std::endl;
        }
        else
        {
            result = Listen(lsocket, ip, key);
            if(result < 0)
            {
                std::cout << "Listening failed. No response or error." << std::endl;
            }
            else
            {
                connectionFound = true;
                std::cout << "Connection response found from " << ip << std::endl;
            }
            
        }
        
        ++count;
    }

    if(!connectionFound)
        std::cout << "No response found on local network." << std::endl;

    close(bsocket);
    close(lsocket);

    return connectionFound;
}

std::string NetworkBroadcast::LastConnectionIP()
{
    return lastConnectionIP;
}

int NetworkBroadcast::CreateBroadcastSocket()
{
    auto bsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(bsock < 0)
    {
        std::cout << "Failed to create UDP socket: " << strerror(errno) << std::endl;
        return -1;
    }

    int optionValue = 1;
    
    auto result = setsockopt(bsock, SOL_SOCKET, SO_BROADCAST, (void *)&optionValue, sizeof(optionValue));
    
    if(result < 0)
    {
        std::cout << "Failed to set socket option SOL_SOCKET, SO_BROADCAST: " << strerror(errno) << std::endl;
        close(bsock);
        return -1;
    }

    return bsock;
}

int NetworkBroadcast::Broadcast(int sock, std::string msg)
{
    sockaddr_in broadcastAddr;
    broadcastAddr.sin_port = htons(networkPort);
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.s_addr = inet_addr("192.168.0.255");
    auto result = sendto(sock, msg.c_str(), msg.length(), SO_BROADCAST, 
                            (const sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

    return result;
}

int NetworkBroadcast::CreateListenerSocket(std::string& localIP)
{
    auto listener = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(listener < 0)
    {
        std::cout << "Failed to create SOCK_STREAM socket: " << strerror(errno) << std::endl;
        return -1;
    }
    
    timeval sockt;
    sockt.tv_sec = 3;
    sockt.tv_usec = 3000;
    auto result = setsockopt(listener, SOL_SOCKET, SO_RCVTIMEO, (const void*)&sockt, sizeof(sockt));

    if(result < 0)
    {
        std::cout << "Failed to set listener socket option SOL_SOCKET, SO_RCVTIMEO: " << strerror(errno) << std::endl;
        close(listener);
        return -1;
    }

    sockaddr_in listenerAddr;
    listenerAddr.sin_family = AF_INET;
    listenerAddr.sin_addr.s_addr = INADDR_ANY;
    listenerAddr.sin_port = htons(networkPort);

    result = bind(listener, (const sockaddr*)&listenerAddr, sizeof(listenerAddr));
    if(result < 0)
    {
        std::cout << "Failed to set bind listener socket: " << strerror(errno) << std::endl;
        close(listener);
        return -1;
    }

    result = listen(listener, 1);
    if(result < 0)
    {
        std::cout << "Failed to set socket into listener mode: " << strerror(errno) << std::endl;
        return -1;
    }

    localIP = inet_ntoa(listenerAddr.sin_addr);
    return listener;
}

int NetworkBroadcast::Listen(int sock, std::string& ip, std::string key)
{
    ip = "";

    sockaddr_in senderInfo;
    senderInfo.sin_port = htons(networkPort);
    senderInfo.sin_family = AF_INET;
    senderInfo.sin_addr.s_addr = INADDR_ANY;
    socklen_t infoSize = sizeof(senderInfo);

    auto csock = accept(sock, (sockaddr*)&senderInfo, &infoSize);
    if(csock < 0)
    {
        std::cout << "Error accepting connection: " << strerror(errno) << std::endl;
        return csock;
    }

    char* msgBuffer = new char[key.length()+1];
    for(size_t i = 0; i < key.length()+1; ++i)
        msgBuffer[i] = 0;

    //auto result = recvfrom(csock, msgBuffer, key.length()+1, 0, (sockaddr*)&senderInfo, &infoSize);
    auto result = recv(csock, msgBuffer, key.length()+1, 0);

    if(result < 0)
    {
        std::cout << "recvfrom failed: " << strerror(errno) << std::endl;
        close(csock);
        return result;
    }

    close(csock);
    std::cout << "received: " << msgBuffer << " | key: " << key << std::endl;
    if(msgBuffer == key)
    {
        ip = inet_ntoa(senderInfo.sin_addr);
        return result;
    }
    else
    {
        return -1;
    }
}