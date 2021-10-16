#ifndef __KEYBOARDCONFIGURATION_H__
#define __KEYBOARDCONFIGURATION_H__

#include <string>
#include "KeyboardOptions.h"
#include "../utils/MappedDataConfiguration.h"

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