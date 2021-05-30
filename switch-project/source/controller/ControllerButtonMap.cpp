#include "ControllerButtonMap.h"
#include "../utils/EnumMapper.h"

namespace controller
{
    ControllerButtonMap ParseControllerButtonMapString(std::string s)
    {
        return strToEnum(controlMapOptions, s);
    }

    std::string ControllerButtonMapToString(ControllerButtonMap map)
    {
        return enumToStr(controlMapOptions, map);
    }

    std::string ControllerButtonMapDescription(ControllerButtonMap map)
    {
        return enumToStr(controlMapDesc, map);
    }
}
