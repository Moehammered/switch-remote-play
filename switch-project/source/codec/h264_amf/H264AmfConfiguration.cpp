#include "H264AmfConfiguration.h"
#include "../../utils/StringUtils.h"

H264AmfConfiguration::H264AmfConfiguration()
	: configFile{ "sdmc:/switch/switch-remote-play/h264_amd.ini" }
{
}

H264AmfConfiguration::H264AmfConfiguration(std::string const file)
	: configFile{ file }
{
}

bool H264AmfConfiguration::Save(h264amf::H264AMFData const data)
{
	return SaveCodecFile(data, configFile);
}

h264amf::H264AMFData const H264AmfConfiguration::Data() const
{
	return ParseCodecFile(configFile);
}

std::vector<std::pair<std::string, std::string>> H264AmfConfiguration::CodecToFileVariables(h264amf::H264AMFData const data) const
{
	auto lines = h264amf::CodecParamsToStr(data);

	auto fileLines = std::vector<std::pair<std::string, std::string>>{};

	for (auto& line : lines)
	{
		auto paramItr = h264amf::ParamsDesc.find(line.first);
		if (paramItr != h264amf::ParamsDesc.end())
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

h264amf::H264AMFData H264AmfConfiguration::ParseCodecFile(Configuration const & config) const
{
	// go through and split it into param:val pairs
	auto entryMap = std::unordered_map<h264amf::Parameters, std::string>{};

	for (auto const& param : h264amf::ParamsDesc)
	{
		auto varVal = std::string{};
		if(config.ExtractVariable(param.second, varVal))
			entryMap[param.first] = varVal;
	}

	return h264amf::CodecParamsFromStr(entryMap);
}

bool H264AmfConfiguration::SaveCodecFile(h264amf::H264AMFData const data, Configuration& config)
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