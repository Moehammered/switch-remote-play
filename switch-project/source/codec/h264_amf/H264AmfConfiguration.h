#ifndef __H264AMFCONFIGURATION_H__
#define __H264AMFCONFIGURATION_H__

#include <string>
#include "../../utils/Configuration.h"
#include "h264amfOptions.h"

class H264AmfConfiguration
{
public:
	H264AmfConfiguration();
	H264AmfConfiguration(std::string const file);

	bool Save(h264amf::H264AMFData const data);
	h264amf::H264AMFData const Data() const;

private:
	Configuration configFile;

	std::vector<std::pair<std::string, std::string>> CodecToFileVariables(h264amf::H264AMFData const data) const;
	h264amf::H264AMFData ParseCodecFile(Configuration const & config) const;
	bool SaveCodecFile(h264amf::H264AMFData const data, Configuration& config);
};

#endif
