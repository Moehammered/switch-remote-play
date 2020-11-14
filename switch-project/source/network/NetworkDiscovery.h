#ifndef __NETWORKDISCOVERY_H__
#define __NETWORKDISCOVERY_H__

#include <thread>
#include <atomic>
#include "Connection.h"
#include "Broadcast.h"

class NetworkDiscovery
{
    public:
        NetworkDiscovery(uint16_t handshakePort, std::string broadcastSubnet, uint16_t broadcastPort);

        void Search();

        bool Searching() const;
        bool HostFound() const;

        bool Stop();

        bool Shutdown();

        std::string const IPAddress() const;

    private:
        std::thread handshakeThread;
        std::thread broadcastThread;
        Connection handshakeConnection;
        Broadcast broadcaster;
        std::string foundIP;
        auto static constexpr handshakeKey = "let-me-play";
};

#endif
