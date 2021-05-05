#include "GenericCodecConfiguration.h"
#include "../../utils/StringUtils.h"

GenericCodecConfiguration::GenericCodecConfiguration(std::string const file)
	: configFile{ file }
{
}

bool GenericCodecConfiguration::Save(VideoData const data)
{
	return SaveCodecFile(data, configFile);
}

VideoData const GenericCodecConfiguration::Data() const
{
	return ParseCodecFile(configFile);
}

std::vector<std::pair<std::string, std::string>> GenericCodecConfiguration::CodecToFileVariables(VideoData const data) const
{
	auto lines = VideoParamsToStr(data);
	auto fileLines = std::vector<std::pair<std::string, std::string>>{};

	for (auto& line : lines)
	{
		auto paramItr = VideoParamToDesc.find(line.first);
		if (paramItr != VideoParamToDesc.end())
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

VideoData GenericCodecConfiguration::ParseCodecFile(Configuration const& config) const
{
	// go through and split it into param:val pairs
	auto entryMap = std::unordered_map<VideoParameters, std::string>{};

	for (auto const& param : VideoParamToDesc)
	{
		auto varVal = std::string{};
		if(config.ExtractVariable(param.second, varVal))
			entryMap[param.first] = varVal;
	}

	return VideoParamsFromStr(entryMap);
}

bool GenericCodecConfiguration::SaveCodecFile(VideoData const data, Configuration& config)
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
