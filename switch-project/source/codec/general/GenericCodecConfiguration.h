#ifndef __GENERICCODECCONFIGURATION_H__
#define __GENERICCODECCONFIGURATION_H__

#include <string>
#include <vector>
#include "GenericOptions.h"
#include "../../utils/MappedDataConfiguration.h"

class GenericCodecConfiguration
{
public:
	GenericCodecConfiguration();
	GenericCodecConfiguration(std::string const file);

	bool Save(VideoData const data);
	VideoData const Data() const;

private:
	MappedDataConfiguration<VideoData, VideoParameters> configMap;
};

#endif
