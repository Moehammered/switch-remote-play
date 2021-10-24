#include "GenericCodec.h"
#include "../../system/SoftwareKeyboard.h"

GenericCodec::GenericCodec()
    : cursor{codec::videoParametersList},
    desktopResCursor{desktopResolutions},
    switchResCursor{switchResolutions},
    bitrateCursor{codec::bitrates},
    vsyncCursor{ffmpeg::vsyncModeStrMap},
    hwaccelCursor{ffmpeg::hwAccelModeStrMap},
    videoCursor{ffmpeg::videoCodecStrMap},
    desiredFramerate{codec::defaultDesiredFramerate},
    monitorNumber{codec::defaultMonitorNumber}
{
}

void GenericCodec::Set(codec::VideoData const data)
{
	cycleNumber(desktopResCursor, data.desktopResolution);
	cycleNumber(switchResCursor, data.switchResolution);
    cycleNumber(bitrateCursor, data.bitrateKB);

    cycleMap(vsyncCursor, data.vsyncMode);
    cycleMap(hwaccelCursor, data.hwaccelMode);
    cycleMap(videoCursor, data.videoCodec);

    desiredFramerate = data.desiredFrameRate;
    monitorNumber = data.monitorNumber;
}

codec::VideoData const GenericCodec::Data() const
{
	auto data = codec::VideoData{};

	data.desktopResolution = *desktopResCursor;
	data.switchResolution = *switchResCursor;
    data.desiredFrameRate = desiredFramerate;
    data.bitrateKB = *bitrateCursor;
    data.vsyncMode = vsyncCursor.KeyPair().first;
    data.hwaccelMode = hwaccelCursor.KeyPair().first;
    data.videoCodec = videoCursor.KeyPair().first;
    data.monitorNumber = monitorNumber;

	return data;
}

codec::VideoParameters GenericCodec::Current()
{
	return *cursor;
}

codec::VideoParameters GenericCodec::Next()
{
	++cursor;
	return *cursor;
}

codec::VideoParameters GenericCodec::Prev()
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
	case codec::VideoParameters::DesktopResolution:
		desktopResCursor += direction;
		break;

	case codec::VideoParameters::SwitchResolution:
		switchResCursor += direction;
		break;

    case codec::VideoParameters::DesiredFramerate:
        desiredFramerate = (int16_t)keyboardNumber(codec::minDesiredFramerate, codec::maxDesiredFramerate, desiredFramerate);
        break;

    case codec::VideoParameters::BitrateKB:
        bitrateCursor += direction;
        break;

    case codec::VideoParameters::VsyncMode:
        vsyncCursor += direction;
        break;
    
    case codec::VideoParameters::HWAccelMode:
        hwaccelCursor += direction;
        break;

    case codec::VideoParameters::VideoCodec:
        videoCursor += direction;
        break;

    case codec::VideoParameters::MonitorNumber:
        monitorNumber = (int16_t)keyboardNumber(codec::minMonitorNumber, codec::maxMonitorNumber, monitorNumber);
        break;
	}
}
