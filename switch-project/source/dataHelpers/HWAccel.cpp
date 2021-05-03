#include "HWAccel.h"
#include "../utils/EnumMapper.h"

std::string HWAccelModeToStr(HWAccelMode mode)
{
    return enumToStr(HWAccelModeStrMap, mode);
}

HWAccelMode HWAccelStrToEnum(std::string s)
{
    return strToEnum(HWAccelModeStrMap, s);
}

std::string HWAccelModeToDesc(HWAccelMode mode)
{
    return enumToStr(HWAccelModeDescMap, mode);
}
