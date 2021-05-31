#ifndef __VIDEOCODEC_H__
#define __VIDEOCODEC_H__

#include <stdint.h>
#include <string>
#include <unordered_map>

enum class VideoCodec : int16_t
{
    H264 = 0,
    H264_AMF,
    H264_NVENC,
    H264_QSV
};

static std::unordered_map<VideoCodec, std::string> const VideoCodecStrMap
{
    { VideoCodec::H264, "h264" },
    { VideoCodec::H264_AMF, "h264_amf" },
    { VideoCodec::H264_NVENC, "h264_nvenc" },
    { VideoCodec::H264_QSV, "h264_qsv" },
};

static std::unordered_map<VideoCodec, std::string> const VideoCodecDescMap
{
    { VideoCodec::H264, "h264 (CPU)" },
    { VideoCodec::H264_AMF, "h264 AMF (AMD)" },
    { VideoCodec::H264_NVENC, "h264 NVENC (NVIDIA) [unsupported]" },
    { VideoCodec::H264_QSV, "h264 Quick Scan Video (Intel) [unsupported]" },
};

static auto const DefaultVideoCodec { VideoCodec::H264 };

std::string VideoCodecToStr(VideoCodec mode);
std::string VideoCodecToDesc(VideoCodec mode);
VideoCodec VideoCodecStrToEnum(std::string s);

#endif
