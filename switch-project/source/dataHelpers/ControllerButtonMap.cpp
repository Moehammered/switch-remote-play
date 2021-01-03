#include "ControllerButtonMap.h"

ControllerButtonMap ParseControllerButtonMapString(std::string s)
{
    if(s == "default")
        return ControllerButtonMap::CONTROLLER_MAP_DEFAULT;
    else if(s == "invert xy")
        return ControllerButtonMap::CONTROLLER_MAP_INV_XY;
    else if(s == "invert ab")
        return ControllerButtonMap::CONTROLLER_MAP_INV_AB;
    else if(s == "invert xyab")
        return ControllerButtonMap::CONTROLLER_MAP_INV_XYAB;
    else
        return ControllerButtonMap::CONTROLLER_MAP_DEFAULT;
}

std::string ControllerButtonMapToString(ControllerButtonMap map)
{
    switch(map)
    {
        default:
        case ControllerButtonMap::CONTROLLER_MAP_DEFAULT:
            return "default";
        
        case ControllerButtonMap::CONTROLLER_MAP_INV_XY:
            return "invert xy";

        case ControllerButtonMap::CONTROLLER_MAP_INV_AB:
            return "invert ab";

        case ControllerButtonMap::CONTROLLER_MAP_INV_XYAB:
            return "invert xyab";
    }
}

std::string ControllerButtonMapDescription(ControllerButtonMap map)
{
    switch(map)
    {
        case ControllerButtonMap::CONTROLLER_MAP_DEFAULT:
            return "Default (no inversion)";
        
        case ControllerButtonMap::CONTROLLER_MAP_INV_XY:
            return "Invert XY (swap X and Y buttons)";

        case ControllerButtonMap::CONTROLLER_MAP_INV_AB:
            return "Invert AB (swap A and B buttons)";

        case ControllerButtonMap::CONTROLLER_MAP_INV_XYAB:
            return "Invert XYAB (swap X and Y, and swap A and B buttons)";

        default:
            return "unknown";
    }
}
