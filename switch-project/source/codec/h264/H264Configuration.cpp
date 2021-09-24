#include "H264Configuration.h"

H264Configuration::H264Configuration()
	: configMap{ "sdmc:/switch/switch-remote-play/h264_cpu.ini", h264::ParamsToDesc }
{
}

H264Configuration::H264Configuration(std::string const file)
	: configMap{ file, h264::ParamsToDesc }
{
}

bool H264Configuration::Save(h264::H264Data const data)
{
	return configMap.Save(data, h264::CodecParamsToStr);
}

h264::H264Data const H264Configuration::Data() const
{
	return configMap.Data(h264::CodecParamsFromStr);
}
