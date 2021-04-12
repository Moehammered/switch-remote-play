#include "H264Codec.h"

H264Codec::H264Codec()
	: cursor{ h264::ParamsList },
	presetCursor{ h264::EncoderPresetStrMap },
	crfCursor{ h264::DefaultCRF }
{
}

void H264Codec::Set(h264::H264Data const data)
{
	cycleMap(presetCursor, data.Preset);
	cycleNumber(crfCursor, data.ConstantRateFactor);
}

h264::H264Data const H264Codec::Data() const
{
	auto data = h264::H264Data{};

	data.Preset = presetCursor.KeyPair().first;
	data.ConstantRateFactor = *crfCursor;

	return data;
}

h264::Parameters H264Codec::Next()
{
	++cursor;
	return *cursor;
}

h264::Parameters H264Codec::Prev()
{
	--cursor;
	return *cursor;
}

void H264Codec::Increase()
{
	ShiftParam(1);
}

void H264Codec::Decrease()
{
	ShiftParam(-1);
}

void H264Codec::ShiftParam(int direction)
{
	switch (*cursor)
	{
	case h264::Parameters::Preset:
		presetCursor += direction;
		break;

	case h264::Parameters::ConstantRateFactor:
		crfCursor += direction;
		break;
	}
}
