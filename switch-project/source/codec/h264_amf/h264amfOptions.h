#ifndef __H264AMF_OPTIONS_H__
#define __H264AMF_OPTIONS_H__

#include "h264amfEnums.h"
#include <vector>
#include <unordered_map>

namespace h264amf
{
    enum class Parameters : int
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

    struct H264AMFData
    {
        H264AMF_USAGE       usage;
        H264AMF_PROFILE     profile;
        int16_t             level;
        H264AMF_QUALITY     quality;
        H264AMF_RATECONTROL rateControl;
        int16_t             qp_i; //iframe quantization
        int16_t             qp_p; //pframe quantization
        int16_t             qp_b; //bframe quantization
        int16_t             qp_bfDelta;
        int16_t             qp_bfRefDelta;
        bool                enforceHRD;
        bool                fillerData;
        bool                vbaq;
        bool                frameskip;
        bool                bfRef;
        bool                logToDbg;
    };

    bool operator==(H264AMFData const a, H264AMFData const b);

    auto constexpr H264AMFDataSize = sizeof(H264AMFData);

    std::unordered_map<Parameters, std::string> CodecParamsToStr(H264AMFData const data);
    H264AMFData CodecParamsFromStr(std::unordered_map<Parameters, std::string> const& map);

    std::string amfLevelToStr(int16_t level);
    int16_t amfLevelStrToInt(std::string s);

    std::string amfFrameQPToStr(int16_t qp);
    int16_t amfFrameQPStrToInt(std::string s);

    std::string amfBFrameDeltaQPToStr(int16_t qp);
    int16_t amfBFrameDeltaQPStrToInt(std::string s);

    static std::vector<Parameters> const ParamsList
    {
        Parameters::Usage, Parameters::Profile, Parameters::Level, Parameters::Quality,
        Parameters::RateControl, Parameters::FrameQuant_I, Parameters::FrameQuant_P,
        Parameters::FrameQuant_B, Parameters::FrameQuant_BDelta, Parameters::FrameQuant_BRefDelta,
        Parameters::EnforceHRD, Parameters::FillerData, Parameters::VBAQ, Parameters::Frameskip,
        Parameters::BFrameRef, Parameters::LogToDbg  
    };

    static std::unordered_map<Parameters, std::string> const ParamsDesc
    {
        {Parameters::Usage, "Usage Hint"}, 
        {Parameters::Profile, "Profile"},
        {Parameters::Level, "Profile Level"},
        {Parameters::Quality, "Quality Hint"},
        {Parameters::RateControl, "Rate Control Method"},
        {Parameters::FrameQuant_I, "I-Frame Rate"},
        {Parameters::FrameQuant_P, "P-Frame Rate"},
        {Parameters::FrameQuant_B, "B-Frame Rate"},
        {Parameters::FrameQuant_BDelta, "B-Frame Delta Rate"},
        {Parameters::FrameQuant_BRefDelta, "B-Frame Reference Delta Rate"},
        {Parameters::EnforceHRD, "Enforce HRD"},
        {Parameters::FillerData, "Filler Data"},
        {Parameters::VBAQ, "VBAQ"},
        {Parameters::Frameskip, "Frameskip"},
        {Parameters::BFrameRef, "F-Frame Ref Delta"},
        {Parameters::LogToDbg, "Log to Debug"}
    };

    auto constexpr levelMin = 0;
    auto constexpr levelDefault = 0;
    auto constexpr levelMax = 62;
    auto constexpr qpFrameMin = -1;
    auto constexpr qpFrameDefault = 15;
    auto constexpr qpFrameMax = 51;
    auto constexpr frameDeltaMin = -10;
    auto constexpr frameDeltaDefault = 4;
    auto constexpr frameDeltaMax = 10;
}

#endif
