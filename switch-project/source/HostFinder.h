#ifndef __HOSTFINDER_H__
#define __HOSTFINDER_H__

#include <thread>
#include <atomic>
#include <string>
#include "Broadcast.h"
#include "Connection.h"

extern std::atomic_bool hostFound;
extern std::string foundIP;

void BroadcastIdentity(std::string const key, std::string const subnet, uint16_t const port);
void Handshake(std::string const key, uint16_t port, Connection*& cn);
// std::thread StartSearch(std::string const key, std::string const subnet, uint16_t const port);

#endif
