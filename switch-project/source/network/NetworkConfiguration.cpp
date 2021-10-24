#include "NetworkConfiguration.h"

NetworkConfiguration::NetworkConfiguration()
    : configMap{ "sdmc:/switch/switch-remote-play/network.ini", network::networkParamsDesc }
{
}

NetworkConfiguration::NetworkConfiguration(std::string const file)
    : configMap{ file, network::networkParamsDesc }
{
}

bool NetworkConfiguration::Save(network::NetworkConfig const data)
{
    return configMap.Save(data, network::networkParamsToStr);
}

network::NetworkConfig const NetworkConfiguration::Data() const
{
    return configMap.Data(network::networkParamsFromStr);
}