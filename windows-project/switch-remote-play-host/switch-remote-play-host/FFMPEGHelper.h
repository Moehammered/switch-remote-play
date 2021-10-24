#pragma once

#include <string>
#include <windows.h>
#include <processthreadsapi.h>
#include "srp/network/NetworkData.h"
#include "DisplayDeviceService.h"

void SetParentDirectory(std::string path);

std::string CreateVideoCommandLineArg(DisplayDeviceInfo const display, EncoderConfig const config, std::string const ip, uint16_t port);

std::string CreateAudioCommandLineArg(int sampleRate, int packetSize, std::string const ip, uint16_t port);

PROCESS_INFORMATION StartStream(DisplayDeviceInfo const display, EncoderConfig const config, std::string const ip, uint16_t port, bool showEncoderOutput, bool& started);
PROCESS_INFORMATION StartAudio(std::string const ip, uint16_t port, bool showAudioEncoderWindow, bool& started);

std::string ConfigToString(EncoderConfig const config);
