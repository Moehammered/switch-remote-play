#include "NetworkOptions.h"
#include <algorithm>

namespace network
{
    namespace
    {
        std::string const boolToStr(bool const val)
        {
            return val == false ? "no" : "yes";
        }

        bool const boolFromStr(std::string const val)
        {
            return val == "yes";
        }
    }

    std::unordered_map<NetworkParameters, std::string> const networkParamsToStr(NetworkConfig const config)
    {
        auto values = std::unordered_map<NetworkParameters, std::string>{};
        
        values[NetworkParameters::ManualIPAddress] = config.manualIP;
        values[NetworkParameters::ManualIPEnabled] = boolToStr(config.manualIPEnabled);
        values[NetworkParameters::BroadcastAddress] = config.broadcastIP;
        values[NetworkParameters::HandshakePort] = std::to_string(config.handshakePort);
        values[NetworkParameters::BroadcastPort] = std::to_string(config.broadcastPort);
        values[NetworkParameters::CommandPort] = std::to_string(config.commandPort);
        values[NetworkParameters::GamepadPort] = std::to_string(config.gamepadPort);
        values[NetworkParameters::VideoPort] = std::to_string(config.videoPort);
        values[NetworkParameters::AudioPort] = std::to_string(config.audioPort);

        return values;
    }

    NetworkConfig const networkParamsFromStr(std::unordered_map<NetworkParameters, std::string> const & map)
    {
        auto config = NetworkConfig{};

        auto parsePort = [&](auto port, auto & member, auto defaultValue)
        {
            auto entry = map.find(port);
            if(entry != map.end())
            {
                auto str = entry->second;
                auto number = std::atoi(str.c_str());
                auto clamped = std::clamp(number, minPortNumber, maxPortNumber);
                member = (uint16_t)clamped;
            }
            else
                member = defaultValue;
        };

        parsePort(NetworkParameters::HandshakePort, config.handshakePort, defaultHandshakePort);
        parsePort(NetworkParameters::BroadcastPort, config.broadcastPort, defaultBroadcastPort);
        parsePort(NetworkParameters::CommandPort, config.commandPort, defaultCommandPort);
        parsePort(NetworkParameters::GamepadPort, config.gamepadPort, defaultGamepadPort);
        parsePort(NetworkParameters::VideoPort, config.videoPort, defaultVideoPort);
        parsePort(NetworkParameters::AudioPort, config.audioPort, defaultAudioPort);

        {
            auto entry = map.find(NetworkParameters::ManualIPAddress);
            if(entry != map.end())
                config.manualIP = entry->second;
            else
                config.manualIP = defaultManualIPAddress;
        }

        {
            auto entry = map.find(NetworkParameters::BroadcastAddress);
            if(entry != map.end())
                config.broadcastIP = entry->second;
            else
                config.broadcastIP = defaultBroadcastAddress;
        }

        {
            auto entry = map.find(NetworkParameters::ManualIPEnabled);
            if(entry != map.end())
                config.manualIPEnabled = boolFromStr(entry->second);
            else
                config.manualIPEnabled = defaultManualIPEnabled;
        }

        return config;
    }
}