#include "VsyncMode.h"
#include "EnumMapper.h"

std::string VsyncModeToStr(VsyncMode mode)
{
    return enumToStr(VsyncModeStrMap, mode);
}

std::string VsyncModeToDesc(VsyncMode mode)
{
    return enumToStr(VsyncModeDescMap, mode);
}

VsyncMode VsyncModeStrToEnum(std::string s)
{
    return strToEnum(VsyncModeStrMap, s);
}
