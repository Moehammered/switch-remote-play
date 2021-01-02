#include "VideoCodecMode.h"

std::string VideoCodecModeToString(VideoCodecMode mode)
{
    switch(mode)
    {
        default:
        case VideoCodecMode::H264:
            return "h264";
        case VideoCodecMode::H264_AMF:
            return "h264_amf";
        case VideoCodecMode::H264_NVENC:
            return "h264_nvenc";
        case VideoCodecMode::H264_QSV:
            return "h264_qsv";
    }
}

std::string VideoCodecModeDescription(VideoCodecMode mode)
{
    switch(mode)
    {
        case VideoCodecMode::H264:
            return "h264";
        case VideoCodecMode::H264_AMF:
            return "h264 AMF (AMD)";
        case VideoCodecMode::H264_NVENC:
            return "h264 NVENC (NVIDIA) [untested]";
        case VideoCodecMode::H264_QSV:
            return "h264 Quick Scan (Intel - requires Intel CPU) [untested]";
        default:
            return "unknown";
    }
}

VideoCodecMode ParseVideoCodecModeString(std::string s)
{
    if(s == "h264")
        return VideoCodecMode::H264;
    else if(s == "h264_amf")
        return VideoCodecMode::H264_AMF;
    else if(s == "h264_nvenc")
        return VideoCodecMode::H264_NVENC;
    else if(s == "h264_qsv")
        return VideoCodecMode::H264_QSV;
    else
        return VideoCodecMode::H264;
}
