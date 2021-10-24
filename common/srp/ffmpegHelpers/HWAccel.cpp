#include "HWAccel.h"
#include "../utils/EnumMapper.h"

namespace ffmpeg
{
    std::string hwAccelModeToStr(HWAccelMode mode)
    {
        return enumToStr(hwAccelModeStrMap, mode);
    }

    HWAccelMode hwAccelFromStr(std::string s)
    {
        return strToEnum(hwAccelModeStrMap, s);
    }

    std::string hwAccelModeToDesc(HWAccelMode mode)
    {
        return enumToStr(hwAccelModeDescMap, mode);
    }
}
