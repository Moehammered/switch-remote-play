#pragma once

#include "NetworkData.h"
#include <string>
//#include <windows.h>
//#include <processthreadsapi.h>
#include "Process.h"

void SetParentDirectory(std::string path);
void SetScreenIndex(std::string index);

std::string CreateVideoCommandLineArg(EncoderConfig const config, std::string const ip, uint16_t port);

std::string CreateAudioCommandLineArg(int sampleRate, int packetSize, std::string const ip, uint16_t port);

Process StartStream(EncoderConfig const config, std::string const ip, uint16_t port, bool& started);
//PROCESS_INFORMATION StartStream(EncoderConfig const config, std::string const ip, uint16_t port, bool& started);
//PROCESS_INFORMATION StartAudio(std::string const ip, uint16_t port, bool& started);

std::string ConfigToString(EncoderConfig const config);
