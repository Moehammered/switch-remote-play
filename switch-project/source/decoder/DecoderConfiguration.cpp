#include "DecoderConfiguration.h"

DecoderConfiguration::DecoderConfiguration()
	: configMap{ "sdmc:/switch/switch-remote-play/decoder.ini", DecoderParamsDesc }
{
}

DecoderConfiguration::DecoderConfiguration(std::string const file)
	: configMap{ file, DecoderParamsDesc }
{
}

bool DecoderConfiguration::Save(DecoderData const data)
{
	return configMap.Save(data, DecoderParamsToStr);
}

DecoderData const DecoderConfiguration::Data() const
{
	return configMap.Data(DecoderParamsFromStr);
}
