#include "GenericCodec.h"
#include "../../system/SoftwareKeyboard.h"

GenericCodec::GenericCodec()
    : cursor{VideoParametersList},
    desktopResCursor{DesktopResolutions},
    switchResCursor{SwitchResolutions},
    bitrateCursor{Bitrates},
    vsyncCursor{VsyncModeStrMap},
    hwaccelCursor{HWAccelModeStrMap},
    videoCursor{VideoCodecStrMap},
    desiredFramerate{DefaultDesiredFramerate},
    monitorNumber{DefaultMonitorNumber}
{
}

void GenericCodec::Set(VideoData const data)
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

VideoData const GenericCodec::Data() const
{
	auto data = VideoData{};

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
        desiredFramerate = (int16_t)KeyboardNumber(MinDesiredFramerate, MaxDesiredFramerate, desiredFramerate);
        break;

    case VideoParameters::BitrateKB:
        bitrateCursor += direction;
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

    case VideoParameters::MonitorNumber:
        monitorNumber = (int16_t)KeyboardNumber(MinMonitorNumber, MaxMonitorNumber, monitorNumber);
        break;
	}
}
