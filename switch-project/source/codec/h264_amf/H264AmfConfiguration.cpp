#include "H264AmfConfiguration.h"

H264AmfConfiguration::H264AmfConfiguration()
	: configMap{ "sdmc:/switch/switch-remote-play/h264_amd.ini", h264amf::ParamsDesc }
{
}

H264AmfConfiguration::H264AmfConfiguration(std::string const file)
	: configMap{ file, h264amf::ParamsDesc }
{
}

bool H264AmfConfiguration::Save(h264amf::H264AMFData const data)
{
	return configMap.Save(data, h264amf::CodecParamsToStr);
}

h264amf::H264AMFData const H264AmfConfiguration::Data() const
{
	return configMap.Data(h264amf::CodecParamsFromStr);
}
