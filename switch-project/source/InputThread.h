#ifndef __INPUTTHREAD_H__
#define __INPUTTHREAD_H__

#include "network/NetworkData.h"
#include <string>

enum StreamState : int32_t
{
    INACTIVE,
    REQUESTED,
    ACTIVE,
    QUIT
};

void runStartConfiguredStreamCommand(std::string ip, uint16_t port, 
    EncoderConfig const config, 
    controller::ControllerConfig const controllerConfig,
    mouse::MouseConfig const mouseConfig,
    keyboard::KeyboardConfig const keyboardConfig,
    touch::TouchConfig const touchConfig);

void runGamepadThread(std::string ip, uint16_t port);

#endif
