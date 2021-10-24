#ifndef __GENERICCODEC_H__
#define __GENERICCODEC_H__

#include "../../utils/ArrayCirculator.h"
#include "../../utils/RangedIterator.h"
#include "../../utils/UnorderedMapCirculator.h"
#include "GenericOptions.h"
#include <vector>
#include <string>
#include <functional>

class GenericCodec
{
public:
    GenericCodec();

    void Set(codec::VideoData const data);
	codec::VideoData const Data() const;

	codec::VideoParameters Current();
	codec::VideoParameters Next();
	codec::VideoParameters Prev();

	void Increase(std::function<int(codec::VideoParameters,int)> const onModify);
	void Decrease(std::function<int(codec::VideoParameters,int)> const onModify);

private:
	//selection cursor
	ArrayCirculator<codec::VideoParameters, std::vector> cursor;

    //fixed value cursors
    ArrayCirculator<Resolution, std::vector> desktopResCursor;
    ArrayCirculator<Resolution, std::vector> switchResCursor;
	ArrayCirculator<int16_t, std::vector> bitrateCursor;

	//enum cursors
	UnorderedMapCirculator<ffmpeg::VsyncMode, std::string> vsyncCursor;
	UnorderedMapCirculator<ffmpeg::HWAccelMode, std::string> hwaccelCursor;
	UnorderedMapCirculator<ffmpeg::VideoCodec, std::string> videoCursor;

	int16_t desiredFramerate;
	int16_t monitorNumber;

	void ShiftParam(int direction, std::function<int(codec::VideoParameters,int)> const onModify);
};

#endif
