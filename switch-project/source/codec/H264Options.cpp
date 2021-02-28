#include "H264Options.h"
#include "../dataHelpers/EnumStrTemplate.h"

namespace h264
{
    int32_t ConstantRateFactorStrToInt(std::string s)
    {
        if(s.empty())
            return DefaultCRF;
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
                return val + " Lossless Quality";
            else if(crf < 12)
                return val + " Good Quality";
            else if(crf < 23)
                return val + " Average Quality";
            else
                return val + " Bad Quality";
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

}