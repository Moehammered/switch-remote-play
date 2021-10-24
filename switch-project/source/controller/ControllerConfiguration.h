#ifndef __CONTROLLERCONFIGURATION_H__
#define __CONTROLLERCONFIGURATION_H__

#include "ControllerOptions.h"
#include "../utils/MappedDataConfiguration.h"
#include <string>

class ControllerConfiguration
{
public:
    ControllerConfiguration();
    ControllerConfiguration(std::string const file);

    bool Save(controller::ControllerConfig const data);
    controller::ControllerConfig const Data() const;

private:
    MappedDataConfiguration<controller::ControllerConfig, controller::ControllerParameters> configMap;
};

#endif
