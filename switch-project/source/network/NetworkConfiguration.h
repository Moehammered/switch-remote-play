#ifndef __NETWORKCONFIGURATION_H__
#define __NETWORKCONFIGURATION_H__

#include <string>
#include "NetworkOptions.h"
#include "../utils/MappedDataConfiguration.h"

class NetworkConfiguration
{
public:
    NetworkConfiguration();
    NetworkConfiguration(std::string const file);

    bool Save(network::NetworkConfig const data);
    network::NetworkConfig const Data() const;

private:
    MappedDataConfiguration<network::NetworkConfig, network::Parameters> configMap;
};

#endif