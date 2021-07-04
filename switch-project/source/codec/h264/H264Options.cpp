#include "H264Options.h"
#include "../../utils/EnumMapper.h"

namespace h264
{
    int32_t ConstantRateFactorStrToInt(std::string s)
    {
        if(s.empty())
            return DefaultCRF;
        else
        {
            auto casted = std::stoi(s);
            if(casted < MinCRF || casted > MaxCRF)
                return DefaultCRF;
            else
                return casted;
        }
    }

    std::string ConstantRateFactorIntToStr(int32_t crf)
    {
        if(crf < MinCRF || crf > MaxCRF)
            return std::to_string(DefaultCRF);
        else
            return std::to_string(crf);
    }

    std::string ConstantRateFactorToDesc(int32_t crf)
    {
        if(crf < MinCRF || crf > MaxCRF)
            return ConstantRateFactorToDesc(DefaultCRF);
        else
        {
            auto val = ConstantRateFactorIntToStr(crf);
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

    EncoderPreset EncoderPresetStrToEnum(std::string s)
    {
        return strToEnum(EncoderPresetStrMap, s);
    }

    std::string EncoderPresetToStr(EncoderPreset preset)
    {
        return enumToStr(EncoderPresetStrMap, preset);
    }

    std::string EncoderPresetToDesc(EncoderPreset preset)
    {
        return enumToStr(EncoderPresetDescMap, preset);
    }

    EncoderBitrateMode EncoderBitrateModeStrToEnum(std::string s)
    {
        return strToEnum(EncoderBitrateModeStrMap, s);
    }

    std::string EncoderBitrateModeToStr(EncoderBitrateMode mode)
    {
        return enumToStr(EncoderBitrateModeStrMap, mode);
    }

    std::string EncoderBitrateModeToDesc(EncoderBitrateMode mode)
    {
        return enumToStr(EncoderBitrateModeDescMap, mode);
    }

    EncoderProfile EncoderProfileStrToEnum(std::string s)
    {
        return strToEnum(EncoderProfileStrMap, s);
    }

    std::string EncoderProfileToStr(EncoderProfile profile)
    {
        return enumToStr(EncoderProfileStrMap, profile);
    }

    std::string EncoderProfileToDesc(EncoderProfile profile)
    {
        return enumToStr(EncoderProfileDescMap, profile);
    }
    
    std::unordered_map<Parameters, std::string> CodecParamsToStr(H264Data const data)
    {
        auto values = std::unordered_map<Parameters, std::string>{};

        values[Parameters::ConstantRateFactor] = ConstantRateFactorIntToStr(data.ConstantRateFactor);
        values[Parameters::Preset] = EncoderPresetToStr(data.Preset);
        values[Parameters::RateControlMode] = EncoderBitrateModeToStr(data.BitrateMode);
        values[Parameters::Profile] = EncoderProfileToStr(data.Profile);

        return values;
    }

    H264Data CodecParamsFromStr(std::unordered_map<Parameters, std::string> const& map)
    {
        auto data = H264Data();

        auto parse = [&](Parameters p, auto& member, auto def, auto strToEnum)
        {
            auto itr = map.find(p);
            if (itr != map.end())
                member = strToEnum(itr->second);
            else
                member = def;
        };

        parse(Parameters::ConstantRateFactor, data.ConstantRateFactor, DefaultCRF, ConstantRateFactorStrToInt);
        parse(Parameters::Preset, data.Preset, EncoderPreset::UltraFast, EncoderPresetStrToEnum);
        parse(Parameters::RateControlMode, data.BitrateMode, EncoderBitrateMode::VariableBitrate, EncoderBitrateModeStrToEnum);
        parse(Parameters::Profile, data.Profile, EncoderProfile::Main, EncoderProfileStrToEnum);

        return data;
    }
}