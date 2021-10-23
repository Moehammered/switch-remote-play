#pragma once

#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
#include <atomic>
#include "NetworkData.h"
#include "DisplayDeviceService.h"
#include "VirtualDesktop.h"

CommandPayload ReadPayloadFromSwitch(SOCKET const& switchSocket);

std::thread StartGamepadListener(DisplayDeviceInfo const sessionDisplay, 
    VirtualDesktop const desktop,
    Resolution const switchResolution,
    controller::ControllerConfig const controllerConfig,
    mouse::MouseConfig const mouseConfig,
    keyboard::KeyboardConfig const keyboardConfig,
    touch::TouchConfig const touchConfig,
    std::atomic_bool& killStream, 
    std::atomic_bool& gamepadActive, 
    uint16_t const port);