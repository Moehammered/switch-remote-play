#include "SimulatedMouseConfiguration.h"

SimulatedMouseConfiguration::SimulatedMouseConfiguration()
    : configMap{ "sdmc:/switch/switch-remote-play/simulated_mouse.ini", touch::SimulatedTouchParamsDesc }
{
}

SimulatedMouseConfiguration::SimulatedMouseConfiguration(std::string const file)
    : configMap{ file, touch::SimulatedTouchParamsDesc }
{
}

bool SimulatedMouseConfiguration::Save(touch::SimulatedTouchConfig const data)
{
    return configMap.Save(data, touch::SimulatedTouchParamsToStr);
}

touch::SimulatedTouchConfig const SimulatedMouseConfiguration::Data() const
{
    return configMap.Data(touch::SimulatedTouchParamsFromStr);
}