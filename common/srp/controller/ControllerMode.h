#ifndef __CONTROLLERMODE_H__
#define __CONTROLLERMODE_H__

#include <stdint.h>
#include <unordered_map>
#include <string>

namespace controller
{
   enum class ControllerMode : int16_t
   {
      X360 = 0,
      DS4
   };

   std::string controllerModeToString(ControllerMode mode);
   std::string controllerModeDescription(ControllerMode mode);
   ControllerMode parseControllerModeString(std::string s);

   static std::unordered_map<ControllerMode, std::string> const controllerModeOptions
   {
      { ControllerMode::X360, "x360" },
      { ControllerMode::DS4, "ds4" }
   };

   static std::unordered_map<ControllerMode, std::string> const controllerModeDesc
   {
      { ControllerMode::X360, "Xbox 360 Controller" },
      { ControllerMode::DS4, "Dualshock 4 Controller (PS4 Controller)" }
   };
}

#endif
