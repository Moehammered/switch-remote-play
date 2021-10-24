#include "SimulatedMouseConfiguration.h"

SimulatedMouseConfiguration::SimulatedMouseConfiguration()
    : configMap{ "sdmc:/switch/switch-remote-play/simulated_mouse.ini", touch::simulatedTouchParamsDesc }
{
}

SimulatedMouseConfiguration::SimulatedMouseConfiguration(std::string const file)
    : configMap{ file, touch::simulatedTouchParamsDesc }
{
}

bool SimulatedMouseConfiguration::Save(touch::SimulatedTouchConfig const data)
{
    return configMap.Save(data, touch::simulatedTouchParamsToStr);
}

touch::SimulatedTouchConfig const SimulatedMouseConfiguration::Data() const
{
    return configMap.Data(touch::simulatedTouchParamsFromStr);
}