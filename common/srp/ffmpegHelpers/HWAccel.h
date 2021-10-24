#ifndef __HWACCEL_H__
#define __HWACCEL_H__

#include <stdint.h>
#include <string>
#include <unordered_map>

namespace ffmpeg
{
    enum class HWAccelMode : int16_t
    {
        Auto = 0,
        DXVA2,
        CUDA,
        VAAPI
    };

    static std::unordered_map<HWAccelMode, std::string> const hwAccelModeStrMap
    {
        { HWAccelMode::Auto, "auto" },
        { HWAccelMode::DXVA2, "dxva2" },
        { HWAccelMode::CUDA, "cuda" },
        { HWAccelMode::VAAPI, "vaapi" }
    };

    static std::unordered_map<HWAccelMode, std::string> const hwAccelModeDescMap
    {
        { HWAccelMode::Auto, "Auto (FFMPEG chooses best option)" },
        { HWAccelMode::DXVA2, "DXVA2 (Direct X)" },
        { HWAccelMode::CUDA, "CUDA (NVENC Transcoder - unsupported)" },
        { HWAccelMode::VAAPI, "VAAPI (Intel/AMD - unsupported)" }
    };

    auto constexpr defaultHWAccelMode { HWAccelMode::Auto };

    HWAccelMode hwAccelFromStr(std::string s);
    std::string hwAccelModeToStr(HWAccelMode mode);
    std::string hwAccelModeToDesc(HWAccelMode mode);
}

#endif

