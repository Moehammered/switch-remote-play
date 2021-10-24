#ifndef __GENERICCODECCONFIGURATION_H__
#define __GENERICCODECCONFIGURATION_H__

#include "GenericOptions.h"
#include "../../utils/MappedDataConfiguration.h"
#include <string>
#include <vector>

class GenericCodecConfiguration
{
public:
	GenericCodecConfiguration();
	GenericCodecConfiguration(std::string const file);

	bool Save(codec::VideoData const data);
	codec::VideoData const Data() const;

private:
	MappedDataConfiguration<codec::VideoData, codec::VideoParameters> configMap;
};

#endif
