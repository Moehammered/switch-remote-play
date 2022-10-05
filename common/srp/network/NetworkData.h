#ifndef __NETWORKDATA_H__
#define __NETWORKDATA_H__

#include "../codec/general/GenericOptions.h"
#include "../codec/h264/H264Options.h"
#include "../codec/h264_amf/h264amfOptions.h"
#include "../ffmpegHelpers/Resolution.h"
#include "../ffmpegHelpers/HWAccel.h"
#include "../ffmpegHelpers/VideoCodec.h"
#include "../ffmpegHelpers/VsyncMode.h"
#include "../controller/ControllerOptions.h"
#include "../mouse/MouseOptions.h"
#include "../keyboard/KeyboardOptions.h"
#include "../touch/TouchOptions.h"
#include "../audio/AudioOptions.h"
#include "../controller/SwitchHidDefinition.h"
#include <stdint.h>

struct alignas(8) EncoderConfig
{
    codec::VideoData       commonSettings;
    union
    {
        h264::H264Data cpuSettings;
        h264amf::H264AmfData amdSettings;
    };
};

constexpr int encoderConfigSize = sizeof(EncoderConfig);

enum Command : int16_t
{
    SHUTDOWN = -1,
    START_STREAM = 0,
    STOP_STREAM,
    CLOSE_SERVER,
    SHUTDOWN_PC,
    IGNORE_COMMAND
};

auto constexpr commandCodeSize = sizeof(Command);

auto constexpr PayloadPropertySizeSum = encoderConfigSize 
    + commandCodeSize + controller::controllerConfigSize
    + mouse::mouseConfigSize + touch::touchConfigSize 
    + keyboard::keyboardConfigSize + audio::audioConfigSize;

size_t constexpr determinePaddingSize(int const alignment, int const size)
{
    auto nextSize = 0;
    do
    {
        nextSize += alignment;
    } while (nextSize < size);
    
    return nextSize - size;
}

auto constexpr PayloadPaddingSize = determinePaddingSize(8, PayloadPropertySizeSum);

struct alignas(8) CommandPayload
{
    //for now only add ffmpeg-config as an extra data member
    EncoderConfig                   encoderData;
    controller::ControllerConfig    controllerData;
    mouse::MouseConfig              mouseData;
    keyboard::KeyboardConfig        keyboardData;
    touch::TouchConfig              touchData;
    audio::AudioConfig              audioData;
    Command                         commandCode;
    //fill the struct to pad it out to next alignment of bytes
    int8_t                          padding[PayloadPaddingSize];
};

auto constexpr commandPayloadSize = sizeof(CommandPayload);

void printCommandPayload(CommandPayload const& payload);

struct TouchState
{
    uint32_t fingerID;
    uint32_t x;
    uint32_t y;
};

auto constexpr touchStateSize = sizeof(TouchState);
auto constexpr maxTouchCount = 5;

struct alignas(8) TouchPayload
{
    int32_t count;
    TouchState touches[maxTouchCount];
};

auto constexpr touchPayloadSize = sizeof(TouchPayload);

struct alignas(8) GamepadDataPayload
{
    uint64_t keys;
    int32_t ljx, ljy;
    int32_t rjx, rjy;
    HidVector gryo;
    HidVector accelerometer;
    char padding[64 - sizeof(uint32_t) - sizeof(int32_t) * 4 - sizeof(HidVector) * 2];
};

auto constexpr gamepadDataPayloadSize = sizeof(GamepadDataPayload);

auto constexpr maxControllerCount = 4;
auto constexpr allGamepadPayloadsSize = gamepadDataPayloadSize * maxControllerCount;

struct InputDataPayload
{
    GamepadDataPayload  gamepads[maxControllerCount];
    TouchPayload        touchScreen;
};

auto constexpr inputDataPayloadSize = sizeof(InputDataPayload);

#endif
