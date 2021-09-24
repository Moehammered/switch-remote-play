#ifndef __H264AMFCONFIGURATION_H__
#define __H264AMFCONFIGURATION_H__

#include <string>
#include "h264amfOptions.h"
#include "../../utils/MappedDataConfiguration.h"

class H264AmfConfiguration
{
public:
	H264AmfConfiguration();
	H264AmfConfiguration(std::string const file);

	bool Save(h264amf::H264AMFData const data);
	h264amf::H264AMFData const Data() const;

private:
	MappedDataConfiguration<h264amf::H264AMFData, h264amf::Parameters> configMap;
};

#endif
