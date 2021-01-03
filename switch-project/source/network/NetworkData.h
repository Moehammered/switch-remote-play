#ifndef __NETWORKDATA_H__
#define __NETWORKDATA_H__

#include <stdint.h>
#include "../dataHelpers/EncoderPreset.h"
#include "../dataHelpers/HWAccel.h"
#include "../dataHelpers/VideoCodecMode.h"
#include "../dataHelpers/VsyncMode.h"
#include "../dataHelpers/ControllerMode.h"
#include "../dataHelpers/ControllerButtonMap.h"

struct alignas(16) AudioConfig
{
    uint32_t    sampleRate;
    uint32_t    framerate;
    uint32_t    channelCount;
    uint32_t    bitrate;
};

int constexpr AUDIO_CONFIG_SIZE = {sizeof(AudioConfig)};
struct alignas(8) EncoderConfig
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
    int8_t          padding[2];
};

constexpr int ENCODER_CONFIG_SIZE = sizeof(EncoderConfig);

struct alignas(2) ControllerConfig
{
    ControllerMode      controllerMode;
    ControllerButtonMap controllerMap;
    int16_t             mouseSensitivity;
    bool                mouseOnConnect;
};

constexpr int CONTROLLER_CONFIG_SIZE = sizeof(ControllerConfig);

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
    EncoderConfig       encoderData;
    ControllerConfig    controllerData;
    Command             commandCode;
    //fill the struct to pad it out to 32 bytes
    int8_t              padding[64 - ENCODER_CONFIG_SIZE - COMMAND_CODE_SIZE - CONTROLLER_CONFIG_SIZE];

    // int16_t dataBufferSize;
    // char dataBuffer[255];
};

constexpr int COMMAND_PAYLOAD_SIZE = sizeof(CommandPayload);

#endif
