#include "TouchConfiguration.h"

TouchConfiguration::TouchConfiguration()
    : configMap{ "sdmc:/switch/switch-remote-play/touch_config.ini", touch::touchConfigParamsDesc },
    virtualTouchMap{}, simulatedMouseMap{}
{
}

TouchConfiguration::TouchConfiguration(std::string const file)
    : configMap{ file, touch::touchConfigParamsDesc },
    virtualTouchMap{}, simulatedMouseMap{}
{
}

TouchConfiguration::TouchConfiguration(std::string const file,
                                        std::string const virtualMouseFile,
                                        std::string const simulatedMouseFile)
    : configMap{ file, touch::touchConfigParamsDesc },
    virtualTouchMap{ virtualMouseFile }, simulatedMouseMap{ simulatedMouseFile }
{
}

bool TouchConfiguration::Save(touch::TouchConfig const data)
{
    auto touchFileSaved = configMap.Save(data, touch::touchConfigParamsToStr);
    auto unionFileSaved = false;

    switch (data.touchMode)
    {
    case touch::TouchScreenMode::VirtualTouch:
        unionFileSaved = virtualTouchMap.Save(data.virtualTouchSettings);
        break;
    
    case touch::TouchScreenMode::SimulatedMouse:
        unionFileSaved = simulatedMouseMap.Save(data.simulatedTouchMouseSettings);
        break;

    default:
        break;
    }

    return touchFileSaved && unionFileSaved;
}

touch::TouchConfig const TouchConfiguration::Data() const
{
    auto data = configMap.Data(touch::touchConfigParamsFromStr);
    switch(data.touchMode)
    {
    default:
    case touch::TouchScreenMode::VirtualTouch:
        data.virtualTouchSettings = virtualTouchMap.Data();
        break;

    case touch::TouchScreenMode::SimulatedMouse:
        data.simulatedTouchMouseSettings = simulatedMouseMap.Data();
        break;
    }

    return data;
}