#pragma once

#include <windows.h>
#include <stdint.h>

#include "GenericOptions.h"
#include "H264Options.h"
#include "h264amfOptions.h"
#include "HWAccel.h"
#include "VideoCodecMode.h"
#include "VsyncMode.h"
#include "ControllerOptions.h"
#include "MouseOptions.h"
#include "TouchOptions.h"

struct alignas(8) EncoderConfig
{
    VideoData       commonSettings;
    union
    {
        h264::H264Data cpuSettings;
        h264amf::H264AMFData amdSettings;
    };
};

constexpr int ENCODER_CONFIG_SIZE = sizeof(EncoderConfig);

enum class Command : int16_t
{
    SHUTDOWN = -1,
    START_STREAM = 0,
    STOP_STREAM,
    CLOSE_SERVER,
    SHUTDOWN_PC,
    IGNORE_COMMAND
};

constexpr int COMMAND_CODE_SIZE = sizeof(Command);

auto constexpr PayloadPaddingSize = 96 - ENCODER_CONFIG_SIZE
- COMMAND_CODE_SIZE - controller::ControllerConfigSize
- mouse::MouseConfigSize - touch::TouchConfigSize;
struct alignas(8) CommandPayload
{
    //for now only add ffmpeg-config as an extra data member
    EncoderConfig                   encoderData;
    controller::ControllerConfig    controllerData;
    mouse::MouseConfig              mouseData;
    touch::TouchConfig              touchData;
    Command                         commandCode;
    //fill the struct to pad it out to 96 bytes
    int8_t                          padding[PayloadPaddingSize];
};

constexpr int COMMAND_PAYLOAD_SIZE = sizeof(CommandPayload);
