#include "NetworkOptions.h"
#include <algorithm>

namespace network
{
    std::unordered_map<NetworkParameters, std::string> const NetworkParamsToStr(NetworkConfig const config)
    {
        auto values = std::unordered_map<NetworkParameters, std::string>{};

        values[NetworkParameters::HandshakePort] = std::to_string(config.handshakePort);
        values[NetworkParameters::BroadcastPort] = std::to_string(config.broadcastPort);
        values[NetworkParameters::CommandPort] = std::to_string(config.commandPort);
        values[NetworkParameters::GamepadPort] = std::to_string(config.gamepadPort);
        values[NetworkParameters::VideoPort] = std::to_string(config.videoPort);
        values[NetworkParameters::AudioPort] = std::to_string(config.audioPort);

        return values;
    }

    NetworkConfig const NetworkParamsFromStr(std::unordered_map<NetworkParameters, std::string> const& map)
    {
        auto config = NetworkConfig{};

        auto parsePort = [&](auto port, auto& member, auto defaultValue)
        {
            auto entry = map.find(port);
            if (entry != map.end())
            {
                auto str = entry->second;
                auto number = std::atoi(str.c_str());
                auto clamped = std::clamp(number, MinPortNumber, MaxPortNumber);
                member = (uint16_t)clamped;
            }
            else
                member = defaultValue;
        };

        parsePort(NetworkParameters::HandshakePort, config.handshakePort, DefaultHandshakePort);
        parsePort(NetworkParameters::BroadcastPort, config.broadcastPort, DefaultBroadcastPort);
        parsePort(NetworkParameters::CommandPort, config.commandPort, DefaultCommandPort);
        parsePort(NetworkParameters::GamepadPort, config.gamepadPort, DefaultGamepadPort);
        parsePort(NetworkParameters::VideoPort, config.videoPort, DefaultVideoPort);
        parsePort(NetworkParameters::AudioPort, config.audioPort, DefaultAudioPort);

        return config;
    }
}