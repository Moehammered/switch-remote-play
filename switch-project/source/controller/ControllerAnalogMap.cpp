#include "ControllerAnalogMap.h"
#include "../utils/EnumMapper.h"

namespace controller
{
    ControllerAnalogMap ParseControllerAnalogMapString(std::string s)
    {
        return strToEnum(analogMapOptions, s);
    }

    std::string ControllerAnalogMapToString(ControllerAnalogMap map)
    {
        return enumToStr(analogMapOptions, map);
    }

    std::string ControllerAnalogMapDescription(ControllerAnalogMap map)
    {
        return enumToStr(analogMapDesc, map);
    }
}