#ifndef __VSYNCMODE_H__
#define __VSYNCMODE_H__

#include <stdint.h>
#include <string>
#include <unordered_map>

namespace ffmpeg
{
    enum class VsyncMode : int16_t
    {
        Auto = -1,
        Passthrough,
        ConstantFps,
        VariableFps,
        DropTime
    };

    static std::unordered_map<VsyncMode, std::string> const vsyncModeStrMap
    {
        { VsyncMode::Auto, "auto" },
        { VsyncMode::Passthrough, "passthrough" },
        { VsyncMode::ConstantFps, "constant frame rate" },
        { VsyncMode::VariableFps, "variable frame rate" },
        { VsyncMode::DropTime, "drop time" },
    };

    static std::unordered_map<VsyncMode, std::string> const vsyncModeDescMap
    {
        { VsyncMode::Auto, "auto (generally bad - ffmpeg decides.)" },
        { VsyncMode::Passthrough, "passthrough (render from timestamps)" },
        { VsyncMode::ConstantFps, "constant frame rate (force fps - latency)" },
        { VsyncMode::VariableFps, "variable frame rate" },
        { VsyncMode::DropTime, "drop time (render ignoring timestamps)" },
    };

    static auto const defaultVsyncMode { VsyncMode::VariableFps };

    std::string vsyncModeToStr(VsyncMode mode);
    std::string vsyncModeToDesc(VsyncMode mode);
    VsyncMode vsyncModeStrToEnum(std::string s);
}

#endif
