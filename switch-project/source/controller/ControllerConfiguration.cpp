#include "ControllerConfiguration.h"

ControllerConfiguration::ControllerConfiguration()
	: configMap{ "sdmc:/switch/switch-remote-play/controller.ini", controller::controllerParamsDesc }
{
}

ControllerConfiguration::ControllerConfiguration(std::string const file)
	: configMap{ file, controller::controllerParamsDesc }
{
}

bool ControllerConfiguration::Save(controller::ControllerConfig const data)
{
	return configMap.Save(data, controller::controllerParamsToStr);
}

controller::ControllerConfig const ControllerConfiguration::Data() const
{
	return configMap.Data(controller::controllerParamsFromStr);
}
