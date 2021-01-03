#ifndef __HWACCEL_H__
#define __HWACCEL_H__

#include <stdint.h>
#include <string>

enum HWAccelMode : int16_t
{
    AUTO = 0,
    DXVA2,
    CUDA,
    VAAPI,
    HWAccelMode_COUNT
};

std::string HWAccelModeToString(HWAccelMode mode);
HWAccelMode ParseHWAccelString(std::string s);
std::string HWAccelDescription(HWAccelMode mode);

#endif

