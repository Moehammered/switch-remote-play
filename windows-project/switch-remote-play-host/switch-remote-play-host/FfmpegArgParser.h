#ifndef __FFMPEGARGPARSER_H__
#define __FFMPEGARGPARSER_H__

#include "NetworkData.h"
#include <string>

class FfmpegArgParser
{
public:
	FfmpegArgParser(EncoderConfig const conf);

	std::string const CommonSettings();
	std::string const EncoderSettings();
	std::string const CompleteArgs();

private:
	EncoderConfig const config;
	std::string commonArgs;
	std::string encoderArgs;

	std::string const ParseCommon(VideoData const& data);
	std::string const ParseVsyncMode(VsyncMode mode);
	std::string const ParseCpuSettings(h264::H264Data const& data);
	std::string const ParseAmdSettings(h264amf::H264AMFData const& data);
};

#endif
