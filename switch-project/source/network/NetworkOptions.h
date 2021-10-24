#ifndef __NETWORKOPTIONS_H__
#define __NETWORKOPTIONS_H__

#include <vector>
#include <unordered_map>
#include <string>

namespace network
{
    enum class NetworkParameters
    {
        ManualIPAddress,
        ManualIPEnabled,
        BroadcastAddress,
        HandshakePort,
        BroadcastPort,
        CommandPort,
        GamepadPort,
        VideoPort,
        AudioPort
    };

    static std::vector<NetworkParameters> const networkParamsList
    {
        NetworkParameters::ManualIPAddress, NetworkParameters::ManualIPEnabled,
        NetworkParameters::BroadcastAddress, NetworkParameters::HandshakePort,
        NetworkParameters::BroadcastPort, NetworkParameters::CommandPort,
        NetworkParameters::GamepadPort, NetworkParameters::VideoPort,
        NetworkParameters::AudioPort
    };

    static std::unordered_map<NetworkParameters, std::string> const networkParamsDesc
    {
        { NetworkParameters::ManualIPAddress, "Manual IP Address" },
        { NetworkParameters::ManualIPEnabled, "Manual IP Enabled" },
        { NetworkParameters::BroadcastAddress, "Broadcast Address" },
        { NetworkParameters::HandshakePort, "Handshake Port" },
        { NetworkParameters::BroadcastPort, "Broadcast Port" },
        { NetworkParameters::CommandPort, "Command Port" },
        { NetworkParameters::GamepadPort, "Gamepad Port" },
        { NetworkParameters::VideoPort, "Video Port" },
        { NetworkParameters::AudioPort, "Audio Port" }
    };

    char constexpr defaultBroadcastAddress[] = "192.168.0.255";
    char constexpr defaultManualIPAddress[] = "192.168.0.1";
    auto constexpr defaultManualIPEnabled = true;
    auto constexpr minPortNumber = 2000;
    auto constexpr maxPortNumber = 0xFFFF;
    auto constexpr defaultHandshakePort = 19999;
    auto constexpr defaultBroadcastPort = 20000;
    auto constexpr defaultCommandPort = 20001;
    auto constexpr defaultGamepadPort = 20002;
    auto constexpr defaultVideoPort = 20003;
    auto constexpr defaultAudioPort = 20004;

    struct NetworkConfig
    {
        std::string manualIP;
        std::string broadcastIP;
        uint16_t handshakePort;
        uint16_t broadcastPort;
        uint16_t commandPort;
        uint16_t gamepadPort;
        uint16_t videoPort;
        uint16_t audioPort;
        bool manualIPEnabled;
    };

    auto constexpr networkConfigSize = sizeof(NetworkConfig);

    std::unordered_map<NetworkParameters, std::string> const networkParamsToStr(NetworkConfig const config);
    NetworkConfig const networkParamsFromStr(std::unordered_map<NetworkParameters, std::string> const & map);
}

#endif