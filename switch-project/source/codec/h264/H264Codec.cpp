#include "H264Codec.h"

H264Codec::H264Codec()
	: cursor{ h264::ParamsList },
	presetCursor{ h264::EncoderPresetStrMap },
	rateModeCursor{ h264::EncoderBitrateModeStrMap },
	profileCursor{ h264::EncoderProfileStrMap },
	crfCursor{ h264::DefaultCRF }
{
}

void H264Codec::Set(h264::H264Data const data)
{
	cycleMap(presetCursor, data.Preset);
	cycleMap(rateModeCursor, data.BitrateMode);
	cycleMap(profileCursor, data.Profile);
	cycleNumber(crfCursor, data.ConstantRateFactor);
}

h264::H264Data const H264Codec::Data() const
{
	auto data = h264::H264Data{};

	data.Preset = presetCursor.KeyPair().first;
	data.ConstantRateFactor = *crfCursor;
	data.BitrateMode = rateModeCursor.KeyPair().first;
	data.Profile = profileCursor.KeyPair().first;

	return data;
}

h264::Parameters H264Codec::Current()
{
	return *cursor;
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

	case h264::Parameters::RateControlMode:
		rateModeCursor += direction;
		break;

	case h264::Parameters::Profile:
		profileCursor += direction;
		break;
	}
}
