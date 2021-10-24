#ifndef __H264OPTIONS_H__
#define __H264OPTIONS_H__

#include <unordered_map>
#include <vector>
#include <string>

namespace h264
{
    enum class H264Parameters : int
    {
        ConstantRateFactor,
        Preset,
        RateControlMode,
        Profile
    };

    static std::vector<H264Parameters> const h264ParamsList
    {
        H264Parameters::ConstantRateFactor,
        H264Parameters::Preset,
        H264Parameters::RateControlMode,
        H264Parameters::Profile
    };

    static std::unordered_map<H264Parameters, std::string> const h264ParamsDesc
    {
        { H264Parameters::ConstantRateFactor, "Constant Rate Factor" },
        { H264Parameters::Preset, "Preset" },
        { H264Parameters::RateControlMode, "Rate Control Mode" },
        { H264Parameters::Profile, "Profile" }
    };

    int32_t constexpr maxCRF {30};
    int32_t constexpr minCRF {0};
    int32_t constexpr defaultCRF {18};

    int32_t constantRateFactorFromStr(std::string s);
    std::string constantRateFactorToStr(int32_t crf);
    std::string constantRateFactorToDesc(int32_t crf);

    enum class EncoderPreset
    {
        UltraFast = 0,
        VeryFast,
        Fast,
        Medium,
        Slow,
        VerySlow,
        Count
    };

    static std::unordered_map<EncoderPreset, std::string> const encoderPresetDescMap
    {
        { EncoderPreset::UltraFast, "Ultra Fast (bad quality)" },
        { EncoderPreset::VeryFast, "Very Fast" },
        { EncoderPreset::Fast, "Fast" },
        { EncoderPreset::Medium, "Medium (balanced quality)" },
        { EncoderPreset::Slow, "Slow" },
        { EncoderPreset::VerySlow, "Very Slow (best quality)" }
    };

    static std::unordered_map<EncoderPreset, std::string> const encoderPresetStrMap
    {
        { EncoderPreset::UltraFast, "ultrafast" },
        { EncoderPreset::VeryFast, "veryfast" },
        { EncoderPreset::Fast, "fast" },
        { EncoderPreset::Medium, "medium" },
        { EncoderPreset::Slow, "slow" },
        { EncoderPreset::VerySlow, "veryslow" }
    };

    EncoderPreset encoderPresetFromStr(std::string s);
    std::string encoderPresetToStr(EncoderPreset preset);
    std::string encoderPresetToDesc(EncoderPreset preset);

    enum class EncoderBitrateMode : int16_t
    {
        VariableBitrate,
        ConstantBitrate
    };

    static std::unordered_map<EncoderBitrateMode, std::string> const encoderBitrateModeDescMap
    {
        { EncoderBitrateMode::VariableBitrate, "Variable Bitrate" },
        { EncoderBitrateMode::ConstantBitrate, "Constant Bitrate" }
    };

    static std::unordered_map<EncoderBitrateMode, std::string> const encoderBitrateModeStrMap
    {
        { EncoderBitrateMode::VariableBitrate, "variable bitrate" },
        { EncoderBitrateMode::ConstantBitrate, "constant bitrate" }
    };

    EncoderBitrateMode encoderBitrateModeFromStr(std::string s);
    std::string encoderBitrateModeToStr(EncoderBitrateMode mode);
    std::string encoderBitrateModeToDesc(EncoderBitrateMode mode);

    enum class EncoderProfile : int16_t
    {
        Baseline,
        Main,
        High
    };

    static std::unordered_map<EncoderProfile, std::string> const encoderProfileDescMap
    {
        { EncoderProfile::Baseline, "Baseline" },
        { EncoderProfile::Main, "Main" },
        { EncoderProfile::High, "High" }
    };

    static std::unordered_map<EncoderProfile, std::string> const encoderProfileStrMap
    {
        { EncoderProfile::Baseline, "baseline" },
        { EncoderProfile::Main, "main" },
        { EncoderProfile::High, "high" }
    };

    EncoderProfile encoderProfileFromStr(std::string s);
    std::string encoderProfileToStr(EncoderProfile profile);
    std::string encoderProfileToDesc(EncoderProfile profile);

    struct H264Data
    {
        EncoderPreset       preset;
        int32_t             constantRateFactor;
        EncoderBitrateMode  bitrateMode;
        EncoderProfile      profile;
    };

    auto constexpr h264DataSize = sizeof(H264Data);

    std::unordered_map<H264Parameters, std::string> codecParamsToStr(H264Data const data);
    H264Data codecParamsFromStr(std::unordered_map<H264Parameters, std::string> const& map);
}

#endif
