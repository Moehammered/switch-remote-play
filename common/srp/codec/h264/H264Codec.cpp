#include "H264Codec.h"

H264Codec::H264Codec()
	: cursor{ h264::h264ParamsList },
	presetCursor{ h264::encoderPresetStrMap },
	rateModeCursor{ h264::encoderBitrateModeStrMap },
	profileCursor{ h264::encoderProfileStrMap },
	constantRateFactor{ h264::defaultCRF }
{
}

void H264Codec::Set(h264::H264Data const data)
{
	cycleMap(presetCursor, data.preset);
	cycleMap(rateModeCursor, data.bitrateMode);
	cycleMap(profileCursor, data.profile);

	constantRateFactor = data.constantRateFactor;
}

h264::H264Data const H264Codec::Data() const
{
	auto data = h264::H264Data{};

	data.preset = presetCursor.KeyPair().first;
	data.constantRateFactor = constantRateFactor;
	data.bitrateMode = rateModeCursor.KeyPair().first;
	data.profile = profileCursor.KeyPair().first;

	return data;
}

h264::H264Parameters H264Codec::Current()
{
	return *cursor;
}

h264::H264Parameters H264Codec::Next()
{
	++cursor;
	return *cursor;
}

h264::H264Parameters H264Codec::Prev()
{
	--cursor;
	return *cursor;
}

void H264Codec::Increase(std::function<int(h264::H264Parameters, int)> const onModify)
{
	ShiftParam(1, onModify);
}

void H264Codec::Decrease(std::function<int(h264::H264Parameters, int)> const onModify)
{
	ShiftParam(-1, onModify);
}

void H264Codec::ShiftParam(int direction, std::function<int(h264::H264Parameters, int)> const onModify)
{
    auto const result = onModify(*cursor, direction);
	switch (*cursor)
	{
	case h264::H264Parameters::Preset:
		presetCursor += result;
		break;

	case h264::H264Parameters::ConstantRateFactor:
		constantRateFactor = result;
		break;

	case h264::H264Parameters::RateControlMode:
		rateModeCursor += result;
		break;

	case h264::H264Parameters::Profile:
		profileCursor += result;
		break;
	}
}
