#ifndef __DECODERCONFIGURATION_H__
#define __DECODERCONFIGURATION_H__

#include <string>
#include <vector>
#include "DecoderOptions.h"
#include "../utils/MappedDataConfiguration.h"

class DecoderConfiguration
{
public:
    DecoderConfiguration();
    DecoderConfiguration(std::string const file);

    bool Save(DecoderData const data);
    DecoderData const Data() const;

private:
    MappedDataConfiguration<DecoderData, DecoderParameters> configMap;
};

#endif
