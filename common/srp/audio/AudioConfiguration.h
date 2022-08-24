#ifndef __AUDIOCONFIGURATION_H__
#define __AUDIOCONFIGURATION_H__

#include "AudioOptions.h"
#include "../utils/MappedDataConfiguration.h"
#include <string>

class AudioConfiguration
{
public:
    AudioConfiguration();
    AudioConfiguration(std::string const file);

    bool Save(audio::AudioConfig const data);
    audio::AudioConfig const Data() const;

private:
    MappedDataConfiguration<audio::AudioConfig, audio::AudioParameters> configMap;
};

#endif
