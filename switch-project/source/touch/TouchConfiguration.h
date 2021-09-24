#ifndef __TOUCHCONFIGURATION_H__
#define __TOUCHCONFIGURATION_H__

#include <string>
#include "TouchOptions.h"
#include "SimulatedMouseConfiguration.h"
#include "VirtualTouchConfiguration.h"
#include "../utils/MappedDataConfiguration.h"

class TouchConfiguration
{
public:
    TouchConfiguration();
    TouchConfiguration(std::string const file);
    TouchConfiguration(std::string const file,
        std::string const virtualMouseFile,
        std::string const simulatedMouseFile);

    bool Save(touch::TouchConfig const data);
    touch::TouchConfig const Data() const;

private:
    MappedDataConfiguration<touch::TouchConfig, touch::TouchConfigParameters> configMap;
    VirtualTouchConfiguration virtualTouchMap;
    SimulatedMouseConfiguration simulatedMouseMap;
};

#endif
