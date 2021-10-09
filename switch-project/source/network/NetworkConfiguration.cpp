#include "NetworkConfiguration.h"

NetworkConfiguration::NetworkConfiguration()
    : configMap{ "sdmc:/switch/switch-remote-play/network.ini", network::ParamsDesc }
{
}

NetworkConfiguration::NetworkConfiguration(std::string const file)
    : configMap{ file, network::ParamsDesc }
{
}

bool NetworkConfiguration::Save(network::NetworkConfig const data)
{
    return configMap.Save(data, network::NetworkParamsToStr);
}

network::NetworkConfig const NetworkConfiguration::Data() const
{
    return configMap.Data(network::NetworkParamsFromStr);
}