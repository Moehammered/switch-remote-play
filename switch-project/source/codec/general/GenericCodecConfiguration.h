#ifndef __GENERICCODECCONFIGURATION_H__
#define __GENERICCODECCONFIGURATION_H__

#include <string>
#include <vector>
#include "../../utils/Configuration.h"
#include "GenericOptions.h"

class GenericCodecConfiguration
{
public:
	GenericCodecConfiguration();
	GenericCodecConfiguration(std::string const file);

	bool Save(VideoData const data);
	VideoData const Data() const;

private:
	Configuration configFile;

	std::vector<std::pair<std::string, std::string>> CodecToFileVariables(VideoData const data) const;
	VideoData ParseCodecFile(Configuration const& config) const;
	bool SaveCodecFile(VideoData const data, Configuration& config);
};

#endif
