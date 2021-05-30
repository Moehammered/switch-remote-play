#ifndef __CONTROLLERCONFIGURATION_H__
#define __CONTROLLERCONFIGURATION_H__

#include <string>
#include <vector>
#include "../utils/Configuration.h"
#include "ControllerOptions.h"

class ControllerConfiguration
{
public:
    ControllerConfiguration();
    ControllerConfiguration(std::string const file);

    bool Save(controller::ControllerConfig const data);
    controller::ControllerConfig const Data() const;

private:
    Configuration configFile;

	std::vector<std::pair<std::string, std::string>> CodecToFileVariables(controller::ControllerConfig const data) const;
	controller::ControllerConfig ParseCodecFile(Configuration const& config) const;
	bool SaveCodecFile(controller::ControllerConfig const data, Configuration& config);
};

#endif
