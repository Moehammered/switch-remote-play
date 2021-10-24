#ifndef __NETWORKCONFIGURATION_H__
#define __NETWORKCONFIGURATION_H__

#include "NetworkOptions.h"
#include "srp/utils/MappedDataConfiguration.h"
#include <string>

class NetworkConfiguration
{
public:
    NetworkConfiguration();
    NetworkConfiguration(std::string const file);

    bool Save(network::NetworkConfig const data);
    network::NetworkConfig const Data() const;

private:
    MappedDataConfiguration<network::NetworkConfig, network::NetworkParameters> configMap;
};
#endif

