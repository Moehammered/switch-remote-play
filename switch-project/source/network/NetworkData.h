#ifndef __NETWORKDATA_H__
#define __NETWORKDATA_H__

#include <stdint.h>
#include "../codec/general/GenericOptions.h"
#include "../codec/h264/H264Options.h"
#include "../codec/h264_amf/h264amfOptions.h"
#include "../dataHelpers/Resolution.h"
#include "../dataHelpers/HWAccel.h"
#include "../dataHelpers/VideoCodec.h"
#include "../dataHelpers/VsyncMode.h"
#include "../controller/ControllerOptions.h"

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
    VideoData       commonSettings;
    union
    {
        h264::H264Data cpuSettings;
        h264amf::H264AMFData amdSettings;
    };
};

constexpr int ENCODER_CONFIG_SIZE = sizeof(EncoderConfig);

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

constexpr int CONTROLLER_CONFIG_SIZE = sizeof(controller::ControllerConfig);

auto constexpr PayloadPaddingSize = 72 - ENCODER_CONFIG_SIZE - COMMAND_CODE_SIZE - CONTROLLER_CONFIG_SIZE;
struct alignas(8) CommandPayload
{
    //for now only add ffmpeg-config as an extra data member
    EncoderConfig                   encoderData;
    controller::ControllerConfig    controllerData;
    Command                         commandCode;
    //fill the struct to pad it out to 72 bytes
    int8_t                          padding[PayloadPaddingSize];
};

constexpr int COMMAND_PAYLOAD_SIZE = sizeof(CommandPayload);

#endif
