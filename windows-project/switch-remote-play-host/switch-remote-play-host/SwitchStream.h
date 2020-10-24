#pragma once

#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
#include <atomic>
#include "NetworkData.h"

enum STREAM_MODE
{
    LOW_LATENCY_30_FPS = 0,
    OK_LATENCY_60_FPS,
    LOW_LATENCY_V_FPS,
    STREAM_MODE_COUNT
};

extern FFMPEG_Config receivedConfig;

constexpr FFMPEG_Config DEFAULT_CONFIGS[] = {
    {60, 1920, 1080, 1280, 720, 5000, 2}, //good performance, gets clamped to 30fps (low latency)
    {60, 1920, 1080, 1280, 720, 5000, 1}, //good performance, tries to stay at 60fps (OK latency)
    {60, 1920, 1080, 1280, 720, 5000, 1} //can be very good, almost similar to mode 1 (low latency)
};

void ReadCommandsFromSwitch(SOCKET const & switchSocket);

std::thread StartGamepadListener();