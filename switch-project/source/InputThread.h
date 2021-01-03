#ifndef __INPUTTHREAD_H__
#define __INPUTTHREAD_H__

#include <atomic>
#include <string>
#include "network/NetworkData.h"
#include <switch.h>

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

enum StreamState : int32_t
{
    INACTIVE,
    REQUESTED,
    ACTIVE,
    QUIT
};

void RunStartConfiguredStreamCommand(std::string ip, uint16_t port, EncoderConfig const config, ControllerConfig const controllerConfig);

void RunGamepadThread(std::string ip, uint16_t port);

#endif
