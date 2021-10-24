#ifndef __H264CODEC_H__
#define __H264CODEC_H__

#include "../../utils/ArrayCirculator.h"
#include "../../utils/RangedIterator.h"
#include "../../utils/UnorderedMapCirculator.h"
#include "H264Options.h"
#include <vector>
#include <string>
#include <functional>

class H264Codec
{
public:
	H264Codec();

	void Set(h264::H264Data const data);
	h264::H264Data const Data() const;

	h264::H264Parameters Current();
	h264::H264Parameters Next();
	h264::H264Parameters Prev();

	void Increase(std::function<int(h264::H264Parameters, int)> const onModify);
	void Decrease(std::function<int(h264::H264Parameters, int)> const onModify);

private:
	//selection cursor
	ArrayCirculator<h264::H264Parameters, std::vector> cursor;

	//enum cursors
	UnorderedMapCirculator<h264::EncoderPreset, std::string> presetCursor;
	UnorderedMapCirculator<h264::EncoderBitrateMode, std::string> rateModeCursor;
	UnorderedMapCirculator<h264::EncoderProfile, std::string> profileCursor;

	int32_t constantRateFactor;

	void ShiftParam(int direction, std::function<int(h264::H264Parameters, int)> const onModify);
};

#endif
