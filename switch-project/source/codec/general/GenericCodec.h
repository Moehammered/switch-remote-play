#ifndef __GENERICCODEC_H__
#define __GENERICCODEC_H__

#include "../../utils/ArrayCirculator.h"
#include "../../utils/RangedIterator.h"
#include "../../utils/UnorderedMapCirculator.h"
#include "GenericOptions.h"
#include <vector>
#include <string>

class GenericCodec
{
public:
    GenericCodec();

    void Set(VideoData const data);
	VideoData const Data() const;

	VideoParameters Current();
	VideoParameters Next();
	VideoParameters Prev();

	void Increase();
	void Decrease();

private:
	//selection cursor
	ArrayCirculator<VideoParameters, std::vector> cursor;

    //fixed value cursors
    ArrayCirculator<Resolution, std::vector> desktopResCursor;
    ArrayCirculator<Resolution, std::vector> switchResCursor;
    ArrayCirculator<int16_t, std::vector> fpsCursor;
	ArrayCirculator<int16_t, std::vector> bitrateCursor;

	//enum cursors
	UnorderedMapCirculator<VsyncMode, std::string> vsyncCursor;
	UnorderedMapCirculator<HWAccelMode, std::string> hwaccelCursor;
	UnorderedMapCirculator<VideoCodec, std::string> videoCursor;

	//number cursors
	NumberRange<int32_t, 0, 10> monitorNumberCursor;

	void ShiftParam(int direction);
};

#endif
