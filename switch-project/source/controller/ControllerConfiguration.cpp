#include "ControllerConfiguration.h"

ControllerConfiguration::ControllerConfiguration()
	: configMap{ "sdmc:/switch/switch-remote-play/controller.ini", controller::ParamsDesc }
{
}

ControllerConfiguration::ControllerConfiguration(std::string const file)
	: configMap{ file, controller::ParamsDesc }
{
}

bool ControllerConfiguration::Save(controller::ControllerConfig const data)
{
	return configMap.Save(data, controller::ControllerParamsToStr);
}

controller::ControllerConfig const ControllerConfiguration::Data() const
{
	return configMap.Data(controller::ControllerParamsFromStr);
}
