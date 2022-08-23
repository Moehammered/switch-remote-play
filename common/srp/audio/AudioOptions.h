#ifndef __AUDIOOPTIONS_H__
#define __AUDIOOPTIONS_H__

#include "AudioFormat.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace audio
{
    enum class AudioParameters
    {
        SampleRateFrequency,
        ChannelCount,
        Format,
        SampleCount
    };

    static std::vector<AudioParameters> const audioParamsList
    {
        AudioParameters::SampleRateFrequency, AudioParameters::ChannelCount,
        AudioParameters::Format, AudioParameters::SampleCount
    };

    static std::unordered_map<AudioParameters, std::string> const audioParamsDesc
    {
        { AudioParameters::SampleRateFrequency, "Sample Rate Frequency" },
        { AudioParameters::ChannelCount, "Channel Count" },
        { AudioParameters::Format, "Format" },
        { AudioParameters::SampleCount, "Sample Count" } 
    };

    static std::vector<AudioFormat> const supportedAudioFormats
    {
        AudioFormat::LittleEndian_Signed16Bit
    };

    static std::vector<uint32_t> const supportedSampleRateFrequencies
    {
        44100, 48000
    };

    uint32_t constexpr defaultSampleRateFrequency = 44100;
    
    uint32_t constexpr defaultChannelCount = 2;
    uint32_t constexpr minChannelCount = 1;
    uint32_t constexpr maxChannelCount = 2;

    AudioFormat constexpr defaultAudioFormat = AudioFormat::DefaultSigned16Bit;

    static std::vector<uint32_t> supportedSampleCounts
    {
        4096, 8192, 16384
    };
    uint32_t constexpr defaultSampleCount = 8192; //make a function that assumes buffer size from 2 by 16bit and buffer alignment of 8192

    struct AudioConfig
    {
        uint32_t sampleRateFrequency;
        uint32_t channelCount;
        AudioFormat format;
        uint32_t sampleCount;
    };

    auto constexpr audioConfigSize = sizeof(AudioConfig);

    std::unordered_map<AudioParameters, std::string> const audioParamsToString(AudioConfig const config);
    AudioConfig const audioParamsFromString(std::unordered_map<AudioParameters, std::string> const& map);
}

#endif
