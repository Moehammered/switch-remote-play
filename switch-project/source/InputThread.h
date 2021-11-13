#ifndef __INPUTTHREAD_H__
#define __INPUTTHREAD_H__

#include "utils/ConfigHelper.h"
#include "srp/network/NetworkData.h"
#include <string>

void sendStartStreamCommand(std::string ip, uint16_t port, ConfigContainer const settings);

void runGamepadThread(std::string ip, uint16_t port);

#endif
