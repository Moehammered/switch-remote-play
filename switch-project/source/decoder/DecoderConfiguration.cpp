#include "DecoderConfiguration.h"
#include "../utils/StringUtils.h"
#include "DecoderOptions.h"

DecoderConfiguration::DecoderConfiguration(std::string const file)
	: configFile{ file }
{
}

bool DecoderConfiguration::Save(DecoderData const data)
{
	return SaveCodecFile(data, configFile);
}

DecoderData const DecoderConfiguration::Data() const
{
	return ParseCodecFile(configFile);
}

std::vector<std::pair<std::string, std::string>> DecoderConfiguration::CodecToFileVariables(DecoderData const data) const
{
	auto lines = DecoderParamsToStr(data);
	auto fileLines = std::vector<std::pair<std::string, std::string>>{};

	for (auto& line : lines)
	{
		auto paramItr = DecoderParamsDesc.find(line.first);
		if (paramItr != DecoderParamsDesc.end())
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

DecoderData DecoderConfiguration::ParseCodecFile(Configuration const& config) const
{
	// go through and split it into param:val pairs
	auto entryMap = std::unordered_map<DecoderParameters, std::string>{};

	for (auto const& param : DecoderParamsDesc)
	{
		auto varVal = std::string{};
		if(config.ExtractVariable(param.second, varVal))
			entryMap[param.first] = varVal;
	}

	return DecoderParamsFromStr(entryMap);
}

bool DecoderConfiguration::SaveCodecFile(DecoderData const data, Configuration& config)
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
