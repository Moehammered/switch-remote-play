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
        SampleCount,
        UseAudioQueue
    };

    static std::vector<AudioParameters> const audioParamsList
    {
        AudioParameters::SampleRateFrequency, AudioParameters::ChannelCount,
        AudioParameters::Format, AudioParameters::SampleCount,
        AudioParameters::UseAudioQueue
    };

    static std::unordered_map<AudioParameters, std::string> const audioParamsDesc
    {
        { AudioParameters::SampleRateFrequency, "Sample Rate Frequency" },
        { AudioParameters::ChannelCount, "Channel Count" },
        { AudioParameters::Format, "Format" },
        { AudioParameters::SampleCount, "Sample Count" },
        { AudioParameters::UseAudioQueue, "Use Audio Queue" }
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
    uint32_t constexpr minChannelCount = 2;
    uint32_t constexpr maxChannelCount = 2;

    AudioFormat constexpr defaultAudioFormat = AudioFormat::DefaultSigned16Bit;

    uint32_t constexpr calculateSampleCountRequirement(uint32_t sampleBufferCount, uint32_t const channels = 2, uint32_t const bitlength = 16, uint32_t const bufferAlignmentSize = 8192)
    {
        auto const singleSampleByteSize = (channels * bitlength) / 4;
        auto const minSampleSize = bufferAlignmentSize / singleSampleByteSize;

        return minSampleSize << sampleBufferCount;
    }

    static std::vector<uint32_t> const supportedSampleCounts
    {
        calculateSampleCountRequirement(2),
        calculateSampleCountRequirement(3),
        calculateSampleCountRequirement(4),
        calculateSampleCountRequirement(5),
        calculateSampleCountRequirement(6)
    };
    uint32_t constexpr defaultSampleCount = calculateSampleCountRequirement(4);

    uint8_t constexpr defaultUseAudioQueue = 0;

    struct AudioConfig
    {
        uint32_t sampleRateFrequency;
        uint32_t channelCount;
        AudioFormat format;
        uint32_t sampleCount;
        uint8_t useAudioQueue;
    };

    auto constexpr audioConfigSize = sizeof(AudioConfig);

    std::unordered_map<AudioParameters, std::string> const audioParamsToString(AudioConfig const config);
    AudioConfig const audioParamsFromString(std::unordered_map<AudioParameters, std::string> const& map);
}

#endif
