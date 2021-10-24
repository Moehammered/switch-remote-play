#include "ControllerMode.h"
#include "../utils/EnumMapper.h"

namespace controller
{
    std::string controllerModeToString(ControllerMode mode)
    {
        return enumToStr(controllerModeOptions, mode);
    }

    std::string controllerModeDescription(ControllerMode mode)
    {
        return enumToStr(controllerModeDesc, mode);
    }

    ControllerMode parseControllerModeString(std::string s)
    {
        return strToEnum(controllerModeOptions, s);
    }
}