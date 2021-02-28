#ifndef __H264OPTIONS_H__
#define __H264OPTIONS_H__

#include <unordered_map>
#include <vector>
#include <string>

namespace h264
{
    enum Parameters
    {
        ConstantRateFactor,
        Preset,
        ParamCount
    };

    static std::vector<Parameters> const ParamsList
    {
        Parameters::ConstantRateFactor, Parameters::Preset
    };

    static std::unordered_map<Parameters, std::string> const ParamsToDesc
    {
        { Parameters::ConstantRateFactor, "Constant Rate Factor" },
        { Parameters::Preset, "Preset" }
    };

    static std::unordered_map<Parameters, std::string> const ParamsToStr
    {
        { Parameters::ConstantRateFactor, "constant_rate_factor" },
        { Parameters::Preset, "preset" }
    };

    int32_t constexpr MaxCRF {30};
    int32_t constexpr MinCRF {0};
    int32_t constexpr DefaultCRF {15};

    int32_t ConstantRateFactorStrToInt(std::string s);
    std::string ConstantRateFactorIntToStr(int32_t crf);
    std::string ConstantRateFactorToDesc(int32_t crf);

    enum EncoderPreset
    {
        UltraFast = 0,
        VeryFast,
        Fast,
        Medium,
        Slow,
        VerySlow,
        Count
    };

    static std::unordered_map<EncoderPreset, std::string> const EncoderPresetDescMap
    {
        { EncoderPreset::UltraFast, "Ultra Fast (bad quality)" },
        { EncoderPreset::VeryFast, "Very Fast" },
        { EncoderPreset::Fast, "Fast" },
        { EncoderPreset::Medium, "Medium (balanced quality)" },
        { EncoderPreset::Slow, "Slow" },
        { EncoderPreset::VerySlow, "Very Slow (best quality)" }
    };

    static std::unordered_map<EncoderPreset, std::string> const EncoderPresetStrMap
    {
        { EncoderPreset::UltraFast, "ultrafast" },
        { EncoderPreset::VeryFast, "veryfast" },
        { EncoderPreset::Fast, "fast" },
        { EncoderPreset::Medium, "medium" },
        { EncoderPreset::Slow, "slow" },
        { EncoderPreset::VerySlow, "veryslow" }
    };

    EncoderPreset EncoderPresetStrToEnum(std::string s);
    std::string EncoderPresetToStr(EncoderPreset preset);
    std::string EncoderPresetToDesc(EncoderPreset preset);
}

#endif
