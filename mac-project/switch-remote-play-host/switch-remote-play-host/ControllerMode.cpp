#include "ControllerMode.h"

std::string ControllerModeToString(ControllerMode mode)
{
    switch(mode)
    {
        default:
        case ControllerMode::X360:
            return "x360";
        case ControllerMode::DS4:
            return "ds4";
    }
}

std::string ControllerModeDescription(ControllerMode mode)
{
    switch(mode)
    {
        case ControllerMode::X360:
            return "Xbox 360 Controller";
        case ControllerMode::DS4:
            return "DualShock 4 Controller (PS4 controller)";

        default:
            return "unknown";
    }
}

ControllerMode ParseControllerModeString(std::string s)
{
    if(s == "x360")
        return ControllerMode::X360;
    else if(s == "ds4")
        return ControllerMode::DS4;
    else
        return ControllerMode::X360;
}