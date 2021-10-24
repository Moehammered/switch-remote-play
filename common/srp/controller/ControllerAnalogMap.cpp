#include "ControllerAnalogMap.h"
#include "../utils/EnumMapper.h"

namespace controller
{
    ControllerAnalogMap parseControllerAnalogMapString(std::string s)
    {
        return strToEnum(analogMapOptions, s);
    }

    std::string controllerAnalogMapToString(ControllerAnalogMap map)
    {
        return enumToStr(analogMapOptions, map);
    }

    std::string controllerAnalogMapDescription(ControllerAnalogMap map)
    {
        return enumToStr(analogMapDesc, map);
    }
}