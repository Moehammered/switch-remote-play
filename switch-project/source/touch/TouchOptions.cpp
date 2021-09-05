#include "TouchOptions.h"
#include "../utils/EnumMapper.h"

namespace touch
{
    TouchScreenMode TouchScreenModeStrToEnum(std::string s)
    {
        return strToEnum(TouchScreenModeStr, s);
    }

    std::string TouchScreenModeEnumToStr(TouchScreenMode mode)
    {
        return enumToStr(TouchScreenModeStr, mode);
    }

    std::string TouchScreenModeEnumToDesc(TouchScreenMode mode)
    {
        return enumToStr(TouchScreenModeDesc, mode);
    }
}