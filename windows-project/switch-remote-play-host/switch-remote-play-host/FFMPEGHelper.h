#pragma once

#include "DisplayDeviceService.h"
#include "Log.h"
#include "srp/network/NetworkData.h"
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <processthreadsapi.h>

void SetParentDirectory(std::string path);

std::string CreateVideoCommandLineArg(DisplayDeviceInfo const display, EncoderConfig const config, std::string const ip, uint16_t port);

std::string CreateAudioCommandLineArg(int sampleRate, int packetSize, std::string const ip, uint16_t port);

PROCESS_INFORMATION StartStream(DisplayDeviceInfo const display, EncoderConfig const config, std::string const ip, uint16_t port, bool showEncoderOutput, Log& logger, bool& started);
PROCESS_INFORMATION StartAudio(std::string const ip, uint16_t port, bool showAudioEncoderWindow, Log& logger, bool& started);

std::string ConfigToString(EncoderConfig const config);
