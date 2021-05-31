#include "NetworkDiscovery.h"
#include <chrono>
#include <iostream>

enum SearchState : int32_t
{
    OFF,
    READY,
    SEARCHING,
    FOUND,
    TIMEDOUT
};


std::atomic_int32_t searchState;

void BroadcastIdentity(Broadcast& broadcaster, std::atomic_int32_t& searchState, std::string const key)
{
    auto const waitTime = std::chrono::duration<int, std::milli>(800);
    if(broadcaster.ReadyToSend())
    {
        do
        {
            broadcaster.Send(key);
            std::cout << "broadcasted key: " << key << "\n";
            std::this_thread::sleep_for(waitTime);
        } while(searchState.load(std::memory_order_acquire) == SEARCHING);
    }
}

void Handshake(Connection& handshakeConnection, std::atomic_int32_t& searchState, std::string const key, std::string& foundIP)
{
    auto const waitTime = std::chrono::duration<int, std::milli>(800);
    if(handshakeConnection.Ready())
    {
        do
        {
            std::cout << "handshake start... " << "\n";
            if(handshakeConnection.WaitForConnection())
            {
                std::cout << "handshake waiting... " << "\n";
                //read and compare key here
                char buffer[255];
                for(auto& c : buffer)
                    c = 0;
                
                auto result = recv(handshakeConnection.ConnectedSocket(), buffer, 255, 0);
                if(result >= 0)
                {
                    for(auto i = key.length(); i < 255; ++i)
                        buffer[i] = 0;
                    
                    if(buffer == key)
                    {
                        foundIP = std::string{handshakeConnection.ConnectedIP()};
                        searchState.store(FOUND, std::memory_order_release);
                        std::cout << "handshake succeeded " << foundIP << "\n";
                    }
                }

                std::cout << "handshake completed: " << buffer << "\n";
            }

            handshakeConnection.Disconnect();
            std::this_thread::sleep_for(waitTime);
        } while (searchState.load(std::memory_order_acquire) == SEARCHING);
    }
}

NetworkDiscovery::NetworkDiscovery(uint16_t handshakePort, std::string broadcastSubnet, uint16_t broadcastPort)
    : handshakeThread{}, broadcastThread{},
    handshakeConnection{handshakePort}, broadcaster{broadcastSubnet, broadcastPort},
    foundIP{}
{
    searchState = READY;
}

void NetworkDiscovery::Search()
{
    // startup all the threads
    if(searchState == READY && handshakeConnection.Ready() && broadcaster.ReadyToSend())
    {
        searchState.store(SEARCHING, std::memory_order_release);
        auto handshakeFunction = [&] {
            Handshake(handshakeConnection, searchState, handshakeKey, foundIP);
        };
        handshakeThread = std::thread(handshakeFunction);
        auto broadcastFunction = [&]{
            BroadcastIdentity(broadcaster, searchState, handshakeKey);
        };
        broadcastThread = std::thread(broadcastFunction);
    }
}

bool NetworkDiscovery::Searching() const
{
    return searchState.load(std::memory_order_acquire) == SEARCHING;
}

bool NetworkDiscovery::HostFound() const
{
    return searchState.load(std::memory_order_acquire) == FOUND;
}

std::string const NetworkDiscovery::IPAddress() const
{
    if(searchState.load(std::memory_order_acquire) == FOUND)
        return foundIP;
    else
        return "N/A";
}

bool NetworkDiscovery::Stop()
{
    if(searchState.load(std::memory_order_acquire) == SEARCHING)
    {
        if(handshakeConnection.Shutdown())
        {
            if(handshakeThread.joinable())
                handshakeThread.join();

            if(broadcaster.Shutdown())
            {
                if(broadcastThread.joinable())
                    broadcastThread.join();

                searchState.store(OFF, std::memory_order_release);
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return true;
}

bool NetworkDiscovery::Shutdown()
{
    searchState.store(OFF, std::memory_order_release);
    if(handshakeConnection.Shutdown())
    {
        if(handshakeThread.joinable())
            handshakeThread.join();

        if(broadcaster.Shutdown())
        {
            if(broadcastThread.joinable())
                broadcastThread.join();

            return true;
        }
        else
            return false;
    }
    else
        return false;
}
