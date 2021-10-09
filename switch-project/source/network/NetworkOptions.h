#ifndef __NETWORKOPTIONS_H__
#define __NETWORKOPTIONS_H__

#include <vector>
#include <unordered_map>
#include <string>

namespace network
{
    enum class Parameters
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

    static std::vector<Parameters> const ParamsList
    {
        Parameters::ManualIPAddress, Parameters::ManualIPEnabled,
        Parameters::BroadcastAddress, Parameters::HandshakePort,
        Parameters::BroadcastPort, Parameters::CommandPort,
        Parameters::GamepadPort, Parameters::VideoPort,
        Parameters::AudioPort
    };

    static std::unordered_map<Parameters, std::string> const ParamsDesc
    {
        { Parameters::ManualIPAddress, "Manual IP Address" },
        { Parameters::ManualIPEnabled, "Manual IP Enabled" },
        { Parameters::BroadcastAddress, "Broadcast Address" },
        { Parameters::HandshakePort, "Handshake Port" },
        { Parameters::BroadcastPort, "Broadcast Port" },
        { Parameters::CommandPort, "Command Port" },
        { Parameters::GamepadPort, "Gamepad Port" },
        { Parameters::VideoPort, "Video Port" },
        { Parameters::AudioPort, "Audio Port" }
    };

    constexpr char DefaultBroadcastAddress[] = "192.168.0.255";
    constexpr char DefaultManualIPAddress[] = "192.168.0.1";
    auto constexpr DefaultManualIPEnabled = true;
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

    auto constexpr NetworkConfigSize = sizeof(NetworkConfig);

    std::unordered_map<Parameters, std::string> const NetworkParamsToStr(NetworkConfig const config);
    NetworkConfig const NetworkParamsFromStr(std::unordered_map<Parameters, std::string> const & map);
}

#endif