#include "H264AmfCodec.h"
#include "../../system/SoftwareKeyboard.h"

H264AmfCodec::H264AmfCodec()
	: cursor{ h264amf::h264AmfParamsList },
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

void H264AmfCodec::Set(h264amf::H264AmfData const data)
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

h264amf::H264AmfParameters H264AmfCodec::Current()
{
	return *cursor;
}

h264amf::H264AmfParameters H264AmfCodec::Next()
{
	++cursor;
	return *cursor;
}

h264amf::H264AmfParameters H264AmfCodec::Prev()
{
	--cursor;
	return *cursor;
}

h264amf::H264AmfData H264AmfCodec::Data() const
{
    auto data = h264amf::H264AmfData();

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
    case h264amf::H264AmfParameters::Usage:
        usageCursor += direction;
        break;
    case h264amf::H264AmfParameters::Profile:
        profileCursor += direction;
        break;
    case h264amf::H264AmfParameters::Quality:
        qualityCursor += direction;
        break;
    case h264amf::H264AmfParameters::RateControl:
        rateControlCursor += direction;
        break;
    // integer based params
    case h264amf::H264AmfParameters::Level:
        // level += direction;
        break;
    case h264amf::H264AmfParameters::FrameQuant_I:
        quantisationI = keyboardNumber(h264amf::qpFrameMin, h264amf::qpFrameMax, quantisationI);
        break;
    case h264amf::H264AmfParameters::FrameQuant_P:
        quantisationP = keyboardNumber(h264amf::qpFrameMin, h264amf::qpFrameMax, quantisationP);
        break;
    case h264amf::H264AmfParameters::FrameQuant_B:
        quantisationB = keyboardNumber(h264amf::qpFrameMin, h264amf::qpFrameMax, quantisationB);
        break;
    case h264amf::H264AmfParameters::FrameQuant_BDelta:
        // delta_qp_b += direction;
        break;
    case h264amf::H264AmfParameters::FrameQuant_BRefDelta:
        // delta_qp_b_ref += direction;
        break;
    // boolean based params
    case h264amf::H264AmfParameters::EnforceHRD:
        // enforceHRD = !enforceHRD;
        break;
    case h264amf::H264AmfParameters::FillerData:
        // fillerData = !fillerData;
        break;
    case h264amf::H264AmfParameters::VBAQ:
        // vbaq = !vbaq;
        break;
    case h264amf::H264AmfParameters::Frameskip:
        frameskip = !frameskip;
        break;
    case h264amf::H264AmfParameters::BFrameRef:
        // bFrameRef = !bFrameRef;
        break;
    case h264amf::H264AmfParameters::LogToDbg:
        // logToDebug = !logToDebug;
        break;
    default:
        break;
	}
}
