#ifndef __INPUTTHREAD_H__
#define __INPUTTHREAD_H__

#include <atomic>
#include <string>
#include "network/NetworkData.h"

struct GamepadDataPayload
{
    uint32_t keys;
    int32_t ljx, ljy;
    int32_t rjx, rjy;
    char padding[32 - (sizeof(uint32_t) + sizeof(int32_t) * 4)];
};

enum StreamState : int32_t
{
    INACTIVE,
    REQUESTED,
    ACTIVE,
    QUIT
};

void RunStopCommandThread(std::string ip, uint16_t port);

void RunStartConfiguredStreamCommand(std::string ip, uint16_t port, FFMPEG_Config const config);

void RunGamepadThread(std::string ip, uint16_t port);

#endif
