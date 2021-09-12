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
#include "../mouse/MouseOptions.h"
#include "../touch/TouchOptions.h"

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
    //fill the struct to pad it out to 72 bytes
    int8_t                          padding[PayloadPaddingSize];
};

constexpr int COMMAND_PAYLOAD_SIZE = sizeof(CommandPayload);

struct TouchState
{
    uint32_t fingerID;
    uint32_t x;
    uint32_t y;
};

auto constexpr TouchStateSize = sizeof(TouchState);
auto constexpr MaxTouchCount = 5;

struct alignas(8) TouchPayload
{
    int32_t count;
    TouchState touches[MaxTouchCount];
};

auto constexpr TouchPayloadSize = sizeof(TouchPayload);

struct alignas(8) GamepadDataPayload
{
    uint32_t keys;
    int32_t ljx, ljy;
    int32_t rjx, rjy;
    HidVector gryo;
    HidVector accelerometer;
    char padding[64 - sizeof(uint32_t) - sizeof(int32_t) * 4 - sizeof(HidVector) * 2];
};

auto constexpr GamepadDataPayloadSize = sizeof(GamepadDataPayload);

struct InputDataPayload
{
    GamepadDataPayload  gamepad;
    TouchPayload        touchScreen;
};

auto constexpr InputDataPayloadSize = sizeof(InputDataPayload);

#endif
