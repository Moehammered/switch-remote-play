#include "AudioOptions.h"
#include <algorithm>

namespace audio
{
    std::unordered_map<AudioParameters, std::string> const audioParamsToString(AudioConfig const config)
    {
        auto values = std::unordered_map<AudioParameters, std::string>{};

        values[AudioParameters::SampleRateFrequency] = std::to_string(config.sampleRateFrequency);
        values[AudioParameters::ChannelCount] = std::to_string(config.channelCount);
        values[AudioParameters::Format] = audioFormatToStr(config.format);
        values[AudioParameters::SampleCount] = std::to_string(config.sampleCount);

        return values;
    }

    AudioConfig const audioParamsFromString(std::unordered_map<AudioParameters, std::string> const& map)
    {
        auto data = AudioConfig{};

        auto parse = [&](AudioParameters const param, auto& member, auto def, auto converter)
        {
            auto const itr = map.find(param);
            if(itr != map.end())
                member = converter(itr->second);
            else
                member = def;
        };

        auto strToSampleRateFreq = [](std::string const& s)
        {
            auto const parsed = (uint32_t)std::atol(s.c_str());
            auto const itr = std::find(supportedSampleRateFrequencies.begin(), supportedSampleRateFrequencies.end(), parsed);
            if(itr != supportedSampleRateFrequencies.end())
                return parsed;
            else
                return defaultSampleRateFrequency;
        };

        parse(AudioParameters::SampleRateFrequency, data.sampleRateFrequency, defaultSampleRateFrequency, strToSampleRateFreq);

        auto strToChannelCount = [](std::string const& s)
        {
            auto const parsed = (uint32_t)std::atol(s.c_str());
            return std::clamp(parsed, minChannelCount, maxChannelCount);
        };

        parse(AudioParameters::ChannelCount, data.channelCount, defaultChannelCount, strToChannelCount);
        parse(AudioParameters::Format, data.format, defaultAudioFormat, audioFormatStrToEnum);

        auto strToSampleCount = [](std::string const& s)
        {
            auto const parsed = (uint32_t)std::atol(s.c_str());
            auto const itr = std::find(supportedSampleCounts.begin(), supportedSampleCounts.end(), parsed);
            if(itr != supportedSampleCounts.end())
                return parsed;
            else
                return defaultSampleCount;
        };
        
        parse(AudioParameters::SampleCount, data.sampleCount, defaultSampleCount, strToSampleCount);

        return data;
    }
}