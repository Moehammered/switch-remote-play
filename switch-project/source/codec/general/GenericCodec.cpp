#include "GenericCodec.h"

GenericCodec::GenericCodec()
    : cursor{VideoParametersList},
    desktopResCursor{DesktopResolutions},
    switchResCursor{SwitchResolutions},
    fpsCursor{Framerates},
    vsyncCursor{VsyncModeStrMap},
    hwaccelCursor{HWAccelModeStrMap},
    videoCursor{VideoCodecStrMap}
{
}

void GenericCodec::Set(VideoData const data)
{
	cycleNumber(desktopResCursor, data.desktopResolution);
	cycleNumber(switchResCursor, data.switchResolution);
	cycleNumber(fpsCursor, data.desiredFrameRate);

    cycleMap(vsyncCursor, data.vsyncMode);
    cycleMap(hwaccelCursor, data.hwaccelMode);
    cycleMap(videoCursor, data.videoCodec);
}

VideoData const GenericCodec::Data() const
{
	auto data = VideoData{};

	data.desktopResolution = *desktopResCursor;
	data.switchResolution = *switchResCursor;
    data.desiredFrameRate = *fpsCursor;
    data.vsyncMode = vsyncCursor.KeyPair().first;
    data.hwaccelMode = hwaccelCursor.KeyPair().first;
    data.videoCodec = videoCursor.KeyPair().first;

	return data;
}

VideoParameters GenericCodec::Current()
{
	return *cursor;
}

VideoParameters GenericCodec::Next()
{
	++cursor;
	return *cursor;
}

VideoParameters GenericCodec::Prev()
{
	--cursor;
	return *cursor;
}

void GenericCodec::Increase()
{
	ShiftParam(1);
}

void GenericCodec::Decrease()
{
	ShiftParam(-1);
}

void GenericCodec::ShiftParam(int direction)
{
	switch (*cursor)
	{
	case VideoParameters::DesktopResolution:
		desktopResCursor += direction;
		break;

	case VideoParameters::SwitchResolution:
		switchResCursor += direction;
		break;

    case VideoParameters::DesiredFramerate:
        fpsCursor += direction;
        break;

    case VideoParameters::VsyncMode:
        vsyncCursor += direction;
        break;
    
    case VideoParameters::HWAccelMode:
        hwaccelCursor += direction;
        break;

    case VideoParameters::VideoCodec:
        videoCursor += direction;
        break;
	}
}
