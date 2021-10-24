#include "VideoCodec.h"
#include "../utils/EnumMapper.h"

namespace ffmpeg
{
    std::string videoCodecToStr(VideoCodec mode)
    {
        return enumToStr(videoCodecStrMap, mode);
    }

    std::string videoCodecToDesc(VideoCodec mode)
    {
        return enumToStr(videoCodecDescMap, mode);
    }

    VideoCodec videoCodecStrToEnum(std::string s)
    {
        return strToEnum(videoCodecStrMap, s);
    }
}