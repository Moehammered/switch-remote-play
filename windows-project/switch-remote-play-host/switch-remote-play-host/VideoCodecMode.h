#ifndef __VIDEOCODECMODE_H__
#define __VIDEOCODECMODE_H__

#include <stdint.h>
#include <string>

enum VideoCodecMode : int16_t
{
    H264 = 0,
    H264_AMF,
    H264_NVENC,
    H264_QSV,
    VIDEOCODECMODE_COUNT
};

std::string VideoCodecModeToString(VideoCodecMode mode);
std::string VideoCodecModeDescription(VideoCodecMode mode);
VideoCodecMode ParseVideoCodecModeString(std::string s);

#endif
