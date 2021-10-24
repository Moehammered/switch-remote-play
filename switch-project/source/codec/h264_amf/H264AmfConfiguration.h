#ifndef __H264AMFCONFIGURATION_H__
#define __H264AMFCONFIGURATION_H__

#include "h264AmfOptions.h"
#include "../../utils/MappedDataConfiguration.h"
#include <string>

class H264AmfConfiguration
{
public:
	H264AmfConfiguration();
	H264AmfConfiguration(std::string const file);

	bool Save(h264amf::H264AmfData const data);
	h264amf::H264AmfData const Data() const;

private:
	MappedDataConfiguration<h264amf::H264AmfData, h264amf::H264AmfParameters> configMap;
};

#endif
