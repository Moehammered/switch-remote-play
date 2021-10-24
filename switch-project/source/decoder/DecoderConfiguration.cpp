#include "DecoderConfiguration.h"

DecoderConfiguration::DecoderConfiguration()
	: configMap{ "sdmc:/switch/switch-remote-play/decoder.ini", decoderParamsDesc }
{
}

DecoderConfiguration::DecoderConfiguration(std::string const file)
	: configMap{ file, decoderParamsDesc }
{
}

bool DecoderConfiguration::Save(DecoderData const data)
{
	return configMap.Save(data, decoderParamsToStr);
}

DecoderData const DecoderConfiguration::Data() const
{
	return configMap.Data(decoderParamsFromStr);
}
