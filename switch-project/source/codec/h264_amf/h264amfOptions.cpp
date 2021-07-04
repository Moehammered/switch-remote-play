#include "h264amfOptions.h"

namespace h264amf
{
    bool operator==(H264AMFData const a, H264AMFData const b)
    {
        return a.usage == b.usage
            && a.profile == b.profile
            // && a.level == b.level
            && a.quality == b.quality
            && a.rateControl == b.rateControl
            && a.qp_i == b.qp_i
            && a.qp_p == b.qp_p
            && a.qp_b == b.qp_b
            // && a.qp_bfDelta == b.qp_bfDelta
            // && a.qp_bfRefDelta == b.qp_bfRefDelta
            // && a.enforceHRD == b.enforceHRD
            // && a.fillerData == b.fillerData
            // && a.vbaq == b.vbaq
            && a.frameskip == b.frameskip;
            // && a.bfRef == b.bfRef
            // && a.logToDbg == b.logToDbg;
    }

    std::unordered_map<Parameters, std::string> CodecParamsToStr(H264AMFData const data)
    {
        auto values = std::unordered_map<Parameters, std::string>{};

        auto boolToStr = [](bool a) {
            return a ? std::string{ "yes" } : std::string{"no"};
        };

        values[Parameters::Usage] = amfUsageToStr(data.usage);
        values[Parameters::Profile] = amfProfileToStr(data.profile);
        // values[Parameters::Level] = amfLevelToStr(data.level);
        values[Parameters::Quality] = amfQualityToStr(data.quality);
        values[Parameters::RateControl] = amfRateControlToStr(data.rateControl);
        values[Parameters::FrameQuant_I] = amfFrameQPToStr(data.qp_i);
        values[Parameters::FrameQuant_P] = amfFrameQPToStr(data.qp_p);
        values[Parameters::FrameQuant_B] = amfFrameQPToStr(data.qp_b);
        // values[Parameters::FrameQuant_BDelta] = amfBFrameDeltaQPToStr(data.qp_bfDelta);
        // values[Parameters::FrameQuant_BRefDelta] = amfBFrameDeltaQPToStr(data.qp_bfRefDelta);
        // values[Parameters::EnforceHRD] = boolToStr(data.enforceHRD);
        // values[Parameters::FillerData] = boolToStr(data.fillerData);
        // values[Parameters::VBAQ] = boolToStr(data.vbaq);
        values[Parameters::Frameskip] = boolToStr(data.frameskip);
        // values[Parameters::BFrameRef] = boolToStr(data.bfRef);
        // values[Parameters::LogToDbg] = boolToStr(data.logToDbg);

        return values;
    }

    H264AMFData CodecParamsFromStr(std::unordered_map<Parameters, std::string> const& map)
    {
        auto data = H264AMFData();

        auto parse = [&](Parameters p, auto& member, auto def, auto strToEnum)
        {
            auto itr = map.find(p);
            if (itr != map.end())
                member = strToEnum(itr->second);
            else
                member = def;
        };

        parse(Parameters::Usage, data.usage, H264AMF_USAGE::ULTRALOWLATENCY, amfUsageStrToEnum);
        parse(Parameters::Profile, data.profile, H264AMF_PROFILE::CONSTRAINED_BASELINE, amfProfileStrToEnum);
        // parse(Parameters::Level, data.level, levelDefault, amfLevelStrToInt);
        parse(Parameters::Quality, data.quality, H264AMF_QUALITY::SPEED, amfQualityStrToEnum);
        parse(Parameters::RateControl, data.rateControl, H264AMF_RATECONTROL::CBR, amfRateControlStrToEnum);
        parse(Parameters::FrameQuant_I, data.qp_i, qpFrameDefault, amfFrameQPStrToInt);
        parse(Parameters::FrameQuant_P, data.qp_p, qpFrameQDefault, amfFrameQPStrToInt);
        parse(Parameters::FrameQuant_B, data.qp_b, qpFrameDefault, amfFrameQPStrToInt);
        // parse(Parameters::FrameQuant_BDelta, data.qp_bfDelta, frameDeltaDefault, amfBFrameDeltaQPStrToInt);
        // parse(Parameters::FrameQuant_BRefDelta, data.qp_bfRefDelta, frameDeltaDefault, amfBFrameDeltaQPStrToInt);
        
        auto strToBool = [](std::string a) {
            return a == "yes" ? true : false;
        };

        // parse(Parameters::EnforceHRD, data.enforceHRD, false, strToBool);
        // parse(Parameters::FillerData, data.fillerData, false, strToBool);
        // parse(Parameters::VBAQ, data.vbaq, false, strToBool);
        parse(Parameters::Frameskip, data.frameskip, false, strToBool);
        // parse(Parameters::BFrameRef, data.bfRef, false, strToBool);
        // parse(Parameters::LogToDbg, data.logToDbg, false, strToBool);
        
        return data;
    }

    std::string amfLevelToStr(int16_t level)
    {
        if(level <= levelMin || level > levelMax)
            return "auto";
        else
            return std::to_string(level);
    }

    int16_t amfLevelStrToInt(std::string s)
    {
        if(s == "auto")
            return levelMin;
        else
        {   
            auto casted = std::stoi(s);
            if(casted < levelMin || casted > levelMax)
                return levelMin;
            else
                return casted;
        }
    }

    std::string amfFrameQPToStr(int16_t qp)
    {
        if(qp < qpFrameMin || qp > qpFrameMax)
            return std::to_string(qpFrameMin);
        else
            return std::to_string(qp);
    }

    int16_t amfFrameQPStrToInt(std::string s)
    {
        auto casted = std::stoi(s);
        if(casted < qpFrameMin || casted > qpFrameMax)
            return qpFrameMin;
        else
            return casted;
    }

    std::string amfBFrameDeltaQPToStr(int16_t qp)
    {
        if(qp < frameDeltaMin || qp > frameDeltaMax)
            return std::to_string(frameDeltaDefault);
        else
            return std::to_string(qp);
    }

    int16_t amfBFrameDeltaQPStrToInt(std::string s)
    {
        auto casted = std::stoi(s);
        if(casted < frameDeltaMin || casted > frameDeltaMax)
            return frameDeltaDefault;
        else
            return casted;
    }
}
