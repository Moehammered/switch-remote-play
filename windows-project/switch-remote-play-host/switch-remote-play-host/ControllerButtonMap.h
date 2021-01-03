#ifndef __CONTROLLERBUTTONMAP_H__
#define __CONTROLLERBUTTONMAP_H__

#include <stdint.h>
#include <string>

enum ControllerButtonMap : int16_t
{
    CONTROLLER_MAP_DEFAULT = 0,
    CONTROLLER_MAP_INV_XY,
    CONTROLLER_MAP_INV_AB,
    CONTROLLER_MAP_INV_XYAB,
    CONTROLLER_MAP_COUNT
};

ControllerButtonMap ParseControllerButtonMapString(std::string s);
std::string ControllerButtonMapToString(ControllerButtonMap map);
std::string ControllerButtonMapDescription(ControllerButtonMap map);

#endif
