#include "H264Configuration.h"
#include "../../utils/StringUtils.h"

H264Configuration::H264Configuration()
	: configFile{ "sdmc:/switch/switch-remote-play/h264_cpu.ini" }
{
}

H264Configuration::H264Configuration(std::string const file)
	: configFile{ file }
{
}

bool H264Configuration::Save(h264::H264Data const data)
{
	return SaveCodecFile(data, configFile);
}

h264::H264Data const H264Configuration::Data() const
{
	return ParseCodecFile(configFile);
}

std::vector<std::pair<std::string, std::string>> H264Configuration::CodecToFileVariables(h264::H264Data const data) const
{
	auto lines = h264::CodecParamsToStr(data);
	auto fileLines = std::vector<std::pair<std::string, std::string>>{};

	for (auto& line : lines)
	{
		auto paramItr = h264::ParamsToDesc.find(line.first);
		if (paramItr != h264::ParamsToDesc.end())
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

h264::H264Data H264Configuration::ParseCodecFile(Configuration const& config) const
{
	// go through and split it into param:val pairs
	auto entryMap = std::unordered_map<h264::Parameters, std::string>{};

	for (auto const& param : h264::ParamsToDesc)
	{
		auto varVal = std::string{};
		if(config.ExtractVariable(param.second, varVal))
			entryMap[param.first] = varVal;
	}

	return h264::CodecParamsFromStr(entryMap);
}

bool H264Configuration::SaveCodecFile(h264::H264Data const data, Configuration& config)
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
