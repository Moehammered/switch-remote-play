#include "ControllerConfiguration.h"
#include "../utils/StringUtils.h"

ControllerConfiguration::ControllerConfiguration()
	: configFile{ "sdmc:/switch/switch-remote-play/controller.ini" }
{
}

ControllerConfiguration::ControllerConfiguration(std::string const file)
	: configFile{ file }
{
}

bool ControllerConfiguration::Save(controller::ControllerConfig const data)
{
	return SaveCodecFile(data, configFile);
}

controller::ControllerConfig const ControllerConfiguration::Data() const
{
	return ParseCodecFile(configFile);
}

std::vector<std::pair<std::string, std::string>> ControllerConfiguration::CodecToFileVariables(controller::ControllerConfig const data) const
{
	auto lines = controller::ControllerParamsToStr(data);
	auto fileLines = std::vector<std::pair<std::string, std::string>>{};

	for (auto& line : lines)
	{
		auto paramItr = controller::ParamsDesc.find(line.first);
		if (paramItr != controller::ParamsDesc.end())
		{
			auto paramStr = paramItr->second;
			Trim(paramStr);

			auto paramVal = line.second;
			Trim(paramVal);

			fileLines.emplace_back(paramStr, paramVal);
		}
	}

	return fileLines;
}

controller::ControllerConfig ControllerConfiguration::ParseCodecFile(Configuration const& config) const
{
	// go through and split it into param:val pairs
	auto entryMap = std::unordered_map<controller::Parameters, std::string>{};

	for (auto const& param : controller::ParamsDesc)
	{
		auto varVal = std::string{};
		if(config.ExtractVariable(param.second, varVal))
			entryMap[param.first] = varVal;
	}

	return controller::ControllerParamsFromStr(entryMap);
}

bool ControllerConfiguration::SaveCodecFile(controller::ControllerConfig const data, Configuration& config)
{
	auto fileLines = CodecToFileVariables(data);

	auto buffer = std::string{};
	for (auto& var : fileLines)
	{
		if (!config.ReplaceVariable(var.first, var.second, buffer))
			config.CreateVariable(var.first, var.second);
	}

	return config.SaveConfigFile();
}
