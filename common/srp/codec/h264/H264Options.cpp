#include "H264Options.h"
#include "../../utils/EnumMapper.h"

namespace h264
{
    int32_t constantRateFactorFromStr(std::string s)
    {
        if(s.empty())
            return defaultCRF;
        else
        {
            auto casted = std::stoi(s);
            if(casted < minCRF || casted > maxCRF)
                return defaultCRF;
            else
                return casted;
        }
    }

    std::string constantRateFactorToStr(int32_t crf)
    {
        if(crf < minCRF || crf > maxCRF)
            return std::to_string(defaultCRF);
        else
            return std::to_string(crf);
    }

    std::string constantRateFactorToDesc(int32_t crf)
    {
        if(crf < minCRF || crf > maxCRF)
            return constantRateFactorToDesc(defaultCRF);
        else
        {
            auto val = constantRateFactorToStr(crf);
            if(crf == 0)
                return val + " (Lossless Quality)";
            else if(crf < 18)
                return val + " (Good Quality)";
            else if(crf < 24)
                return val + " (Average Quality)";
            else
                return val + " (Bad Quality)";
        }
    }

    EncoderPreset encoderPresetFromStr(std::string s)
    {
        return strToEnum(encoderPresetStrMap, s);
    }

    std::string encoderPresetToStr(EncoderPreset preset)
    {
        return enumToStr(encoderPresetStrMap, preset);
    }

    std::string encoderPresetToDesc(EncoderPreset preset)
    {
        return enumToStr(encoderPresetDescMap, preset);
    }

    EncoderBitrateMode encoderBitrateModeFromStr(std::string s)
    {
        return strToEnum(encoderBitrateModeStrMap, s);
    }

    std::string encoderBitrateModeToStr(EncoderBitrateMode mode)
    {
        return enumToStr(encoderBitrateModeStrMap, mode);
    }

    std::string encoderBitrateModeToDesc(EncoderBitrateMode mode)
    {
        return enumToStr(encoderBitrateModeDescMap, mode);
    }

    EncoderProfile encoderProfileFromStr(std::string s)
    {
        return strToEnum(encoderProfileStrMap, s);
    }

    std::string encoderProfileToStr(EncoderProfile profile)
    {
        return enumToStr(encoderProfileStrMap, profile);
    }

    std::string encoderProfileToDesc(EncoderProfile profile)
    {
        return enumToStr(encoderProfileDescMap, profile);
    }
    
    std::unordered_map<H264Parameters, std::string> codecParamsToStr(H264Data const data)
    {
        auto values = std::unordered_map<H264Parameters, std::string>{};

        values[H264Parameters::ConstantRateFactor] = constantRateFactorToStr(data.constantRateFactor);
        values[H264Parameters::Preset] = encoderPresetToStr(data.preset);
        values[H264Parameters::RateControlMode] = encoderBitrateModeToStr(data.bitrateMode);
        values[H264Parameters::Profile] = encoderProfileToStr(data.profile);

        return values;
    }

    H264Data codecParamsFromStr(std::unordered_map<H264Parameters, std::string> const& map)
    {
        auto data = H264Data();

        auto parse = [&](H264Parameters p, auto& member, auto def, auto strToEnum)
        {
            auto itr = map.find(p);
            if (itr != map.end())
                member = strToEnum(itr->second);
            else
                member = def;
        };

        parse(H264Parameters::ConstantRateFactor, data.constantRateFactor, defaultCRF, constantRateFactorFromStr);
        parse(H264Parameters::Preset, data.preset, EncoderPreset::UltraFast, encoderPresetFromStr);
        parse(H264Parameters::RateControlMode, data.bitrateMode, EncoderBitrateMode::VariableBitrate, encoderBitrateModeFromStr);
        parse(H264Parameters::Profile, data.profile, EncoderProfile::Main, encoderProfileFromStr);

        return data;
    }
}