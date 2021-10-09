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

    std::unordered_map<Parameters, std::string> const NetworkParamsToStr(NetworkConfig const config)
    {
        auto values = std::unordered_map<Parameters, std::string>{};
        
        values[Parameters::ManualIPAddress] = config.manualIP;
        values[Parameters::ManualIPEnabled] = boolToStr(config.manualIPEnabled);
        values[Parameters::BroadcastAddress] = config.broadcastIP;
        values[Parameters::HandshakePort] = std::to_string(config.handshakePort);
        values[Parameters::BroadcastPort] = std::to_string(config.broadcastPort);
        values[Parameters::CommandPort] = std::to_string(config.commandPort);
        values[Parameters::GamepadPort] = std::to_string(config.gamepadPort);
        values[Parameters::VideoPort] = std::to_string(config.videoPort);
        values[Parameters::AudioPort] = std::to_string(config.audioPort);

        return values;
    }

    NetworkConfig const NetworkParamsFromStr(std::unordered_map<Parameters, std::string> const & map)
    {
        auto config = NetworkConfig{};

        auto parsePort = [&](auto port, auto & member, auto defaultValue)
        {
            auto entry = map.find(port);
            if(entry != map.end())
            {
                auto str = entry->second;
                auto number = std::atoi(str.c_str());
                auto clamped = std::clamp(number, MinPortNumber, MaxPortNumber);
                member = (uint16_t)clamped;
            }
            else
                member = defaultValue;
        };

        parsePort(Parameters::HandshakePort, config.handshakePort, DefaultHandshakePort);
        parsePort(Parameters::BroadcastPort, config.broadcastPort, DefaultBroadcastPort);
        parsePort(Parameters::CommandPort, config.commandPort, DefaultCommandPort);
        parsePort(Parameters::GamepadPort, config.gamepadPort, DefaultGamepadPort);
        parsePort(Parameters::VideoPort, config.videoPort, DefaultVideoPort);
        parsePort(Parameters::AudioPort, config.audioPort, DefaultAudioPort);

        {
            auto entry = map.find(Parameters::ManualIPAddress);
            if(entry != map.end())
                config.manualIP = entry->second;
            else
                config.manualIP = DefaultManualIPAddress;
        }

        {
            auto entry = map.find(Parameters::BroadcastAddress);
            if(entry != map.end())
                config.broadcastIP = entry->second;
            else
                config.broadcastIP = DefaultBroadcastAddress;
        }

        {
            auto entry = map.find(Parameters::ManualIPEnabled);
            if(entry != map.end())
                config.manualIPEnabled = boolFromStr(entry->second);
            else
                config.manualIPEnabled = DefaultManualIPEnabled;
        }

        return config;
    }
}