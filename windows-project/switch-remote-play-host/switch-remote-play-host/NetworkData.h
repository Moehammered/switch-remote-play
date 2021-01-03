#pragma once

#include <windows.h>
#include <stdint.h>

enum ControllerMode : int16_t
{
   X360 = 0,
   DS4,
   CONTROLLER_MODE_COUNT
};

#include "EncoderPreset.h"
#include "HWAccel.h"
#include "VideoCodecMode.h"
#include "VsyncMode.h"

struct alignas(8) FFMPEG_Config
{
    int16_t         desiredFrameRate;
    int16_t         videoX;
    int16_t         videoY;
    int16_t         scaleX;
    int16_t         scaleY;
    uint16_t        bitrateKB;
    VsyncMode       vsyncMode;
    int16_t         constantRateFactor;
    EncoderPreset   preset;
    HWAccelMode     hwaccelMode;
    VideoCodecMode  videoCodecMode;
    int16_t         mouseSensitivity;
};

constexpr int FFMPEG_CONFIG_SIZE = sizeof(FFMPEG_Config);

struct alignas(2) Controller_Config
{
    ControllerMode   controllerMode;
};

constexpr int CONTROLLER_CONFIG_SIZE = sizeof(Controller_Config);

enum Command : int16_t
{
    SHUTDOWN = -1,
    START_STREAM = 0,
    STOP_STREAM,
    CLOSE_SERVER,
    SHUTDOWN_PC,
    IGNORE_COMMAND
};

constexpr int COMMAND_CODE_SIZE = sizeof(Command);

struct alignas(32) CommandPayload
{
    //for now only add ffmpeg-config as an extra data member
    FFMPEG_Config      configData;
    Controller_Config  controllerData;
    Command            commandCode;
    //fill the struct to pad it out to 32 bytes
    int8_t             padding[32 - FFMPEG_CONFIG_SIZE - COMMAND_CODE_SIZE - CONTROLLER_CONFIG_SIZE];

    // int16_t dataBufferSize;
    // char dataBuffer[255];
};

constexpr int COMMAND_PAYLOAD_SIZE = sizeof(CommandPayload);