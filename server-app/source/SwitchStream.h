#pragma once

#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>

struct StreamConfig
{
    int16_t    desiredFrameRate;
    int8_t     vsyncMode;
    int16_t    videoX, videoY;
    int16_t    scaleX, scaleY;
    uint16_t    bitrateKB;
};

void ListenForStreamStart();

PROCESS_INFORMATION StartStream(StreamConfig config, bool& started);

std::string CreateCommandLineArg(StreamConfig config);