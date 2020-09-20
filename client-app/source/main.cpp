/**
  * Test console application to demonstrate connecting to a server automatically on the network without inputting IP manually.
  * To test this, run the client application first, then run the server application and the connection will occur.
*/

#include <iostream>
#include <string>
#include <winsock2.h>

void HandshakeSwitch(std::string ip, std::string key)
{
    sockaddr_in serverAddr;
    unsigned short portNo = 20001;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNo);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    auto clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(clientSocket == INVALID_SOCKET)
        std::cout << "error occurred trying to create socket" << std::endl;

    auto result = connect(clientSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
    if(result == SOCKET_ERROR)
        std::cout << "failed to connect to server" << std::endl;

    // char msgBuffer[256];
    // for(auto& c : msgBuffer)
    //     c = 0;
    
    // result = recv(clientSocket, msgBuffer, 255, 0);
    // if(result == SOCKET_ERROR)
    //     std::cout << "error receiving data from server" << std::endl;
    // else
    // {
    //     std::cout << "received " << result << " bytes. (msg: " << msgBuffer << ")" << std::endl;
    // }
    // std::string reply = "Thanks mate. I'm on the switch so no BSOD here, just atmos crash dumps.";
    result = send(clientSocket, key.c_str(), key.length(), 0);
    if(result == SOCKET_ERROR)
        std::cout << "failed to send reply" << std::endl;
    else
    {
        std::cout << "Sent " << key.length() << " bytes." << std::endl;
    }
    
    closesocket(clientSocket);
}


struct CommandPayload
{
    enum Command : int16_t
    {
        SHUTDOWN = -10, CLOSE_STREAM = -1, UPDATE_FFMPEG_CONFIG = 0, CLOSE_SERVER, SHUTDOWN_PC, START_STREAM, IGNORE_COMMAND
    } commandCode;
    //for now maximum of 255 bytes, replace with union definition once command payloads are better defined
    int16_t dataBufferSize;
    char dataBuffer[255];
};

struct FFMPEG_Config
{
    int16_t     desiredFrameRate;
    int8_t      vsyncMode;
    int16_t     videoX, videoY;
    int16_t     scaleX, scaleY;
    uint16_t    bitrateKB;
};

bool SendCode(const SOCKET connectionSock, int16_t data)
{
    CommandPayload payload;
    payload.commandCode = data >= CommandPayload::Command::IGNORE_COMMAND ? 
                                    CommandPayload::Command::IGNORE_COMMAND 
                                 : (CommandPayload::Command)data;
    //clear the data buffer incase of unintialised memory
    for(auto& c : payload.dataBuffer)
        c = 0;

    //for now always send config data to test
    FFMPEG_Config config;
    config.desiredFrameRate = 3 * data;
    config.vsyncMode = data;
    config.videoX = 4 * data; config.videoY = 2 * data;
    config.scaleX = 2 * data; config.scaleY = data;
    config.bitrateKB = data < 0 ? data * -1 * 1000 : data * 1000;
    //now copy the config into the data buffer
    int dataBufferSize = sizeof(FFMPEG_Config);
    char* configData = (char*)&config;
    for(int i = 0; i < dataBufferSize; ++i)
        payload.dataBuffer[i] = configData[i];
    
    payload.dataBufferSize = dataBufferSize;

    char* dataPtr = (char*)&payload;
    int payloadSize = sizeof(payload);

    std::cout << "Sending payload of size: " << payloadSize << " bytes" << std::endl;

    auto result = send(connectionSock, dataPtr, payloadSize, 0);
    if(result == SOCKET_ERROR)
    {
        std::cout << "Failed to send data code" << std::endl;
        return false;
    }

    return true;
}

bool ConnectTo(std::string ip, uint16_t port, SOCKET& connectionSock)
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    auto clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(clientSocket == INVALID_SOCKET)
    {
        std::cout << "error occurred trying to create socket" << std::endl;
        return false;
    }

    auto result = connect(clientSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
    if(result == SOCKET_ERROR)
    {
        std::cout << "failed to connect to server" << std::endl;
        closesocket(clientSocket);
        return false;
    }

    connectionSock = clientSocket;
    return true;
}

//connect to the inputted IP under port 20001
void ConnectDirectly(std::string ip, uint16_t portNo)
{
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNo);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    auto clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(clientSocket == INVALID_SOCKET)
        std::cout << "error occurred trying to create socket" << std::endl;

    auto result = connect(clientSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
    if(result == SOCKET_ERROR)
        std::cout << "failed to connect to server" << std::endl;

    char msgBuffer[256];
    for(auto& c : msgBuffer)
        c = 0;
    
    result = recv(clientSocket, msgBuffer, 255, 0);
    if(result == SOCKET_ERROR)
        std::cout << "error receiving data from server" << std::endl;
    else
    {
        std::cout << "received " << result << " bytes. (msg: " << msgBuffer << ")" << std::endl;
    }
    std::string reply = "Thanks mate. I'm on the switch so no BSOD here, just atmos crash dumps.";
    result = send(clientSocket, reply.c_str(), reply.length(), 0);
    if(result == SOCKET_ERROR)
        std::cout << "failed to send reply" << std::endl;
    else
    {
        std::cout << "Sent " << reply.length() << " bytes." << std::endl;
    }
    
    closesocket(clientSocket);
}

//get the client to wait for a broadcast on the network, when finding one it will set the ipFound to the broadcaster
bool WaitForBroadcast(std::string& ipFound, std::string& key)
{
    using namespace std;

    bool success = false;

    sockaddr_in broadcastAddr;
    int sizeofAddr;
    sizeofAddr = sizeof(broadcastAddr);
    USHORT portNo = 20001;
    broadcastAddr.sin_port = htons(portNo);
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //any addr (including broadcasts)

    auto broadcastSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(portNo);
    recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    auto result = bind(broadcastSock, (sockaddr *)&recvAddr, sizeof(recvAddr));
    if(result == SOCKET_ERROR)
        cout << "failed to bind socket for broadcast receival: " << WSAGetLastError() << endl;

    char dataBuffer[256];
    for(auto& c : dataBuffer)
        c = 0;

    cout << "waiting for anything to send a message to me..." << endl;
    result = recvfrom(broadcastSock, dataBuffer, 255, 0, (sockaddr*)&broadcastAddr, &sizeofAddr);
    if(result == SOCKET_ERROR)
        cout << "error trying to receive data: " << WSAGetLastError() << endl;
    else
    {
        cout << "Data received: " << dataBuffer << endl;
        cout << "From: " << inet_ntoa(broadcastAddr.sin_addr) << endl;
        cout << "broadcast received. closing." << endl;
        ipFound = inet_ntoa(broadcastAddr.sin_addr);
        key = dataBuffer;
        success = true;
    }

    closesocket(broadcastSock);

    return success;
}

int main(int argc, char* argv[])
{
    using namespace std;
    cout << "hello server world" << endl;

    WSADATA wsaStateData;
    WSAStartup(MAKEWORD(2,2), &wsaStateData);

    // string serverIP = "";
    // string key = "";
    // if(WaitForBroadcast(serverIP, key))
    // {
    //     //ConnectDirectly(serverIP);
    //     HandshakeSwitch(serverIP, key);
    // }
    //ConnectDirectly("192.168.0.19");

    int16_t input = 0;
    string serverIP = "127.0.0.1"; //localhost
    SOCKET serverSocket;
    if(ConnectTo(serverIP, 20001, serverSocket))
    {
        do
        {
            cout << "Input data to send to server: (using numbers only for now)" << endl;
            cout << "-10 = shutdown client and server" << endl;
            cout << "-1 = close stream" << endl << "0 = update ffmpeg config" << endl << "1 = close server" << endl;
            cout << "2 = shutdown host PC" << endl << "3 = Start stream" << endl << endl;
            cout << "Command: ";

            cin >> input;

            SendCode(serverSocket, input);
            Sleep(500);
        } while(input != -1 && input != -10);
    }

    WSACleanup();
 
    return 0;
}