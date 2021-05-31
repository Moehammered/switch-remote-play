#ifndef __NETWORKCONFIGURATION_H__
#define __NETWORKCONFIGURATION_H__

#include <string>
#include "../utils/Configuration.h"

class NetworkConfiguration
{
public:
    NetworkConfiguration();
    NetworkConfiguration(std::string const file);

    bool SaveManualIP(std::string const ip);
    bool SaveManualEnabled(bool enabled);
    bool SaveBroadcastAddress(std::string const ip);

    std::string const ManualIP() const;
    bool ManualIPEnabled() const;
    std::string const BroadcastAddress() const;

private:
    Configuration configFile;
    std::string const ManualIPTag;
    std::string const ManualEnabledTag;
    std::string const BroadcastAddressTag;

    bool SaveIP(std::string const ip, std::string const& tag);
    std::string const LoadIP(std::string const& tag) const;
};

#endif
