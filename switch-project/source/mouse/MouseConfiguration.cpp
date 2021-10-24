#include "MouseConfiguration.h"

MouseConfiguration::MouseConfiguration()
    : configMap{ "sdmc:/switch/switch-remote-play/mouse.ini", mouse::mouseParamsDesc }
{
}

MouseConfiguration::MouseConfiguration(std::string const file)
    : configMap{ file, mouse::mouseParamsDesc }
{
}

bool MouseConfiguration::Save(mouse::MouseConfig const data)
{
    return configMap.Save(data, mouse::mouseParamsToStr);
}

mouse::MouseConfig const MouseConfiguration::Data() const
{
    return configMap.Data(mouse::mouseParamsFromStr);
}