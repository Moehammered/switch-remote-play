#include "GenericCodecConfiguration.h"

GenericCodecConfiguration::GenericCodecConfiguration()
	: configMap{ "sdmc:/switch/switch-remote-play/common.ini", VideoParamToDesc }
{
}

GenericCodecConfiguration::GenericCodecConfiguration(std::string const file)
	: configMap{ file, VideoParamToDesc }
{
}

bool GenericCodecConfiguration::Save(VideoData const data)
{
	return configMap.Save(data, VideoParamsToStr);
}

VideoData const GenericCodecConfiguration::Data() const
{
	return configMap.Data(VideoParamsFromStr);
}
