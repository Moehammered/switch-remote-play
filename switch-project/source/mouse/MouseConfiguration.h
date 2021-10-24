#ifndef __MOUSECONFIGURATION_H__
#define __MOUSECONFIGURATION_H__

#include "MouseOptions.h"
#include "../utils/MappedDataConfiguration.h"
#include <string>

class MouseConfiguration
{
public:
    MouseConfiguration();
    MouseConfiguration(std::string const file);

    bool Save(mouse::MouseConfig const data);
    mouse::MouseConfig const Data() const;

private:
    MappedDataConfiguration<mouse::MouseConfig, mouse::MouseParameters> configMap;
};

#endif