#include "VsyncMode.h"

std::string VsyncModeToString(VsyncMode mode)
{
    switch(mode)
    {
        case VsyncMode::VSYNC_AUTO:
            return "auto";

        case VsyncMode::VSYNC_PASSTHROUGH:
            return "passthrough";

        case VsyncMode::CONSTANT_FPS:
            return "constant frame rate";

        default:
        case VsyncMode::VARIABLE_FPS:
            return "variable frame rate";

        case VsyncMode::VSYNC_DROP_TIME:
            return "drop time";
    }
}

std::string VsyncModeDescription(VsyncMode mode)
{
    switch(mode)
    {
        case VsyncMode::VSYNC_AUTO:
            return "auto (generally bad - ffmpeg decides.)";

        case VsyncMode::VSYNC_PASSTHROUGH:
            return "passthrough (render from timestamps)";

        case VsyncMode::CONSTANT_FPS:
            return "constant frame rate (force fps - latency)";

        case VsyncMode::VARIABLE_FPS:
            return "variable frame rate";

        case VsyncMode::VSYNC_DROP_TIME:
            return "drop time (render ignoring timestamps)";

        default:
            return "unknown";
    }
}

VsyncMode ParseVsyncModeString(std::string s)
{
    if(s == "auto")
        return VsyncMode::VSYNC_AUTO;
    else if(s == "passthrough")
        return VsyncMode::VSYNC_PASSTHROUGH;
    else if(s == "constant frame rate")
        return VsyncMode::CONSTANT_FPS;
    else if(s == "variable frame rate")
        return VsyncMode::VARIABLE_FPS;
    else if(s == "drop time")
        return VsyncMode::VSYNC_DROP_TIME;
    else
        return VsyncMode::VARIABLE_FPS;
}
