#ifndef __CONTROLLERBUTTONMAP_H__
#define __CONTROLLERBUTTONMAP_H__

#include <stdint.h>
#include <unordered_map>
#include <string>

namespace controller
{
    enum class ControllerButtonMap : int16_t
    {
        None = 0,
        InvertXY,
        InvertAB,
        InvertXYAB
    };

    ControllerButtonMap ParseControllerButtonMapString(std::string s);
    std::string ControllerButtonMapToString(ControllerButtonMap map);
    std::string ControllerButtonMapDescription(ControllerButtonMap map);

    static std::unordered_map<ControllerButtonMap, std::string> const controlMapOptions
    {
        { ControllerButtonMap::None, "none" },
        { ControllerButtonMap::InvertXY, "invert xy" },
        { ControllerButtonMap::InvertAB, "invert ab" },
        { ControllerButtonMap::InvertXYAB, "invert xyab" }
    };

    static std::unordered_map<ControllerButtonMap, std::string> const controlMapDesc
    {
        { ControllerButtonMap::None, "None (no inversion)" },
        { ControllerButtonMap::InvertXY, "Invert XY (swap X and Y buttons)" },
        { ControllerButtonMap::InvertAB, "Invert AB (swap A and B buttons)" },
        { ControllerButtonMap::InvertXYAB, "Invert XYAB (swap X and Y buttons + swap A and B buttons)" }
    };
}

#endif
