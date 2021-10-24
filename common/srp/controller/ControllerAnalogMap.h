#ifndef __CONTROLLERANALOGMAP_H__
#define __CONTROLLERANALOGMAP_H__

#include <string>
#include <unordered_map>

namespace controller
{
    enum class ControllerAnalogMap : int16_t
    {
        None = 0,
        InvertX,
        InvertY,
        InvertXY
    };

    ControllerAnalogMap parseControllerAnalogMapString(std::string s);
    std::string controllerAnalogMapToString(ControllerAnalogMap map);
    std::string controllerAnalogMapDescription(ControllerAnalogMap map);

    static std::unordered_map<ControllerAnalogMap, std::string> const analogMapOptions
    {
        { ControllerAnalogMap::None, "none" },
        { ControllerAnalogMap::InvertX, "invert x" },
        { ControllerAnalogMap::InvertY, "invert y" },
        { ControllerAnalogMap::InvertXY, "invert xy" }
    };

    static std::unordered_map<ControllerAnalogMap, std::string> const analogMapDesc
    {
        { ControllerAnalogMap::None, "None (no inversion)" },
        { ControllerAnalogMap::InvertX, "Invert X axis" },
        { ControllerAnalogMap::InvertY, "Invert Y axis" },
        { ControllerAnalogMap::InvertXY, "Invert X and Y axis" }
    };
}

#endif
