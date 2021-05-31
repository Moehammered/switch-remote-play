#include "NetworkConfiguration.h"
#include <algorithm>

NetworkConfiguration::NetworkConfiguration()
    : configFile{"sdmc:/switch/switch-remote-play/network.ini"}, 
    ManualIPTag{"Manual IP"},
    ManualEnabledTag{"Manual Enabled"},
    BroadcastAddressTag{"Broadcast Address"}
{
}

NetworkConfiguration::NetworkConfiguration(std::string const file)
    : configFile{file}, 
    ManualIPTag{"Manual IP"},
    ManualEnabledTag{"Manual Enabled"},
    BroadcastAddressTag{"Broadcast Address"}
{
}

bool NetworkConfiguration::SaveManualIP(std::string const ip)
{
    return SaveIP(ip, ManualIPTag);
}

bool NetworkConfiguration::SaveManualEnabled(bool enabled)
{
    auto value = enabled ? "yes" : "no";
    configFile.CreateVariable(ManualEnabledTag, value);
    return configFile.SaveConfigFile();
}

bool NetworkConfiguration::SaveBroadcastAddress(std::string const ip)
{
    return SaveIP(ip, BroadcastAddressTag);
}

std::string const NetworkConfiguration::ManualIP() const
{
    auto result = LoadIP(ManualIPTag);
    if(result.empty())
        return "192.168.0.1";
    else
        return result;
}

bool NetworkConfiguration::ManualIPEnabled() const
{
    auto result = std::string{"no"};
    configFile.ExtractVariable(ManualEnabledTag, result);

    return result == "yes";
}

std::string const NetworkConfiguration::BroadcastAddress() const
{
    auto result = LoadIP(BroadcastAddressTag);
    if(result.empty())
        return "192.168.0.255";
    else
        return result;
}

bool NetworkConfiguration::SaveIP(std::string const ip, std::string const& tag)
{
    auto periods = std::count(ip.begin(), ip.end(), '.');
    if(periods == 3)
    {
        configFile.CreateVariable(tag, ip);
        return configFile.SaveConfigFile();
    }
    else
        return false;
}

std::string const NetworkConfiguration::LoadIP(std::string const& tag) const
{
    auto result = std::string{};
    if(configFile.ExtractVariable(tag, result))
    {
        auto periods = std::count(result.begin(), result.end(), '.');
        if(periods == 3)
            return result;
        else
            return std::string{};
    }
    else
        return result;
}