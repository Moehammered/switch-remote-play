#ifndef __COMMANDSENDER_H__
#define __COMMANDSENDER_H__

#include "NetworkData.h"
#include <string>

bool ConnectTo(std::string ip, uint16_t port, int& connectionSock);

bool SendCommandPayload(const int connectionSock, CommandPayload const config);

#endif
