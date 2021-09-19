#pragma once

#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
#include <atomic>
#include "NetworkData.h"
#include "DisplayDeviceService.h"

CommandPayload ReadPayloadFromSwitch(SOCKET const& switchSocket);

std::thread StartGamepadListener(DisplayDeviceInfo sessionDisplay, 
    controller::ControllerConfig controllerConfig,
    mouse::MouseConfig mouseConfig,
    touch::TouchConfig touchConfig,
    std::atomic_bool& killStream, 
    std::atomic_bool& gamepadActive, 
    uint16_t port);