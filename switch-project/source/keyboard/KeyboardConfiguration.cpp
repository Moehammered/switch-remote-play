#include "KeyboardConfiguration.h"

KeyboardConfiguration::KeyboardConfiguration()
    : configMap{ "sdmc:/switch/switch-remote-play/keyboard.ini", keyboard::ParamsDesc }
{
}

KeyboardConfiguration::KeyboardConfiguration(std::string const file)
    : configMap{ file, keyboard::ParamsDesc }
{
}

bool KeyboardConfiguration::Save(keyboard::KeyboardConfig const data)
{
    return configMap.Save(data, keyboard::KeybindingsToString);
}

keyboard::KeyboardConfig const KeyboardConfiguration::Data() const
{
    return configMap.Data(keyboard::KeybindingsFromStr);
}