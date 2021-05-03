#include "VideoCodec.h"
#include "../utils/EnumMapper.h"

std::string VideoCodecToStr(VideoCodec mode)
{
    return enumToStr(VideoCodecStrMap, mode);
}

std::string VideoCodecToDesc(VideoCodec mode)
{
    return enumToStr(VideoCodecDescMap, mode);
}

VideoCodec VideoCodecStrToEnum(std::string s)
{
    return strToEnum(VideoCodecStrMap, s);
}
