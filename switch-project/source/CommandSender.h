#ifndef __COMMANDSENDER_H__
#define __COMMANDSENDER_H__

#include <string>
#include "NetworkData.h"

bool ConnectTo(std::string ip, uint16_t port, int& connectionSock);

bool SendCode(const int connectionSock, Command commandCode);

bool SendStreamConfig(const int connectionSock, FFMPEG_Config config);

bool SendCommandPayload(const int connectionSock, CommandPayload const config);

#endif
