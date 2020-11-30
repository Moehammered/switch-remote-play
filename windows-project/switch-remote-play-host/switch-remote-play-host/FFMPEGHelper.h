#pragma once

#include "NetworkData.h"
#include <string>
#include <windows.h>
#include <processthreadsapi.h>

std::string CreateCommandLineArg(FFMPEG_Config const config);

std::string CreateAudioCommandLineArg();

PROCESS_INFORMATION StartStream(FFMPEG_Config const config, bool& started);
PROCESS_INFORMATION StartAudio(bool& started);

std::string ConfigToString(FFMPEG_Config const config);
