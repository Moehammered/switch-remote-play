#pragma once

#include "NetworkData.h"
#include <string>
#include <windows.h>
#include <processthreadsapi.h>

std::string CreateVideoCommandLineArg(FFMPEG_Config const config, uint16_t port);

std::string CreateAudioCommandLineArg(int sampleRate, int packetSize, uint16_t port);

PROCESS_INFORMATION StartStream(FFMPEG_Config const config, uint16_t port, bool& started);
PROCESS_INFORMATION StartAudio(uint16_t port, bool& started);

std::string ConfigToString(FFMPEG_Config const config);
