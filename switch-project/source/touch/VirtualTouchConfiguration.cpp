#include "VirtualTouchConfiguration.h"

VirtualTouchConfiguration::VirtualTouchConfiguration()
    : configMap{ "sdmc:/switch/switch-remote-play/virtual_touch.ini" , touch::VirtualTouchParamsDesc }
{
}

VirtualTouchConfiguration::VirtualTouchConfiguration(std::string const file)
    : configMap{ file, touch::VirtualTouchParamsDesc }
{
}

bool VirtualTouchConfiguration::Save(touch::VirtualTouchConfig const data)
{
    return configMap.Save(data, touch::VirtualTouchParamsToStr);
}

touch::VirtualTouchConfig const VirtualTouchConfiguration::Data() const
{
    return configMap.Data(touch::VirtualTouchParamsFromStr);
}