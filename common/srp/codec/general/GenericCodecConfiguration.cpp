#include "GenericCodecConfiguration.h"

GenericCodecConfiguration::GenericCodecConfiguration()
	: configMap{ "sdmc:/switch/switch-remote-play/common.ini", codec::videoParamToDesc }
{
}

GenericCodecConfiguration::GenericCodecConfiguration(std::string const file)
	: configMap{ file, codec::videoParamToDesc }
{
}

bool GenericCodecConfiguration::Save(codec::VideoData const data)
{
	return configMap.Save(data, codec::videoParamsToStr);
}

codec::VideoData const GenericCodecConfiguration::Data() const
{
	return configMap.Data(codec::videoParamsFromStr);
}
