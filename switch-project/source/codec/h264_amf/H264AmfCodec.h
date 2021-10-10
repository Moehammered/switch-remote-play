#ifndef __H264AMFCODEC_H__
#define __H264AMFCODEC_H__

#include "h264amfEnums.h"
#include "h264amfOptions.h"
#include "../../utils/ArrayCirculator.h"
#include "../../utils/UnorderedMapCirculator.h"
#include "../../utils/RangedIterator.h"

class H264AmfCodec
{
public:
	H264AmfCodec();

	void Set(h264amf::H264AMFData const data);

	h264amf::Parameters Current();
	h264amf::Parameters Next();
	h264amf::Parameters Prev();

	h264amf::H264AMFData Data() const;

	void Increase();
	void Decrease();

private:
	// selection cursor
	ArrayCirculator<h264amf::Parameters, std::vector> cursor;

	// enum parameter cursors
	UnorderedMapCirculator<h264amf::H264AMF_USAGE, std::string> usageCursor;
	UnorderedMapCirculator<h264amf::H264AMF_PROFILE, std::string> profileCursor;
	UnorderedMapCirculator<h264amf::H264AMF_QUALITY, std::string> qualityCursor;
	UnorderedMapCirculator<h264amf::H264AMF_RATECONTROL, std::string> rateControlCursor;

	// integer parameter 'cursors'
	// NumberRange<int, h264amf::levelMin, h264amf::levelMax> level;
	int32_t quantisationI;
	int32_t quantisationP;
	int32_t quantisationB;
	// NumberRange<int, h264amf::frameDeltaMin, h264amf::frameDeltaMax> delta_qp_b;
	// NumberRange<int, h264amf::frameDeltaMin, h264amf::frameDeltaMax> delta_qp_b_ref;

	// boolean parameters
	// bool enforceHRD;
	// bool fillerData;
	// bool vbaq;
	bool frameskip;
	// bool bFrameRef;
	// bool logToDebug;

	void ShiftParam(int direction);
};

#endif

