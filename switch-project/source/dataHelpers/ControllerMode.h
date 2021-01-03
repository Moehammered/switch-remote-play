#ifndef __CONTROLLERMODE_H__
#define __CONTROLLERMODE_H__

#include <stdint.h>
#include <string>

enum ControllerMode : int16_t
{
   X360 = 0,
   DS4,
   CONTROLLER_MODE_COUNT
};

std::string ControllerModeToString(ControllerMode mode);
std::string ControllerModeDescription(ControllerMode mode);
ControllerMode ParseControllerModeString(std::string s);

#endif
