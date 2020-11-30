#ifndef __INPUTTHREAD_H__
#define __INPUTTHREAD_H__

#include <atomic>
#include "ui/FFMPEGConfigUI.h"
#include "network/NetworkDiscovery.h"

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

void CommandStopThreadStart(std::string ip, uint16_t port);

void RunStartConfiguredStreamCommand(std::string ip, uint16_t port, FFMPEG_Config const config);

bool ProcessInactiveStreamInput(uint32_t kDown, std::atomic_int32_t& streamState, FFMPEGConfigUI& configRender,
                                uint16_t hostPort, std::string const subnet, uint16_t broadcastPort,
                                std::reference_wrapper<NetworkDiscovery*> network);

void RunInactiveStreamInput(std::atomic_int32_t& streamState, FFMPEGConfigUI& configRender,
                            uint16_t hostPort, std::string const subnet, uint16_t broadcastPort,
                            std::reference_wrapper<NetworkDiscovery*> network);

void RunGamepadThread(std::string ip, uint16_t port);

#endif
