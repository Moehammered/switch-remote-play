#ifndef __INPUTTHREAD_H__
#define __INPUTTHREAD_H__

// #include <atomic>
#include <string>
#include "network/NetworkData.h"
// #include <switch.h>

enum StreamState : int32_t
{
    INACTIVE,
    REQUESTED,
    ACTIVE,
    QUIT
};

void RunStartConfiguredStreamCommand(std::string ip, uint16_t port, 
    EncoderConfig const config, 
    controller::ControllerConfig const controllerConfig,
    mouse::MouseConfig const mouseConfig,
    keyboard::KeyboardConfig const keyboardConfig,
    touch::TouchConfig const touchConfig);

void RunGamepadThread(std::string ip, uint16_t port);

#endif
