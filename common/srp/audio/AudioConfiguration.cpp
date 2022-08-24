#include "AudioConfiguration.h"

AudioConfiguration::AudioConfiguration()
	: configMap{ "sdmc:/switch/switch-remote-play/audio.ini", audio::audioParamsDesc }
{
}

AudioConfiguration::AudioConfiguration(std::string const file)
	: configMap{ file, audio::audioParamsDesc }
{
}

bool AudioConfiguration::Save(audio::AudioConfig const data)
{
	return configMap.Save(data, audio::audioParamsToString);
}

audio::AudioConfig const AudioConfiguration::Data() const
{
	return configMap.Data(audio::audioParamsFromString);
}
