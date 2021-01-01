#pragma once

#include "NetworkData.h"
#include <string>
#include <windows.h>
#include <processthreadsapi.h>

void SetParentDirectory(std::string path);

std::string CreateVideoCommandLineArg(FFMPEG_Config const config, std::string const ip, uint16_t port);

std::string CreateAudioCommandLineArg(int sampleRate, int packetSize, std::string const ip, uint16_t port);

PROCESS_INFORMATION StartStream(FFMPEG_Config const config, std::string const ip, uint16_t port, bool& started);
PROCESS_INFORMATION StartAudio(std::string const ip, uint16_t port, bool& started);

std::string ConfigToString(FFMPEG_Config const config);
