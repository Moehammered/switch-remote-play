#ifndef __VIRTUALTOUCHCONFIGURATION_H__
#define __VIRTUALTOUCHCONFIGURATION_H__

#include <string>
#include "VirtualTouchOptions.h"
#include "../utils/MappedDataConfiguration.h"

class VirtualTouchConfiguration
{
public:
    VirtualTouchConfiguration();
    VirtualTouchConfiguration(std::string const file);

    bool Save(touch::VirtualTouchConfig const data);
    touch::VirtualTouchConfig const Data() const;

private:
    MappedDataConfiguration<touch::VirtualTouchConfig, touch::VirtualTouchParameters> configMap;
};

#endif