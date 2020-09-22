#include "SwitchStream.h"
#include <windows.h>
#include <processthreadsapi.h>
#include <sstream>

FFMPEG_Config receivedConfig = DEFAULT_CONFIGS[0];

void ReadCommandsFromSwitch(SOCKET& switchSocket)
{
    using namespace std;

    cout << "Expected size of command payload: " << COMMAND_PAYLOAD_SIZE << " bytes" << endl;
    char* readBuffer = new char[COMMAND_PAYLOAD_SIZE];
    CommandPayload* data = (CommandPayload*)readBuffer;
    int retryCount = 3;

    PROCESS_INFORMATION streamProcessInfo;
    ZeroMemory(&streamProcessInfo, sizeof(streamProcessInfo));
    bool started = false;

    do
    {
        for(int i = 0; i < COMMAND_PAYLOAD_SIZE; ++i)
            readBuffer[i] = 0;

        auto result = recv(switchSocket, readBuffer, COMMAND_PAYLOAD_SIZE, 0);
        if(result == SOCKET_ERROR)
        {
            cout << "Failed to receive data" << endl;
            --retryCount;
            data->commandCode = retryCount < 0 ? Command::SHUTDOWN : Command::IGNORE_COMMAND;
        }
        switch(data->commandCode)
        {
            case Command::STOP_STREAM:
                cout << "Close stream" << endl;
                TerminateProcess(streamProcessInfo.hProcess, 1);
                CloseHandle(streamProcessInfo.hProcess);
                CloseHandle(streamProcessInfo.hThread);
                break;

            case Command::UPDATE_FFMPEG_CONFIG:
                cout << "Receive ffmpeg config" << endl;
                receivedConfig = data->configData;
                cout << "Received config:" << endl << endl << ConfigToString(receivedConfig) << endl;
                break;

            case Command::CLOSE_SERVER:
                cout << "Close server application" << endl;
                TerminateProcess(streamProcessInfo.hProcess, 1);
                CloseHandle(streamProcessInfo.hProcess);
                CloseHandle(streamProcessInfo.hThread);
                break;

            case Command::SHUTDOWN_PC:
                cout << "Shutdown host PC (Not implemented)" << endl;
                TerminateProcess(streamProcessInfo.hProcess, 1);
                CloseHandle(streamProcessInfo.hProcess);
                CloseHandle(streamProcessInfo.hThread);
                break;

            case Command::START_STREAM:
                TerminateProcess(streamProcessInfo.hProcess, 1);
                CloseHandle(streamProcessInfo.hProcess);
                CloseHandle(streamProcessInfo.hThread);
                cout << "Start stream with last received config from switch..." << endl;
                cout << "FFMPEG Configuration: " << endl << ConfigToString(receivedConfig) << endl;
                streamProcessInfo = StartStream(receivedConfig, started);
                break;

            case Command::START_STREAM_LOW_LATENCY_30FPS:
                TerminateProcess(streamProcessInfo.hProcess, 1);
                CloseHandle(streamProcessInfo.hProcess);
                CloseHandle(streamProcessInfo.hThread);
                cout << "Start stream [low latency 30fps]" << endl;
                cout << "FFMPEG Configuration: " << endl << ConfigToString(DEFAULT_CONFIGS[STREAM_MODE::LOW_LATENCY_30_FPS]) << endl;
                streamProcessInfo = StartStream(DEFAULT_CONFIGS[STREAM_MODE::LOW_LATENCY_30_FPS], started);
                break;

            case Command::START_STREAM_LOW_LATENCY_VFPS:
                TerminateProcess(streamProcessInfo.hProcess, 1);
                CloseHandle(streamProcessInfo.hProcess);
                CloseHandle(streamProcessInfo.hThread);
                cout << "Start stream [low latency variable fps]" << endl;
                cout << "FFMPEG Configuration: " << endl << ConfigToString(DEFAULT_CONFIGS[STREAM_MODE::LOW_LATENCY_V_FPS]) << endl;
                streamProcessInfo = StartStream(DEFAULT_CONFIGS[STREAM_MODE::LOW_LATENCY_V_FPS], started);
                break;

            case Command::START_STREAM_OK_LATENCY_60FPS:
                TerminateProcess(streamProcessInfo.hProcess, 1);
                CloseHandle(streamProcessInfo.hProcess);
                CloseHandle(streamProcessInfo.hThread);
                cout << "Start stream [OK latency 60 fps]" << endl;
                cout << "FFMPEG Configuration: " << endl << ConfigToString(DEFAULT_CONFIGS[STREAM_MODE::OK_LATENCY_60_FPS]) << endl;
                streamProcessInfo = StartStream(DEFAULT_CONFIGS[STREAM_MODE::OK_LATENCY_60_FPS], started);
                break;
        }
    } while(data->commandCode != Command::SHUTDOWN);

    TerminateProcess(streamProcessInfo.hProcess, 1);
    CloseHandle(streamProcessInfo.hProcess);
    CloseHandle(streamProcessInfo.hThread);
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

    if(!CreateProcess(NULL, (LPSTR)args.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
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

std::string ConfigToString(FFMPEG_Config config)
{
    using namespace std;

    auto vsyncText = "";
    switch(config.vsyncMode)
    {
        case 0:
            vsyncText = "passthrough"; //each frame is passed to the muxer
            break;
        case 1:
            vsyncText = "constant frame rate"; //constant fps
            break;
        case 2:
            vsyncText = "variable frame rate"; //variable fps (prevent duplicate frames)
            break;
        case 3:
            vsyncText = "drop duplicate frames"; //same as passthrough, but removes timestamps
            break;
        case -1:
            vsyncText = "auto"; //automatically choose between 1 or 2
            break;
        default:
            vsyncText = "UNKNOWN";
            break;
    }

    stringstream args;
    args << "Vsync Mode: " << (int)config.vsyncMode << " (" << vsyncText << ")" << endl;
    args << "Target Framerate: " << config.desiredFrameRate << " fps" << endl;
    args << "Video Capture Size(x,y): " << config.videoX << ", " << config.videoY << endl;
    args << "Stream Scale Size(x,y): " << config.scaleX << ", " << config.scaleY << endl;
    args << "Target Stream Bitrate: " << config.bitrateKB << " kb/s" << endl;

    return args.str();
}