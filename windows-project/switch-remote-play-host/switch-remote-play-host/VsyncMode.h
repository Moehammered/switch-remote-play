#ifndef __VSYNCMODE_H__
#define __VSYNCMODE_H__

#include <stdint.h>
#include <string>
#include <unordered_map>

enum class VsyncMode : int16_t
{
    VSYNC_AUTO = -1,
    VSYNC_PASSTHROUGH,
    CONSTANT_FPS,
    VARIABLE_FPS,
    VSYNC_DROP_TIME
};

static std::unordered_map<VsyncMode, std::string> const VsyncModeStrMap
{
    { VsyncMode::VSYNC_AUTO, "auto" },
    { VsyncMode::VSYNC_PASSTHROUGH, "passthrough" },
    { VsyncMode::CONSTANT_FPS, "constant frame rate" },
    { VsyncMode::VARIABLE_FPS, "variable frame rate" },
    { VsyncMode::VSYNC_DROP_TIME, "drop time" },
};

static std::unordered_map<VsyncMode, std::string> const VsyncModeDescMap
{
    { VsyncMode::VSYNC_AUTO, "auto (generally bad - ffmpeg decides.)" },
    { VsyncMode::VSYNC_PASSTHROUGH, "passthrough (render from timestamps)" },
    { VsyncMode::CONSTANT_FPS, "constant frame rate (force fps - latency)" },
    { VsyncMode::VARIABLE_FPS, "variable frame rate" },
    { VsyncMode::VSYNC_DROP_TIME, "drop time (render ignoring timestamps)" },
};

static auto const DefaultVsyncMode{ VsyncMode::VARIABLE_FPS };

std::string VsyncModeToStr(VsyncMode mode);
std::string VsyncModeToDesc(VsyncMode mode);
VsyncMode VsyncModeStrToEnum(std::string s);

#endif
