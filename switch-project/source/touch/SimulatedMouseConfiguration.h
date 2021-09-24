#ifndef __SIMULATEDMOUSECONFIGURATION_H__
#define __SIMULATEDMOUSECONFIGURATION_H__

#include <string>
#include "SimulatedMouseOptions.h"
#include "../utils/MappedDataConfiguration.h"

class SimulatedMouseConfiguration
{
public:
    SimulatedMouseConfiguration();
    SimulatedMouseConfiguration(std::string const file);

    bool Save(touch::SimulatedTouchConfig const data);
    touch::SimulatedTouchConfig const Data() const;

private:
    MappedDataConfiguration<touch::SimulatedTouchConfig, touch::SimulatedTouchMouseParameters> configMap;
};

#endif