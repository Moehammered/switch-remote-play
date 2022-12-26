#include "CommandSender.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <string.h>
#include <assert.h>

bool ConnectTo(std::string ip, uint16_t port, int& connectionSock)
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    auto clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(clientSocket < 0)
    {
        std::cout << "error occurred trying to create socket: " << strerror(errno) << std::endl;
        return false;
    }

    auto result = connect(clientSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
    if(result < 0)
    {
        std::cout << "failed to connect to server: " << strerror(errno) << "(" << errno << ")" << std::endl;
        close(clientSocket);
        return false;
    }

    connectionSock = clientSocket;
    return true;
}

bool SendCommandPayload(int const connectionSock, CommandPayload const payload)
{
    static_assert(commandCodeSize == 2);
    static_assert(encoderConfigSize == 40);
    static_assert(controller::controllerConfigSize == 32);
    static_assert(commandPayloadSize == 464);
    static_assert(PayloadPropertySizeSum == 458);
    static_assert(commandCodeSize == 2);
    static_assert(mouse::mouseConfigSize == 56);
    static_assert(touch::touchConfigSize == 20);
    static_assert(keyboard::keyboardConfigSize == 288);
    static_assert(audio::audioConfigSize == 20);

    char* dataPtr = (char*)&payload;

    // std::cout << "Sending command payload with configuration of size: " << COMMAND_PAYLOAD_SIZE << " bytes" << std::endl;

    auto result = send(connectionSock, dataPtr, commandPayloadSize, 0);
    if(result < 0)
    {
        std::cout << "Failed to send data code: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}