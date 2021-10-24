#ifndef __DECODERACCEL_H__
#define __DECODERACCEL_H__

#include <unordered_map>
#include <string>
#include <vector>

extern "C"
{
    #include <libavcodec/avcodec.h>
}

namespace decoder
{
    std::unordered_map<int32_t, std::string> const accelFlagsToStrings(int32_t flags);
    std::unordered_map<int32_t, std::string> const accelFlagsToDescriptions(int32_t flags);
    int32_t const parseAccelFlagStrings(std::vector<std::string> const & strings);

    static std::unordered_map<int32_t, std::string> const accelOptions
    {
        {AV_HWACCEL_CODEC_CAP_EXPERIMENTAL,  "experimental"},
        {AV_HWACCEL_FLAG_IGNORE_LEVEL,  "ignore level"},
        {AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH,  "allow high depth"},
        {AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH,  "allow profile mismatch"}
    };
    
    static std::unordered_map<int32_t, std::string> const accelDesc
    {
        {AV_HWACCEL_CODEC_CAP_EXPERIMENTAL,  "EXPERIMENTAL - Allow experimental decoding methods"},
        {AV_HWACCEL_FLAG_IGNORE_LEVEL,  "IGNORE LEVEL - Ignore hardware support level"},
        {AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH,  "ALLOW HIGH DEPTH - Allow output greater than YUV420"},
        {AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH,  "ALLOW PROFILE MISMATCH - Attempt HW accel even if mismatched"}
    };
}

#endif
