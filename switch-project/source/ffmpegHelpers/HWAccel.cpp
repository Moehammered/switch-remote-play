#include "HWAccel.h"

std::string HWAccelModeToString(HWAccelMode mode)
{
    switch(mode)
    {
        default:
        case HWAccelMode::AUTO:
            return "auto";
        case HWAccelMode::DXVA2:
            return "dxva2";
        case HWAccelMode::VAAPI:
            return "vaapi";
        case HWAccelMode::CUDA:
            return "cuda";
    }
}

HWAccelMode ParseHWAccelString(std::string s)
{
    if(s == "auto")
        return HWAccelMode::AUTO;
    else if(s == "dxva2")
        return HWAccelMode::DXVA2;
    else if(s == "cuda")
        return HWAccelMode::CUDA;
    else if(s == "vaapi")
        return HWAccelMode::VAAPI;
    else
        return HWAccelMode::AUTO;
}

std::string HWAccelDescription(HWAccelMode mode)
{
    switch(mode)
    {
        case HWAccelMode::AUTO:
            return "Auto (FFMPEG chooses best option)";
        case HWAccelMode::DXVA2:
            return "DXVA2 (Direct X)";
        case HWAccelMode::VAAPI:
            return "VAAPI (Intel/AMD - unsupported)";
        case HWAccelMode::CUDA:
            return "CUDA (NVENC Transcoder - unsupported)";

        default:
            return "unknown";
    }
}
