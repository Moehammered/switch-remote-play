#ifndef __H264AMF_ENUMS_H__
#define __H264AMF_ENUMS_H__

#include <stdint.h>
#include <string>
#include <unordered_map>

namespace h264amf
{
    enum class H264AmfUsage : int16_t
    {
        Transcoding = 0,
        UltraLowLatency,
        LowLatency,
        Webcam
    };

    auto constexpr defaultUsage {H264AmfUsage::LowLatency};

    enum class H264AmfProfile : int16_t
    {
        Main = 66,
        High,
        ConstrainedBaseline,
        ConstrainedHigh
    };

    auto constexpr defaultProfile {H264AmfProfile::High};

    enum class H264AmfQuality : int16_t
    {
        Speed = 0,
        Balanced,
        Quality
    };

    auto constexpr defaultQuality {H264AmfQuality::Balanced};

    enum class H264AmfRateControl : int16_t
    {
        CQP = -1,
        CBR,
        VBR_Peak,
        VBR_Latency
    };

    auto constexpr defaultRateControl {H264AmfRateControl::CQP};

    static std::unordered_map<H264AmfUsage, std::string> usageMap
    {
        {H264AmfUsage::UltraLowLatency, "ultralowlatency"},
        {H264AmfUsage::Transcoding, "transcoding"},
        {H264AmfUsage::LowLatency, "lowlatency"},
        {H264AmfUsage::Webcam, "webcam"}
    };

    static std::unordered_map<H264AmfUsage, std::string> usageDescriptions
    {
        {H264AmfUsage::UltraLowLatency, "ultra low latency - prioritise latency at all costs"},
        {H264AmfUsage::Transcoding, "transcoding - output is suited for further encoding"},
        {H264AmfUsage::LowLatency, "low latency - prioritise latency while maintaining quality"},
        {H264AmfUsage::Webcam, "webcam - output is processed as a stream from a camera device"}
    };

    static std::unordered_map<H264AmfProfile, std::string> profileMap
    {
        {H264AmfProfile::Main, "main"},
        {H264AmfProfile::High, "high"},
        {H264AmfProfile::ConstrainedBaseline, "constrained_baseline"},
        {H264AmfProfile::ConstrainedHigh, "constrained_high"}
    };

    static std::unordered_map<H264AmfQuality, std::string> qualityMap
    {
        {H264AmfQuality::Speed, "speed"},
        {H264AmfQuality::Balanced, "balanced"},
        {H264AmfQuality::Quality, "quality"},
    };

    static std::unordered_map<H264AmfRateControl, std::string> rateControlMap
    {
        {H264AmfRateControl::CQP, "constant_quantization_parameter"},
        {H264AmfRateControl::CBR, "constant_bitrate"},
        {H264AmfRateControl::VBR_Peak, "peak_constrained_variable_bitrate"},
        {H264AmfRateControl::VBR_Latency, "latency_constrained_variable_bitrate"}
    };

    std::string amfUsageToStr(H264AmfUsage usage);
    std::string amfUsageToDescription(H264AmfUsage usage);
    H264AmfUsage amfUsageFromStr(std::string s);

    std::string amfProfileToStr(H264AmfProfile prof);
    H264AmfProfile amfProfileFromStr(std::string s);

    std::string amfQualityToStr(H264AmfQuality qual);
    H264AmfQuality amfQualityFromStr(std::string s);

    std::string amfRateControlToStr(H264AmfRateControl rc);
    H264AmfRateControl amfRateControlFromStr(std::string s);
}
#endif
