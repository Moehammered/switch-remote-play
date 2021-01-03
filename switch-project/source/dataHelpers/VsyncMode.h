#ifndef __VSYNCMODE_H__
#define __VSYNCMODE_H__

#include <stdint.h>
#include <string>

enum VsyncMode : int16_t
{
    VSYNC_AUTO = -1,
    VSYNC_PASSTHROUGH,
    CONSTANT_FPS,
    VARIABLE_FPS,
    VSYNC_DROP_TIME,
    VSYNC_MODE_COUNT
};

std::string VsyncModeToString(VsyncMode mode);
std::string VsyncModeDescription(VsyncMode mode);
VsyncMode ParseVsyncModeString(std::string s);

#endif
