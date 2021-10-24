#include "h264AmfOptions.h"

namespace h264amf
{
    bool operator==(H264AmfData const a, H264AmfData const b)
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

    std::unordered_map<H264AmfParameters, std::string> codecParamsToStr(H264AmfData const data)
    {
        auto values = std::unordered_map<H264AmfParameters, std::string>{};

        auto boolToStr = [](bool a) {
            return a ? std::string{ "yes" } : std::string{"no"};
        };

        values[H264AmfParameters::Usage] = amfUsageToStr(data.usage);
        values[H264AmfParameters::Profile] = amfProfileToStr(data.profile);
        // values[Parameters::Level] = amfLevelToStr(data.level);
        values[H264AmfParameters::Quality] = amfQualityToStr(data.quality);
        values[H264AmfParameters::RateControl] = amfRateControlToStr(data.rateControl);
        values[H264AmfParameters::FrameQuant_I] = amfFrameQPToStr(data.qp_i);
        values[H264AmfParameters::FrameQuant_P] = amfFrameQPToStr(data.qp_p);
        values[H264AmfParameters::FrameQuant_B] = amfFrameQPToStr(data.qp_b);
        // values[Parameters::FrameQuant_BDelta] = amfBFrameDeltaQPToStr(data.qp_bfDelta);
        // values[Parameters::FrameQuant_BRefDelta] = amfBFrameDeltaQPToStr(data.qp_bfRefDelta);
        // values[Parameters::EnforceHRD] = boolToStr(data.enforceHRD);
        // values[Parameters::FillerData] = boolToStr(data.fillerData);
        // values[Parameters::VBAQ] = boolToStr(data.vbaq);
        values[H264AmfParameters::Frameskip] = boolToStr(data.frameskip);
        // values[Parameters::BFrameRef] = boolToStr(data.bfRef);
        // values[Parameters::LogToDbg] = boolToStr(data.logToDbg);

        return values;
    }

    H264AmfData codecParamsFromStr(std::unordered_map<H264AmfParameters, std::string> const& map)
    {
        auto data = H264AmfData();

        auto parse = [&](H264AmfParameters p, auto& member, auto def, auto strToEnum)
        {
            auto itr = map.find(p);
            if (itr != map.end())
                member = strToEnum(itr->second);
            else
                member = def;
        };

        parse(H264AmfParameters::Usage, data.usage, H264AmfUsage::UltraLowLatency, amfUsageFromStr);
        parse(H264AmfParameters::Profile, data.profile, H264AmfProfile::ConstrainedBaseline, amfProfileFromStr);
        // parse(Parameters::Level, data.level, levelDefault, amfLevelStrToInt);
        parse(H264AmfParameters::Quality, data.quality, H264AmfQuality::Speed, amfQualityFromStr);
        parse(H264AmfParameters::RateControl, data.rateControl, H264AmfRateControl::CBR, amfRateControlFromStr);
        parse(H264AmfParameters::FrameQuant_I, data.qp_i, qpFrameDefault, amfFrameQPStrToInt);
        parse(H264AmfParameters::FrameQuant_P, data.qp_p, qpFrameQDefault, amfFrameQPStrToInt);
        parse(H264AmfParameters::FrameQuant_B, data.qp_b, qpFrameDefault, amfFrameQPStrToInt);
        // parse(Parameters::FrameQuant_BDelta, data.qp_bfDelta, frameDeltaDefault, amfBFrameDeltaQPStrToInt);
        // parse(Parameters::FrameQuant_BRefDelta, data.qp_bfRefDelta, frameDeltaDefault, amfBFrameDeltaQPStrToInt);
        
        auto strToBool = [](std::string a) {
            return a == "yes" ? true : false;
        };

        // parse(Parameters::EnforceHRD, data.enforceHRD, false, strToBool);
        // parse(Parameters::FillerData, data.fillerData, false, strToBool);
        // parse(Parameters::VBAQ, data.vbaq, false, strToBool);
        parse(H264AmfParameters::Frameskip, data.frameskip, false, strToBool);
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
