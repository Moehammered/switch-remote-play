#ifndef __FFMPEGARGPARSER_H__
#define __FFMPEGARGPARSER_H__

#include "DisplayDeviceService.h"
#include "srp/network/NetworkData.h"
#include <string>

class FfmpegArgParser
{
public:
	FfmpegArgParser(EncoderConfig const conf, DisplayDeviceInfo const display);

	std::string const CommonSettings();
	std::string const EncoderSettings();
	std::string const CompleteArgs();

private:
	EncoderConfig const config;
	std::string commonArgs;
	std::string encoderArgs;

	std::string const ParseCommon(codec::VideoData const& data, DisplayDeviceInfo const display);
	std::string const ParseVsyncMode(ffmpeg::VsyncMode mode);
	std::string const ParseCpuSettings(h264::H264Data const& data);
	std::string const ParseAmdSettings(h264amf::H264AmfData const& data);
};

#endif
