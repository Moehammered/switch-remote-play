#include "AudioFormat.h"
#include "../utils/EnumMapper.h"

namespace audio
{
    std::string audioFormatToStr(AudioFormat format)
    {
        return enumToStr(audioFormatStrMap, format);
    }

    std::string audioFormatToDesc(AudioFormat format)
    {
        return enumToStr(audioFormatDescMap, format);
    }

    AudioFormat audioFormatStrToEnum(std::string const s)
    {
        return strToEnum(audioFormatStrMap, s);
    }
}