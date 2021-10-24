#ifndef __KEYBOARDCONFIGURATION_H__
#define __KEYBOARDCONFIGURATION_H__

#include "KeyboardOptions.h"
#include "../utils/MappedDataConfiguration.h"
#include <string>

class KeyboardConfiguration
{
public:
    KeyboardConfiguration();
    KeyboardConfiguration(std::string const file);

    bool Save(keyboard::KeyboardConfig const data);
    keyboard::KeyboardConfig const Data() const;

private:
    MappedDataConfiguration<keyboard::KeyboardConfig, keyboard::KeyParameter> configMap;
};

#endif