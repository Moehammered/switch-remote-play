#include "h264AmfEnums.h"
#include "../../utils/EnumMapper.h"
#include <unordered_map>

namespace h264amf
{
    std::string amfUsageToStr(H264AmfUsage usage)
    {
        return enumToStr(usageMap, usage);
    }

    std::string amfUsageToDescription(H264AmfUsage usage)
    {
        auto pair = usageDescriptions.find(usage);
        if(pair != usageDescriptions.end())
            return pair->second;
        else
            return enumToStr(usageMap, usage) + " - no description";
    }

    H264AmfUsage amfUsageFromStr(std::string s)
    {
        return strToEnum(usageMap, s);
    }

    std::string amfProfileToStr(H264AmfProfile prof)
    {
        return enumToStr(profileMap, prof);
    }

    H264AmfProfile amfProfileFromStr(std::string s)
    {
        return strToEnum(profileMap, s);
    }

    std::string amfQualityToStr(H264AmfQuality qual)
    {
        return enumToStr(qualityMap, qual);
    }

    H264AmfQuality amfQualityFromStr(std::string s)
    {
        return strToEnum(qualityMap, s);
    }

    std::string amfRateControlToStr(H264AmfRateControl rc)
    {
        return enumToStr(rateControlMap, rc);
    }

    H264AmfRateControl amfRateControlFromStr(std::string s)
    {
        return strToEnum(rateControlMap, s);
    }
}