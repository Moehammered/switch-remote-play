#ifndef __DECODERCONFIGURATION_H__
#define __DECODERCONFIGURATION_H__

#include <string>
#include <vector>
#include "DecoderOptions.h"
#include "../utils/Configuration.h"

class DecoderConfiguration
{
public:
    DecoderConfiguration(std::string const file);

    bool Save(DecoderData const data);
    DecoderData const Data() const;

private:
    Configuration configFile;

    std::vector<std::pair<std::string, std::string>> CodecToFileVariables(DecoderData const data) const;
	DecoderData ParseCodecFile(Configuration const& config) const;
	bool SaveCodecFile(DecoderData const data, Configuration& config);
};

#endif
