#include "ControllerMode.h"
#include "EnumMapper.h"

namespace controller
{
    std::string ControllerModeToString(ControllerMode mode)
    {
        return enumToStr(controllerModeOptions, mode);
    }

    std::string ControllerModeDescription(ControllerMode mode)
    {
        return enumToStr(controllerModeDesc, mode);
    }

    ControllerMode ParseControllerModeString(std::string s)
    {
        return strToEnum(controllerModeOptions, s);
    }
}