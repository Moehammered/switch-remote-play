#include "GenericCodec.h"

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

void GenericCodec::Increase(std::function<int(codec::VideoParameters,int)> const onModify)
{
	ShiftParam(1, onModify);
}

void GenericCodec::Decrease(std::function<int(codec::VideoParameters,int)> const onModify)
{
	ShiftParam(-1, onModify);
}

void GenericCodec::ShiftParam(int direction, std::function<int(codec::VideoParameters,int)> const onModify)
{
    auto const result = onModify(*cursor, direction);
	switch (*cursor)
	{
	case codec::VideoParameters::DesktopResolution:
		desktopResCursor += result;
		break;

	case codec::VideoParameters::SwitchResolution:
		switchResCursor += result;
		break;

    case codec::VideoParameters::DesiredFramerate:
        desiredFramerate = result;
        break;

    case codec::VideoParameters::BitrateKB:
        bitrateCursor += result;
        break;

    case codec::VideoParameters::VsyncMode:
        vsyncCursor += result;
        break;
    
    case codec::VideoParameters::HWAccelMode:
        hwaccelCursor += result;
        break;

    case codec::VideoParameters::VideoCodec:
        videoCursor += result;
        break;

    case codec::VideoParameters::MonitorNumber:
        monitorNumber = result;
        break;
	}
}
