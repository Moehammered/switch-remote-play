#pragma once

#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
#include <atomic>
#include "NetworkData.h"

CommandPayload ReadPayloadFromSwitch(SOCKET const& switchSocket);

std::thread StartGamepadListener(int16_t mouseSensitivity, std::atomic_bool& killStream, std::atomic_bool& gamepadActive, uint16_t port, ControllerMode controllerMode);