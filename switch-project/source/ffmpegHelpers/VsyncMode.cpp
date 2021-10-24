#include "VsyncMode.h"
#include "../utils/EnumMapper.h"

namespace ffmpeg
{
    std::string vsyncModeToStr(VsyncMode mode)
    {
        return enumToStr(vsyncModeStrMap, mode);
    }

    std::string vsyncModeToDesc(VsyncMode mode)
    {
        return enumToStr(vsyncModeDescMap, mode);
    }

    VsyncMode vsyncModeStrToEnum(std::string s)
    {
        return strToEnum(vsyncModeStrMap, s);
    }
}