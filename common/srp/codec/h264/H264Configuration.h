#ifndef __H264CONFIGURATION_H__
#define __H264CONFIGURATION_H__

#include "H264Options.h"
#include "../../utils/MappedDataConfiguration.h"
#include <string>
#include <vector>

class H264Configuration
{
public:
	H264Configuration();
	H264Configuration(std::string const file);

	bool Save(h264::H264Data const data);
	h264::H264Data const Data() const;

private:
	MappedDataConfiguration<h264::H264Data, h264::H264Parameters> configMap;
};

#endif