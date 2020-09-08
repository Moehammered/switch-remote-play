/**
  * Test console application to demonstrate connecting to a server automatically on the network without inputting IP manually.
  * To test this, run the client application first, then run the server application and the connection will occur.
*/

#include <iostream>
#include <string>
#include <winsock2.h>

//connect to the inputted IP under port 20001
void ConnectDirectly(std::string ip)
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
bool WaitForBroadcast(std::string& ipFound)
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

    string serverIP = "";
    if(WaitForBroadcast(serverIP))
        ConnectDirectly(serverIP);
    //ConnectDirectly("192.168.0.19");

    WSACleanup();
 
    return 0;
}