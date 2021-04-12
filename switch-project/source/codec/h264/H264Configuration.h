#pragma once

#include <string>
#include <vector>
#include "Configuration.h"
#include "H264Options.h"

class H264Configuration
{
public:
	H264Configuration(std::string const file);

	bool Save(h264::H264Data const data);
	h264::H264Data const Data() const;

private:
	Configuration configFile;

	std::vector<std::pair<std::string, std::string>> CodecToFileVariables(h264::H264Data const data) const;
	h264::H264Data ParseCodecFile(Configuration const& config) const;
	bool SaveCodecFile(h264::H264Data const data, Configuration& config);
};

