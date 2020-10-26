#ifndef __NETWORKBROADCAST_H__
#define __NETWORKBROADCAST_H__

#include <string>

class NetworkBroadcast
{
    public:
        bool FindConnectionIP(std::string key, std::string& ip);
        std::string LastConnectionIP();
        
    private:
        bool connectionFound;
        std::string lastConnectionIP;
        const int networkPort = 20010;

        int CreateBroadcastSocket();
        int CreateListenerSocket(std::string& localIP);
        int Broadcast(int sock, std::string msg);
        int Listen(int sock, std::string& ip, std::string key);
};

#endif
