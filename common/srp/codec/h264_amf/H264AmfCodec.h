#ifndef __H264AMFCODEC_H__
#define __H264AMFCODEC_H__

#include "h264AmfEnums.h"
#include "h264AmfOptions.h"
#include "../../utils/ArrayCirculator.h"
#include "../../utils/UnorderedMapCirculator.h"
#include "../../utils/RangedIterator.h"
#include <functional>

class H264AmfCodec
{
public:
	H264AmfCodec();

	void Set(h264amf::H264AmfData const data);

	h264amf::H264AmfParameters Current();
	h264amf::H264AmfParameters Next();
	h264amf::H264AmfParameters Prev();

	h264amf::H264AmfData Data() const;

	void Increase(std::function<int(h264amf::H264AmfParameters,int)> const onModify);
	void Decrease(std::function<int(h264amf::H264AmfParameters,int)> const onModify);

private:
	// selection cursor
	ArrayCirculator<h264amf::H264AmfParameters, std::vector> cursor;

	// enum parameter cursors
	UnorderedMapCirculator<h264amf::H264AmfUsage, std::string> usageCursor;
	UnorderedMapCirculator<h264amf::H264AmfProfile, std::string> profileCursor;
	UnorderedMapCirculator<h264amf::H264AmfQuality, std::string> qualityCursor;
	UnorderedMapCirculator<h264amf::H264AmfRateControl, std::string> rateControlCursor;

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

	void ShiftParam(int direction, std::function<int(h264amf::H264AmfParameters,int)> const onModify);
};

#endif

