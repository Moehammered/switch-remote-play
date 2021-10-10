#include "H264AmfCodec.h"
#include "../../system/SoftwareKeyboard.h"

H264AmfCodec::H264AmfCodec()
	: cursor{ h264amf::ParamsList },
    usageCursor{ h264amf::usageMap },
    profileCursor{ h264amf::profileMap },
    qualityCursor{ h264amf::qualityMap },
    rateControlCursor{ h264amf::rateControlMap },
    quantisationI{h264amf::qpFrameDefault}, 
    quantisationP{h264amf::qpFrameDefault}, 
    quantisationB{h264amf::qpFrameDefault}, 
    frameskip{}
{
}

void H264AmfCodec::Set(h264amf::H264AMFData const data)
{
    cycleMap(usageCursor, data.usage);
    cycleMap(profileCursor, data.profile);
    cycleMap(qualityCursor, data.quality);
    cycleMap(rateControlCursor, data.rateControl);

    // cycleNumber(level, data.level);
    quantisationI = data.qp_i;
    quantisationP = data.qp_p;
    quantisationB = data.qp_b;
    // cycleNumber(delta_qp_b, data.qp_bfDelta);
    // cycleNumber(delta_qp_b_ref, data.qp_bfRefDelta);

    //check booleans here
    // enforceHRD = data.enforceHRD;
    // fillerData = data.fillerData;
    // vbaq = data.vbaq;
    frameskip = data.frameskip;
    // bFrameRef = data.bfRef;
    // logToDebug = data.logToDbg;
}

h264amf::Parameters H264AmfCodec::Current()
{
	return *cursor;
}

h264amf::Parameters H264AmfCodec::Next()
{
	++cursor;
	return *cursor;
}

h264amf::Parameters H264AmfCodec::Prev()
{
	--cursor;
	return *cursor;
}

h264amf::H264AMFData H264AmfCodec::Data() const
{
    auto data = h264amf::H264AMFData();

    data.usage = usageCursor.KeyPair().first;
    data.profile = profileCursor.KeyPair().first;
    // data.level = *level;
    data.quality = qualityCursor.KeyPair().first;
    data.rateControl = rateControlCursor.KeyPair().first;
    data.qp_i = quantisationI;
    data.qp_p = quantisationP;
    data.qp_b = quantisationB;
    // data.qp_bfDelta = *delta_qp_b;
    // data.qp_bfRefDelta = *delta_qp_b_ref;
    // data.enforceHRD = enforceHRD;
    // data.fillerData = fillerData;
    // data.vbaq = vbaq;
    data.frameskip = frameskip;
    // data.bfRef = bFrameRef;
    // data.logToDbg = logToDebug;

    return data;
}

void H264AmfCodec::Increase()
{
    ShiftParam(1);
}

void H264AmfCodec::Decrease()
{
    ShiftParam(-1);
}

void H264AmfCodec::ShiftParam(int direction)
{
	switch (*cursor)
	{
    // enum based params
    case h264amf::Parameters::Usage:
        usageCursor += direction;
        break;
    case h264amf::Parameters::Profile:
        profileCursor += direction;
        break;
    case h264amf::Parameters::Quality:
        qualityCursor += direction;
        break;
    case h264amf::Parameters::RateControl:
        rateControlCursor += direction;
        break;
    // integer based params
    case h264amf::Parameters::Level:
        // level += direction;
        break;
    case h264amf::Parameters::FrameQuant_I:
        quantisationI = KeyboardNumber(h264amf::qpFrameMin, h264amf::qpFrameMax, quantisationI);
        break;
    case h264amf::Parameters::FrameQuant_P:
        quantisationP = KeyboardNumber(h264amf::qpFrameMin, h264amf::qpFrameMax, quantisationP);
        break;
    case h264amf::Parameters::FrameQuant_B:
        quantisationB = KeyboardNumber(h264amf::qpFrameMin, h264amf::qpFrameMax, quantisationB);
        break;
    case h264amf::Parameters::FrameQuant_BDelta:
        // delta_qp_b += direction;
        break;
    case h264amf::Parameters::FrameQuant_BRefDelta:
        // delta_qp_b_ref += direction;
        break;
    // boolean based params
    case h264amf::Parameters::EnforceHRD:
        // enforceHRD = !enforceHRD;
        break;
    case h264amf::Parameters::FillerData:
        // fillerData = !fillerData;
        break;
    case h264amf::Parameters::VBAQ:
        // vbaq = !vbaq;
        break;
    case h264amf::Parameters::Frameskip:
        frameskip = !frameskip;
        break;
    case h264amf::Parameters::BFrameRef:
        // bFrameRef = !bFrameRef;
        break;
    case h264amf::Parameters::LogToDbg:
        // logToDebug = !logToDebug;
        break;
    default:
        break;
	}
}
