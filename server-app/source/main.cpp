/**
 * Test console application to demonstrate connecting to a client automatically on the network without inputting IP manually.
 * To test this, run the client application first, then run the server application and the connection will occur.
*/

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "SwitchStream.h"

using namespace std;

enum STREAM_MODE
{
    OK_30_FPS = 0,
    OK_60_FPS,
    VG_60_FPS,
    STREAM_MODE_COUNT
};

const FFMPEG_Config DEFAULT_CONFIGS[] = {
    {60, 2, 1920, 1080, 1280, 720, 5000}, //good performance, gets clamped to 30fps (low latency)
    {60, 1, 1920, 1080, 1280, 720, 5000}, //good performance, tries to stay at 60fps (OK latency)
    {60, 3, 1920, 1080, 1280, 720, 5000} //can be very good, almost similar to mode 1 (low latency)
};

//wait for something to connect to this 'server' application and exchange messages
void WaitForDirectConnection()
{
    sockaddr_in serverAddr, clientAddr;

    int result = 0;

    auto serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(serverSocket == INVALID_SOCKET)
        cout << "Failed to create socket for server: " << WSAGetLastError() << endl;

    USHORT portNo = 20001;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY; //get host IP
    serverAddr.sin_port = htons(portNo); //host to network short (byte order for machine and network is different)

    result = bind(serverSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
    if(result == SOCKET_ERROR)
        cout << "Failed to bind socket: " << WSAGetLastError() << endl;

    listen(serverSocket, 1); //start a listener for the server socket, with a maximum of 1 allowed in the queue

    auto serverIP = inet_ntoa(serverAddr.sin_addr);

    int clientAddrSize = sizeof(clientAddr);
    cout << "waiting for a client connection... (IP: " << serverIP << ": " << portNo << ")" << endl;
    auto clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);
    if(clientSocket == INVALID_SOCKET)
        cout << "Failed to accept client connection: " << WSAGetLastError() << endl;

    //if we're here, we're connected to the client. So send it a nice message.
    auto clientIP = inet_ntoa(clientAddr.sin_addr);
    auto clientPort = ntohs(clientAddr.sin_port); //network to host short (byte order for network and machine is different)

    cout << "Connected to " << clientIP << " on port " << clientPort << endl;
    cout << "sending message to client..." << endl;

    string welcomeMessage = "This is the ghost of the cmd from windooooows... wooooo (BSOD)";
    result = send(clientSocket, welcomeMessage.c_str(), welcomeMessage.length(), 0);
    if(result == SOCKET_ERROR)
        cout << "Failed to send message to client: " << WSAGetLastError() << endl;
    else
        cout << "Sent " << result << " bytes" << endl;

    char readBuffer[256];
    for(auto& c : readBuffer)
        c = 0;

    cout << "waiting for message from client..." << endl;
    //wait for the client to send us something now
    result = recv(clientSocket, readBuffer, 255, 0);
    if(result == SOCKET_ERROR)
        cout << "Failed to recieve from client: " << WSAGetLastError() << endl;
    else
        cout << "Received " << result << " bytes from client. Message: " << readBuffer << endl;

    //done here, let's wrap it up
    cout << "Closing server now. Bye bye." << endl;
    closesocket(clientSocket);
    closesocket(serverSocket);
}

//broadcast a message from this 'server' application to let clients know it's live and ready to get connections
void BroadcastServer()
{
    sockaddr_in broadcastAddr;
    USHORT portNo = 20001;
    broadcastAddr.sin_port = htons(portNo);
    broadcastAddr.sin_family = AF_INET;
    // need to find a way to automatically get the (wireless)local area network IP and subnet mask to make the broadcast address
    broadcastAddr.sin_addr.S_un.S_addr = inet_addr("192.168.0.255"); //broadcast addr of my LAN (gateway 192.168.0.1, subnet mask 255.255.255.0)
    //broadcastAddr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST); //broadcast addr for all network IPs (from local network, to ISP network)

    auto broadcastSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    bool socketOption = true;
    int sizeofOption = sizeof(socketOption);
    //must setup the socket to explicitly want to broadcast
    auto result = setsockopt(broadcastSock, SOL_SOCKET, SO_BROADCAST, (char*)&socketOption, sizeofOption);
    if(result == SOCKET_ERROR)
        cout << "Failed to set socket option to broadcast: " << WSAGetLastError() << endl;

    string bcsMsg = "testes";
    cout << "broadcasting..." << endl;
    result = sendto(broadcastSock, bcsMsg.c_str(), bcsMsg.length(), 0, (const sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

    if(result == SOCKET_ERROR)
        cout << "Failed to send on broadcast socket: " << WSAGetLastError() << endl;
    else
        cout << "broadcasted message" << endl;

    cout << "broadcast finished" << endl;

    closesocket(broadcastSock);
}

int main(int argc, char* argv[])
{
    cout << "FFMPEG stream starter" << endl;

    WSADATA wsaStateData;
    WSAStartup(MAKEWORD(2,2), &wsaStateData);

    //BroadcastServer();

    //WaitForDirectConnection();

    SOCKET serverSocket;
    SOCKET switchSocket;

    if(CreateListenerSocket(serverSocket, 20001))
    {
        cout << "Listener socket created" << endl << endl;
        sockaddr_in connectionInfo;
        if(ListenForConnection(serverSocket, switchSocket, connectionInfo))
        {
            ReadCommandsFromSwitch(switchSocket); //blocks here until a stop command or failed retry occurs
            cout << "Closing server now..." << endl << endl;
            //cleanup
            closesocket(switchSocket);
        }
        closesocket(serverSocket);
    }

    // std::string choice = "";

    // cout << "Choose stream configuration mode: " << endl;
    // cout << "0 - Low latency, clamps to 30fps, good performance" << endl;
    // cout << "1 - OK latency, maintains 60fps, good performance" << endl;
    // cout << "2 - Low Latency, maintains constant fps(30 or 60, it chooses), very good performance" << endl << endl;

    // cout << "Which mode would you like? ";
    // int configChoice = 0;
    // do
    // {
    //     cin >> configChoice;
    // } while(configChoice < OK_30_FPS || configChoice >= STREAM_MODE_COUNT);

    // bool started = false;
    // auto processorInfo = StartStream(DEFAULT_CONFIGS[configChoice], started);

    // if(started)
    // {
    //     cout << "ffmpeg started..." << endl;

    //     cout << "To kill stream, please enter 'stop'" << endl;

    //     while(choice != "stop")
    //     {
    //         cin >> choice;
    //     }

    //     TerminateProcess(processorInfo.hProcess, 1);
    //     CloseHandle(processorInfo.hProcess);
    //     CloseHandle(processorInfo.hThread);
    // }

    WSACleanup();

    return 0;
}