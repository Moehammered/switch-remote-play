#include <iostream>
#include <string>
#include <winsock2.h>

int main(int argc, char* argv[])
{
    using namespace std;
    cout << "hello server world" << endl;

    WSADATA wsaStateData;
    WSAStartup(MAKEWORD(2,2), &wsaStateData);

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

    WSACleanup();

    return 0;
}