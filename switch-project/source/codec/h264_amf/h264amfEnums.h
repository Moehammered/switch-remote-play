#ifndef __H264AMF_ENUMS_H__
#define __H264AMF_ENUMS_H__

#include <stdint.h>
#include <string>
#include <unordered_map>

namespace h264amf
{
    enum class H264AMF_USAGE : int16_t
    {
        TRANSCODING = 0,
        ULTRALOWLATENCY,
        LOWLATENCY,
        WEBCAM
    };

    auto constexpr DefaultUsage {H264AMF_USAGE::LOWLATENCY};

    enum class H264AMF_PROFILE : int16_t
    {
        MAIN = 66,
        HIGH,
        CONSTRAINED_BASELINE,
        CONSTRAINED_HIGH
    };

    auto constexpr DefaultProfile {H264AMF_PROFILE::HIGH};

    enum class H264AMF_QUALITY : int16_t
    {
        SPEED = 0,
        BALANCED,
        QUALITY
    };

    auto constexpr DefaultQuality {H264AMF_QUALITY::BALANCED};

    enum class H264AMF_RATECONTROL : int16_t
    {
        CQP = -1,
        CBR,
        VBR_PEAK,
        VBR_LATENCY
    };

    auto constexpr DefaultRateControl {H264AMF_RATECONTROL::CQP};

    static std::unordered_map<H264AMF_USAGE, std::string> usageMap
    {
        {H264AMF_USAGE::ULTRALOWLATENCY, "ultralowlatency"},
        {H264AMF_USAGE::TRANSCODING, "transcoding"},
        {H264AMF_USAGE::LOWLATENCY, "lowlatency"},
        {H264AMF_USAGE::WEBCAM, "webcam"}
    };

    static std::unordered_map<H264AMF_USAGE, std::string> usageDescriptions
    {
        {H264AMF_USAGE::ULTRALOWLATENCY, "ultra low latency - prioritise latency at all costs"},
        {H264AMF_USAGE::TRANSCODING, "transcoding - output is suited for further encoding"},
        {H264AMF_USAGE::LOWLATENCY, "low latency - prioritise latency while maintaining quality"},
        {H264AMF_USAGE::WEBCAM, "webcam - output is processed as a stream from a camera device"}
    };

    static std::unordered_map<H264AMF_PROFILE, std::string> profileMap
    {
        {H264AMF_PROFILE::MAIN, "main"},
        {H264AMF_PROFILE::HIGH, "high"},
        {H264AMF_PROFILE::CONSTRAINED_BASELINE, "constrained_baseline"},
        {H264AMF_PROFILE::CONSTRAINED_HIGH, "constrained_high"}
    };

    static std::unordered_map<H264AMF_QUALITY, std::string> qualityMap
    {
        {H264AMF_QUALITY::SPEED, "speed"},
        {H264AMF_QUALITY::BALANCED, "balanced"},
        {H264AMF_QUALITY::QUALITY, "quality"},
    };

    static std::unordered_map<H264AMF_RATECONTROL, std::string> rateControlMap
    {
        {H264AMF_RATECONTROL::CQP, "constant_quantization_parameter"},
        {H264AMF_RATECONTROL::CBR, "constant_bitrate"},
        {H264AMF_RATECONTROL::VBR_PEAK, "peak_constrained_variable_bitrate"},
        {H264AMF_RATECONTROL::VBR_LATENCY, "latency_constrained_variable_bitrate"}
    };

    std::string amfUsageToStr(H264AMF_USAGE usage);
    std::string amfUsageToDescription(H264AMF_USAGE usage);
    H264AMF_USAGE amfUsageStrToEnum(std::string s);

    std::string amfProfileToStr(H264AMF_PROFILE prof);
    H264AMF_PROFILE amfProfileStrToEnum(std::string s);

    std::string amfQualityToStr(H264AMF_QUALITY qual);
    H264AMF_QUALITY amfQualityStrToEnum(std::string s);

    std::string amfRateControlToStr(H264AMF_RATECONTROL rc);
    H264AMF_RATECONTROL amfRateControlStrToEnum(std::string s);
}
#endif
