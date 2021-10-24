#include "FfmpegArgParser.h"
#include <sstream>

FfmpegArgParser::FfmpegArgParser(EncoderConfig const conf, DisplayDeviceInfo const display)
	: config{conf}
{
	commonArgs = ParseCommon(config.commonSettings, display);
    switch (config.commonSettings.videoCodec)
    {
    case ffmpeg::VideoCodec::H264:
        encoderArgs = ParseCpuSettings(config.cpuSettings);
        break;

    case ffmpeg::VideoCodec::H264_AMF:
        encoderArgs = ParseAmdSettings(config.amdSettings);
        break;

    case ffmpeg::VideoCodec::H264_QSV:
        encoderArgs = "Unsupported";
        break;

    case ffmpeg::VideoCodec::H264_NVENC:
        encoderArgs = "Unsupported";
        break;
    }
}

std::string const FfmpegArgParser::CommonSettings()
{
	return commonArgs;
}

std::string const FfmpegArgParser::EncoderSettings()
{
    return encoderArgs;
}

std::string const FfmpegArgParser::CompleteArgs()
{
    return commonArgs + encoderArgs;
}

std::string const FfmpegArgParser::ParseCommon(codec::VideoData const& data, DisplayDeviceInfo const display)
{
    auto vsync = ParseVsyncMode(data.vsyncMode);
    auto hwaccel = ffmpeg::hwAccelModeToStr(data.hwaccelMode);
    auto desktopRes = resolutionToString(data.desktopResolution);
    auto switchRes = resolutionToString(data.switchResolution);
    auto codec = ffmpeg::videoCodecToStr(data.videoCodec);
    auto bitrate = std::to_string(data.bitrateKB) + "k";

    auto ss = std::stringstream{};

    ss << "-probesize 32 -hwaccel " << hwaccel << " ";
    ss << "-y -f gdigrab -framerate " << data.desiredFrameRate << " ";
    ss << "-vsync " << vsync << " ";
    ss << "-video_size " << desktopRes << " ";
    ss << "-offset_x " << display.x << " -offset_y " << display.y << " ";
    ss << "-i desktop -f h264 ";
    ss << "-vcodec " << codec << " ";
    if (data.desktopResolution != data.switchResolution)
        ss << "-vf \"scale=" << switchRes << "\" ";
    ss << "-b:v " << bitrate << " -minrate " << bitrate << " -maxrate " << bitrate << " ";
    ss << "-bufsize " << bitrate << " ";

    return ss.str();
}

std::string const FfmpegArgParser::ParseVsyncMode(ffmpeg::VsyncMode mode)
{
	switch (mode)
	{
    case ffmpeg::VsyncMode::Passthrough:
        return "passthrough"; //each frame is passed to the muxer
    case ffmpeg::VsyncMode::ConstantFps:
        return "cfr"; //constant fps
    case ffmpeg::VsyncMode::VariableFps:
        return "vfr"; //variable fps (prevent duplicate frames)
    case ffmpeg::VsyncMode::DropTime:
        return "drop"; //same as passthrough, but removes timestamps
    case ffmpeg::VsyncMode::Auto:
        return "-1"; //automatically choose between 1 or 2
    default:
        return "vfr";
	}
}

std::string const FfmpegArgParser::ParseCpuSettings(h264::H264Data const& data)
{
    auto parseBitrateMode = [](h264::EncoderBitrateMode mode)
    {
        switch (mode)
        {
        default:
        case h264::EncoderBitrateMode::VariableBitrate:
            return "vbr";

        case h264::EncoderBitrateMode::ConstantBitrate:
            return "cbr";
        }
    };

    auto preset = h264::encoderPresetToStr(data.preset);
    auto bitrateMode = parseBitrateMode(data.bitrateMode);
    auto profile = h264::encoderProfileToStr(data.profile);

    auto ss = std::stringstream{};

    ss << "-preset " << preset << " ";
    ss << "-crf " << data.constantRateFactor << " ";
    if(data.constantRateFactor != 0)
        ss << "-profile:v " << profile << " ";
    ss << "-tune zerolatency -pix_fmt yuv420p ";
    
    ss << "-x264-params \"nal-hrd=" << bitrateMode << ":opencl=true\"";

    return ss.str();
}

std::string const FfmpegArgParser::ParseAmdSettings(h264amf::H264AmfData const& data)
{
    auto parseRateControl = [](h264amf::H264AmfRateControl rate)
    {
        switch (rate)
        {
        default:
        case h264amf::H264AmfRateControl::CQP:
            return "cqp";

        case h264amf::H264AmfRateControl::CBR:
            return "cbr";

        case h264amf::H264AmfRateControl::VBR_Peak:
            return "vbr_peak";

        case h264amf::H264AmfRateControl::VBR_Latency:
            return "vbr_latency";
        }
    };

    auto usage = h264amf::amfUsageToStr(data.usage);
    auto profile = h264amf::amfProfileToStr(data.profile);
    //auto level = h264amf::amfLevelToStr(data.level);
    auto quality = h264amf::amfQualityToStr(data.quality);
    auto rateControl = parseRateControl(data.rateControl);
    auto frameskip = data.frameskip == true ? "true" : "false";
    auto iFrameQuality = h264amf::amfFrameQPToStr(data.qp_i);
    auto pFrameQuality = h264amf::amfFrameQPToStr(data.qp_p);
    auto bFrameQuality = h264amf::amfFrameQPToStr(data.qp_b);

    auto ss = std::stringstream{};

    ss << "-usage " << usage << " ";
    ss << "-profile:v " << profile << " ";
    //ss << "-level " << level << " ";
    ss << "-quality " << quality << " ";
    ss << "-rc " << rateControl << " ";
    ss << "-frame_skipping " << frameskip << " ";
    ss << "-qp_i " << iFrameQuality << " -qp_p " << pFrameQuality << " -qp_b " << bFrameQuality;
    //ss << " -rc_max_rate 1000"; // need to figure out how to define the max bitrate for constrained bitrates

    return ss.str();
}
