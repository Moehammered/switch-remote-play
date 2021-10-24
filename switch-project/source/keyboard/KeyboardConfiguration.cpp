#include "KeyboardConfiguration.h"

KeyboardConfiguration::KeyboardConfiguration()
    : configMap{ "sdmc:/switch/switch-remote-play/keyboard.ini", keyboard::keyParamsDesc }
{
}

KeyboardConfiguration::KeyboardConfiguration(std::string const file)
    : configMap{ file, keyboard::keyParamsDesc }
{
}

bool KeyboardConfiguration::Save(keyboard::KeyboardConfig const data)
{
    return configMap.Save(data, keyboard::keybindingsToString);
}

keyboard::KeyboardConfig const KeyboardConfiguration::Data() const
{
    return configMap.Data(keyboard::keybindingsFromStr);
}