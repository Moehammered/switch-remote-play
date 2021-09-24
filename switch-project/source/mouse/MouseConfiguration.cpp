#include "MouseConfiguration.h"

MouseConfiguration::MouseConfiguration()
    : configMap{ "sdmc:/switch/switch-remote-play/mouse.ini", mouse::ParamsDesc }
{
}

MouseConfiguration::MouseConfiguration(std::string const file)
    : configMap{ file, mouse::ParamsDesc }
{
}

bool MouseConfiguration::Save(mouse::MouseConfig const data)
{
    return configMap.Save(data, mouse::MouseParamsToStr);
}

mouse::MouseConfig const MouseConfiguration::Data() const
{
    return configMap.Data(mouse::MouseParamsFromStr);
}