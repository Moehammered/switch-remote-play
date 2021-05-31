#ifndef __HWACCEL_H__
#define __HWACCEL_H__

#include <stdint.h>
#include <string>
#include <unordered_map>

enum class HWAccelMode : int16_t
{
    AUTO = 0,
    DXVA2,
    CUDA,
    VAAPI
};

static std::unordered_map<HWAccelMode, std::string> const HWAccelModeStrMap
{
    { HWAccelMode::AUTO, "auto" },
    { HWAccelMode::DXVA2, "dxva2" },
    { HWAccelMode::CUDA, "cuda" },
    { HWAccelMode::VAAPI, "vaapi" }
};

static std::unordered_map<HWAccelMode, std::string> const HWAccelModeDescMap
{
    { HWAccelMode::AUTO, "Auto (FFMPEG chooses best option)" },
    { HWAccelMode::DXVA2, "DXVA2 (Direct X)" },
    { HWAccelMode::CUDA, "CUDA (NVENC Transcoder - unsupported)" },
    { HWAccelMode::VAAPI, "VAAPI (Intel/AMD - unsupported)" }
};

static auto const DefaultHWAccelMode{ HWAccelMode::AUTO };

std::string HWAccelModeToStr(HWAccelMode mode);
HWAccelMode HWAccelStrToEnum(std::string s);
std::string HWAccelModeToDesc(HWAccelMode mode);

#endif

