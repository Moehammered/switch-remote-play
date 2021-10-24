#include "VirtualTouchConfiguration.h"

VirtualTouchConfiguration::VirtualTouchConfiguration()
    : configMap{ "sdmc:/switch/switch-remote-play/virtual_touch.ini" , touch::virtualTouchParamsDesc }
{
}

VirtualTouchConfiguration::VirtualTouchConfiguration(std::string const file)
    : configMap{ file, touch::virtualTouchParamsDesc }
{
}

bool VirtualTouchConfiguration::Save(touch::VirtualTouchConfig const data)
{
    return configMap.Save(data, touch::virtualTouchParamsToStr);
}

touch::VirtualTouchConfig const VirtualTouchConfiguration::Data() const
{
    return configMap.Data(touch::virtualTouchParamsFromStr);
}