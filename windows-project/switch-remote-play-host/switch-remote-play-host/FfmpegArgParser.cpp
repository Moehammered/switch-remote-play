#include "FfmpegArgParser.h"
#include <sstream>

FfmpegArgParser::FfmpegArgParser(EncoderConfig const conf)
	: config{conf}
{
	commonArgs = ParseCommon(config.commonSettings);
    switch (config.commonSettings.videoCodec)
    {
    case VideoCodec::H264:
        encoderArgs = ParseCpuSettings(config.cpuSettings);
        break;

    case VideoCodec::H264_AMF:
        encoderArgs = ParseAmdSettings(config.amdSettings);
        break;

    case VideoCodec::H264_QSV:
        encoderArgs = "Unsupported";
        break;

    case VideoCodec::H264_NVENC:
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

std::string const FfmpegArgParser::ParseCommon(VideoData const& data)
{
    auto vsync = ParseVsyncMode(data.vsyncMode);
    auto hwaccel = HWAccelModeToStr(data.hwaccelMode);
    auto desktopRes = ResolutionToString(data.desktopResolution);
    auto switchRes = ResolutionToString(data.switchResolution);
    auto codec = VideoCodecToStr(data.videoCodec);
    auto bitrate = std::to_string(data.bitrateKB) + "k";

    auto ss = std::stringstream{};

    ss << "-probesize 32 -hwaccel " << hwaccel << " ";
    ss << "-y -f gdigrab -framerate " << data.desiredFrameRate << " ";
    ss << "-vsync " << vsync << " ";
    ss << "-video_size " << desktopRes << " -i desktop -f h264 ";
    ss << "-vcodec " << codec << " ";
    if(data.desktopResolution != data.switchResolution)
        ss << "-vf \"scale=" << switchRes << "\" ";
    ss << "-b:v " << bitrate << " -minrate " << bitrate << " -maxrate " << bitrate << " ";
    ss << "-bufsize " << bitrate << " ";

	return ss.str();
}

std::string const FfmpegArgParser::ParseVsyncMode(VsyncMode mode)
{
	switch (mode)
	{
    case VsyncMode::VSYNC_PASSTHROUGH:
        return "passthrough"; //each frame is passed to the muxer
    case VsyncMode::CONSTANT_FPS:
        return "cfr"; //constant fps
    case VsyncMode::VARIABLE_FPS:
        return "vfr"; //variable fps (prevent duplicate frames)
    case VsyncMode::VSYNC_DROP_TIME:
        return "drop"; //same as passthrough, but removes timestamps
    case VsyncMode::VSYNC_AUTO:
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

    auto preset = h264::EncoderPresetToStr(data.Preset);
    auto bitrateMode = parseBitrateMode(data.BitrateMode);
    auto profile = h264::EncoderProfileToStr(data.Profile);

    auto ss = std::stringstream{};

    ss << "-preset " << preset << " ";
    ss << "-crf " << data.ConstantRateFactor << " ";
    if(data.ConstantRateFactor != 0)
        ss << "-profile:v " << profile << " ";
    ss << "-tune zerolatency -pix_fmt yuv420p ";
    
    ss << "-x264-params \"nal-hrd=" << bitrateMode << ":opencl=true\"";

    return ss.str();
}

std::string const FfmpegArgParser::ParseAmdSettings(h264amf::H264AMFData const& data)
{
    auto parseRateControl = [](h264amf::H264AMF_RATECONTROL rate)
    {
        switch (rate)
        {
        default:
        case h264amf::H264AMF_RATECONTROL::CQP:
            return "cqp";

        case h264amf::H264AMF_RATECONTROL::CBR:
            return "cbr";

        case h264amf::H264AMF_RATECONTROL::VBR_PEAK:
            return "vbr_peak";

        case h264amf::H264AMF_RATECONTROL::VBR_LATENCY:
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
