#ifndef __H264CODEC_H__
#define __H264CODEC_H__

#include "../../utils/ArrayCirculator.h"
#include "../../utils/RangedIterator.h"
#include "../../utils/UnorderedMapCirculator.h"
#include "H264Options.h"
#include <vector>
#include <string>

class H264Codec
{
public:
	H264Codec();

	void Set(h264::H264Data const data);
	h264::H264Data const Data() const;

	h264::Parameters Current();
	h264::Parameters Next();
	h264::Parameters Prev();

	void Increase();
	void Decrease();

private:
	//selection cursor
	ArrayCirculator<h264::Parameters, std::vector> cursor;

	//enum cursors
	UnorderedMapCirculator<h264::EncoderPreset, std::string> presetCursor;

	//number range cursors
	NumberRange<int32_t, h264::MinCRF, h264::MaxCRF> crfCursor;

	void ShiftParam(int direction);
};

#endif
