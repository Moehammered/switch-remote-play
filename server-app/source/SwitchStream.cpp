#include "SwitchStream.h"
#include <windows.h>
#include <processthreadsapi.h>
#include <sstream>

void ReadCommandsFromSwitch(SOCKET& switchSocket)
{
    using namespace std;

    int dataSize = sizeof(CommandPayload);
    cout << "Expected size of command payload: " << dataSize << " bytes" << endl;
    char* readBuffer = new char[dataSize];
    CommandPayload* data = (CommandPayload*)readBuffer;
    int retryCount = 3;

    do
    {
        for(int i = 0; i < dataSize; ++i)
            readBuffer[i] = 0;

        auto result = recv(switchSocket, readBuffer, dataSize, 0);
        if(result == SOCKET_ERROR)
        {
            cout << "Failed to receive data" << endl;
            --retryCount;
            data->commandCode = retryCount < 0 ? CommandPayload::Command::SHUTDOWN : CommandPayload::Command::IGNORE_COMMAND;
        }
        switch(data->commandCode)
        {
            case CommandPayload::Command::CLOSE_STREAM:
                cout << "Close stream (Not implemented)" << endl;
                break;

            case CommandPayload::Command::UPDATE_FFMPEG_CONFIG:
                cout << "Receive ffmpeg config (Not implemented)" << endl;
                break;

            case CommandPayload::Command::CLOSE_SERVER:
                cout << "Close server application (Not implemented)" << endl;
                break;

            case CommandPayload::Command::SHUTDOWN_PC:
                cout << "Shutdown host PC (Not implemented)" << endl;
                break;

            case CommandPayload::Command::START_STREAM:
                cout << "Start stream (Not implemented)" << endl;
                break;
        }
    } while(data->commandCode != CommandPayload::Command::SHUTDOWN);
}

bool ListenForConnection(const SOCKET listenSock, SOCKET& connectedSocket, sockaddr_in& connectionInfo)
{
    using namespace std;
    cout << "waiting for connection to server..." << endl << endl;

    sockaddr_in clientInfo;
    int infoSize = sizeof(clientInfo);

    ZeroMemory(&clientInfo, infoSize);

    auto connectionSocket = accept(listenSock, (sockaddr*)&clientInfo, &infoSize);
    if(connectionSocket == INVALID_SOCKET)
    {
        cout << "Failed to accept connection: " << WSAGetLastError() << endl;
        return false;
    }

    connectedSocket = connectionSocket;
    connectionInfo = clientInfo;

    return true;
}

bool CreateListenerSocket(SOCKET& sock, uint16_t portNo)
{
    using namespace std;

    sock = INVALID_SOCKET;

    sockaddr_in serverAddr, clientAddr;

    ZeroMemory(&serverAddr, sizeof(serverAddr));
    ZeroMemory(&clientAddr, sizeof(clientAddr));

    int result = 0;

    auto serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(serverSocket == INVALID_SOCKET)
    {
        cout << "Failed to create socket for server: " << WSAGetLastError() << endl;
        return false;
    }
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    serverAddr.sin_port = htons(portNo);

    result = bind(serverSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
    if(result == SOCKET_ERROR)
    {
        cout << "Failed to bind socket: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        return false;
    }
    
    result = listen(serverSocket, 1);
    if(result == SOCKET_ERROR)
    {
        cout << "Failed to set the socket into listening mode: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        return false;
    }

    sock = serverSocket;
    return true;
}

// Will listen for the switch to send it a signal to begin the stream
void ListenForStreamStart() //to be written, first test starting and stopping the stream
{
    using namespace std;

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

// Generate the command line argument string to execute ffmpeg
std::string CreateCommandLineArg(FFMPEG_Config config)
{
    using namespace std;

    auto vsyncMode = "1";
    switch(config.vsyncMode)
    {
        case 0:
            vsyncMode = "passthrough"; //each frame is passed to the muxer
            break;
        case 1:
            vsyncMode = "cfr"; //constant fps
            break;
        case 2:
            vsyncMode = "vfr"; //variable fps (prevent duplicate frames)
            break;
        case 3:
            vsyncMode = "drop"; //same as passthrough, but removes timestamps
            break;
        case -1:
            vsyncMode = "auto"; //automatically choose between 1 or 2
            break;
        default:
            vsyncMode = "cfr";
            break;
    }
    stringstream args;
    args << "ffmpeg.exe -probesize 32 -hwaccel auto -y -f gdigrab ";
    args << "-framerate " << config.desiredFrameRate << " ";
    args << "-vsync " << vsyncMode << " ";
    args << "-video_size " << config.videoX << "x" << config.videoY << " ";
    args << "-i desktop -f h264 -vf ";
    args << "\"scale=" << config.scaleX << "x" << config.scaleY << "\" ";
    args << "-preset ultrafast -tune zerolatency -pix_fmt yuv420p -profile:v baseline ";
    args << "-x264-params \"nal-hrd=cbr\" ";
    args << "-b:v " << config.bitrateKB << "k -minrate " << config.bitrateKB << "k -maxrate " << config.bitrateKB << "k ";
    args << "-bufsize " << config.bitrateKB << "k tcp://192.168.0.17:2222";

    return args.str();
}

// Create a windows process to start the ffmpeg.exe application via CMD in a new window
PROCESS_INFORMATION StartStream(FFMPEG_Config config, bool& started)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);

    auto args = CreateCommandLineArg(config);

    if(!CreateProcess(NULL, (LPSTR)args.c_str(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {
        //error occured
        std::cout << "Failed to start process. CreateProcess error code: " << GetLastError() << std::endl;
        std::cout << "Argument line passed to cmd: " << std::endl << std::endl << args << std::endl;
        started = false;
    }
    else
        started = true;

    return pi;
}