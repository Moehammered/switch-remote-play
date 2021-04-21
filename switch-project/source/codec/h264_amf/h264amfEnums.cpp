#include "h264amfEnums.h"
#include <unordered_map>
#include "../../utils/EnumMapper.h"

namespace h264amf
{
    std::string amfUsageToStr(H264AMF_USAGE usage)
    {
        return enumToStr(usageMap, usage);
    }

    std::string amfUsageToDescription(H264AMF_USAGE usage)
    {
        auto pair = usageDescriptions.find(usage);
        if(pair != usageDescriptions.end())
            return pair->second;
        else
            return enumToStr(usageMap, usage) + " - no description";
    }

    H264AMF_USAGE amfUsageStrToEnum(std::string s)
    {
        return strToEnum(usageMap, s);
    }

    std::string amfProfileToStr(H264AMF_PROFILE prof)
    {
        return enumToStr(profileMap, prof);
    }

    H264AMF_PROFILE amfProfileStrToEnum(std::string s)
    {
        return strToEnum(profileMap, s);
    }

    std::string amfQualityToStr(H264AMF_QUALITY qual)
    {
        return enumToStr(qualityMap, qual);
    }

    H264AMF_QUALITY amfQualityStrToEnum(std::string s)
    {
        return strToEnum(qualityMap, s);
    }

    std::string amfRateControlToStr(H264AMF_RATECONTROL rc)
    {
        return enumToStr(rateControlMap, rc);
    }

    H264AMF_RATECONTROL amfRateControlStrToEnum(std::string s)
    {
        return strToEnum(rateControlMap, s);
    }
}