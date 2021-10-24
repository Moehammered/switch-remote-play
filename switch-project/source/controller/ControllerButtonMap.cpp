#include "ControllerButtonMap.h"
#include "../utils/EnumMapper.h"

namespace controller
{
    ControllerButtonMap parseControllerButtonMapString(std::string s)
    {
        return strToEnum(controlMapOptions, s);
    }

    std::string controllerButtonMapToString(ControllerButtonMap map)
    {
        return enumToStr(controlMapOptions, map);
    }

    std::string controllerButtonMapDescription(ControllerButtonMap map)
    {
        return enumToStr(controlMapDesc, map);
    }
}
