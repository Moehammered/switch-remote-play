#ifndef __NETWORKOPTIONS_H__
#define __NETWORKOPTIONS_H__

#include <unordered_map>
#include <string>

namespace network
{
    enum class NetworkParameters
    {
        HandshakePort,
        BroadcastPort,
        CommandPort,
        GamepadPort,
        VideoPort,
        AudioPort
    };

    static std::unordered_map<NetworkParameters, std::string> const ParamsDesc
    {
        { NetworkParameters::HandshakePort, "Handshake Port" },
        { NetworkParameters::BroadcastPort, "Broadcast Port" },
        { NetworkParameters::CommandPort, "Command Port" },
        { NetworkParameters::GamepadPort, "Gamepad Port" },
        { NetworkParameters::VideoPort, "Video Port" },
        { NetworkParameters::AudioPort, "Audio Port" }
    };

    auto constexpr MinPortNumber = 2000;
    auto constexpr MaxPortNumber = 0xFFFF;
    auto constexpr DefaultHandshakePort = 19999;
    auto constexpr DefaultBroadcastPort = 20000;
    auto constexpr DefaultCommandPort = 20001;
    auto constexpr DefaultGamepadPort = 20002;
    auto constexpr DefaultVideoPort = 20003;
    auto constexpr DefaultAudioPort = 20004;

    struct NetworkConfig
    {
        uint16_t handshakePort;
        uint16_t broadcastPort;
        uint16_t commandPort;
        uint16_t gamepadPort;
        uint16_t videoPort;
        uint16_t audioPort;
    };

    auto constexpr NetworkConfigSize = sizeof(NetworkConfig);

    std::unordered_map<NetworkParameters, std::string> const NetworkParamsToStr(NetworkConfig const config);
    NetworkConfig const NetworkParamsFromStr(std::unordered_map<NetworkParameters, std::string> const& map);
}

#endif
