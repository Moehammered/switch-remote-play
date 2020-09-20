#pragma once

#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
#include <atomic>

struct FFMPEG_Config
{
    int16_t     desiredFrameRate;
    int8_t      vsyncMode;
    int16_t     videoX, videoY;
    int16_t     scaleX, scaleY;
    uint16_t    bitrateKB;
};

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

void ListenForStreamStart();

void ReadCommandsFromSwitch(SOCKET& switchSocket);

bool ListenForConnection(const SOCKET listenSock, SOCKET& connectedSocket, sockaddr_in& connectionInfo);

bool CreateListenerSocket(SOCKET& sock, uint16_t portNo);

PROCESS_INFORMATION StartStream(FFMPEG_Config config, bool& started);

std::string CreateCommandLineArg(FFMPEG_Config config);