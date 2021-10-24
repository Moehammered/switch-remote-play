#ifndef __H264AMF_OPTIONS_H__
#define __H264AMF_OPTIONS_H__

#include "h264AmfEnums.h"
#include <vector>
#include <unordered_map>

namespace h264amf
{
    enum class H264AmfParameters : int
    {
        Usage,
        Profile,
        Level,
        Quality,
        RateControl,
        FrameQuant_I,
        FrameQuant_P,
        FrameQuant_B,
        FrameQuant_BDelta,
        FrameQuant_BRefDelta,
        EnforceHRD,
        FillerData,
        VBAQ,
        Frameskip,
        BFrameRef,
        LogToDbg
    };

    struct H264AmfData
    {
        H264AmfUsage       usage;
        H264AmfProfile     profile;
        //int16_t             level;
        H264AmfQuality     quality;
        H264AmfRateControl rateControl;
        int16_t             qp_i; //iframe quantization
        int16_t             qp_p; //pframe quantization
        int16_t             qp_b; //bframe quantization
        //int16_t             qp_bfDelta;
        //int16_t             qp_bfRefDelta;
        //bool                enforceHRD;
        //bool                fillerData;
        //bool                vbaq;
        bool                frameskip;
        //bool                bfRef;
        //bool                logToDbg;
    };

    bool operator==(H264AmfData const a, H264AmfData const b);

    auto constexpr h264AmfDataSize = sizeof(H264AmfData);

    std::unordered_map<H264AmfParameters, std::string> codecParamsToStr(H264AmfData const data);
    H264AmfData codecParamsFromStr(std::unordered_map<H264AmfParameters, std::string> const& map);

    std::string amfLevelToStr(int16_t level);
    int16_t amfLevelStrToInt(std::string s);

    std::string amfFrameQPToStr(int16_t qp);
    int16_t amfFrameQPStrToInt(std::string s);

    std::string amfBFrameDeltaQPToStr(int16_t qp);
    int16_t amfBFrameDeltaQPStrToInt(std::string s);

    static std::vector<H264AmfParameters> const h264AmfParamsList
    {
        H264AmfParameters::Usage, H264AmfParameters::Profile, H264AmfParameters::Quality,
        H264AmfParameters::RateControl, H264AmfParameters::FrameQuant_I, H264AmfParameters::FrameQuant_P,
        H264AmfParameters::FrameQuant_B, H264AmfParameters::Frameskip
    };

    static std::unordered_map<H264AmfParameters, std::string> const h264AmfParamsDesc
    {
        {H264AmfParameters::Usage, "Usage Hint"}, 
        {H264AmfParameters::Profile, "Profile"},
        // {Parameters::Level, "Profile Level"},
        {H264AmfParameters::Quality, "Quality Hint"},
        {H264AmfParameters::RateControl, "Rate Control Method"},
        {H264AmfParameters::FrameQuant_I, "I-Frame Control Factor"},
        {H264AmfParameters::FrameQuant_P, "P-Frame Control Factor"},
        {H264AmfParameters::FrameQuant_B, "B-Frame Control Factor"},
        // {Parameters::FrameQuant_BDelta, "B-Frame Delta Rate"},
        // {Parameters::FrameQuant_BRefDelta, "B-Frame Reference Delta Rate"},
        // {Parameters::EnforceHRD, "Enforce HRD"},
        // {Parameters::FillerData, "Filler Data"},
        // {Parameters::VBAQ, "VBAQ"},
        {H264AmfParameters::Frameskip, "Frameskip"},
        // {Parameters::BFrameRef, "B-Frame Ref Delta"},
        // {Parameters::LogToDbg, "Log to Debug"}
    };

    auto constexpr levelMin = 0;
    auto constexpr levelDefault = 0;
    auto constexpr levelMax = 62;
    auto constexpr qpFrameMin = -1;
    auto constexpr qpFrameDefault = 20;
    auto constexpr qpFrameQDefault = -1;
    auto constexpr qpFrameMax = 51;
    auto constexpr frameDeltaMin = -10;
    auto constexpr frameDeltaDefault = 4;
    auto constexpr frameDeltaMax = 10;
}

#endif
