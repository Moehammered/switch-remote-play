#include "H264AmfConfiguration.h"

H264AmfConfiguration::H264AmfConfiguration()
	: configMap{ "sdmc:/switch/switch-remote-play/h264_amd.ini", h264amf::h264AmfParamsDesc }
{
}

H264AmfConfiguration::H264AmfConfiguration(std::string const file)
	: configMap{ file, h264amf::h264AmfParamsDesc }
{
}

bool H264AmfConfiguration::Save(h264amf::H264AmfData const data)
{
	return configMap.Save(data, h264amf::codecParamsToStr);
}

h264amf::H264AmfData const H264AmfConfiguration::Data() const
{
	return configMap.Data(h264amf::codecParamsFromStr);
}
