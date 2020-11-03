#include "HostFinder.h"
#include "Broadcast.h"
#include "Connection.h"
#include <sys/types.h>
#include <sys/time.h>
#include <iostream>
#include <string.h>
#include <chrono>

std::atomic_bool hostFound{};
std::string foundIP{};

void BroadcastIdentity(std::string const key, std::string const subnet, uint16_t const port)
{
    auto broadcaster = Broadcast(subnet, port);
    auto const waitTime = std::chrono::duration<int, std::milli>(800);

    if(broadcaster.ReadyToSend())
    {
        while(!hostFound)
        {
            if(!broadcaster.Send(key))
                std::cout << "Error broadcasting: " << strerror(errno) << std::endl;
            // else
            //     std::cout << "Broadcasted: " << key << std::endl;

            std::this_thread::sleep_for(waitTime);
        }
    }

    broadcaster.Close();

    // std::cout << "Closing broadcaster" << std::endl;
}

void Handshake(std::string const key, uint16_t port, Connection*& cn)
{
    auto const waitTime = std::chrono::duration<int, std::milli>(500);
    do
    {
        auto connection = Connection(port);
        cn = &connection;
        if(connection.Ready())
        {
            std::cout << "Waiting for handshake..." << std::endl;
            // timeval timeout;
            // timeout.tv_sec  = 7;  // after 7 seconds connect() will timeout
            // timeout.tv_usec = 0;
            // // this isn't working properly
            // setsockopt(connection.ListeningSocket(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
            if(connection.WaitForConnection())
            {
                //read and compare key here
                char buffer[255];
                for(auto& c : buffer)
                    c = 0;
                
                auto result = recv(connection.ConnectedSocket(), buffer, 255, 0);
                if(result < 0)
                {
                    std::cout << "Failed to receive from connected client" << std::endl;
                }
                else
                {
                    for(auto i = key.length(); i < 255; ++i)
                        buffer[i] = 0;
                    
                    if(buffer == key)
                    {
                        foundIP = connection.ConnectedIP();
                        std::cout << "Found host: " << foundIP << std::endl;
                        hostFound = true;
                    }
                    else
                    {
                        std::cout << "Key didn't match: " << buffer << std::endl;
                    }
                }
            }
        }

        connection.Close();
        cn = nullptr;
        std::cout << "Waiting for handshake..." << std::endl;
        std::this_thread::sleep_for(waitTime);
    } while (!hostFound);
}

// std::thread StartSearch(std::string const key, std::string const subnet, uint16_t const port)
// {
//     hostFound = false;
//     foundIP = std::string{};

//     auto searchingThread = [&]{
//         Connection* cn = nullptr;
//         auto broadcastingThread = std::thread(BroadcastIdentity, key, subnet, port);
//         auto connectionThread = std::thread(Handshake, key, 20001, std::ref(cn));

//         std::cout << "Joining broadcaster thread... " << foundIP << std::endl;
//         if(broadcastingThread.joinable())
//             broadcastingThread.join();
//         std::cout << "Joining handshake thread... " << foundIP << std::endl;            
//         if(connectionThread.joinable())
//             connectionThread.join();
//     };

//     return std::thread(searchingThread);
// }