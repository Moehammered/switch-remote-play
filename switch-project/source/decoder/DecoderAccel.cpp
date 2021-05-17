#include "DecoderAccel.h"
#include "../utils/EnumMapper.h"
#include <algorithm>
#include <numeric>

extern "C"
{
    #include <libavcodec/avcodec.h>
}

DecoderAccel::DecoderAccel()
    : accelOptions{
        {AV_HWACCEL_CODEC_CAP_EXPERIMENTAL,  "experimental"},
        {AV_HWACCEL_FLAG_IGNORE_LEVEL,  "ignore level"},
        {AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH,  "allow high depth"},
        {AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH,  "allow profile mismatch"}
    },
    accelDesc{
        {AV_HWACCEL_CODEC_CAP_EXPERIMENTAL,  "EXPERIMENTAL - Allow experimental decoding methods"},
        {AV_HWACCEL_FLAG_IGNORE_LEVEL,  "IGNORE LEVEL - Ignore hardware support level"},
        {AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH,  "ALLOW HIGH DEPTH - Allow output greater than YUV420"},
        {AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH,  "ALLOW PROFILE MISMATCH - Attempt HW accel even if mismatched"}
    }
{
}

std::unordered_map<int32_t, std::string> const& DecoderAccel::Options() const
{
    return accelOptions;
}

std::unordered_map<int32_t, std::string> const& DecoderAccel::Descriptions() const
{
    return accelDesc;
}

std::unordered_map<int32_t, std::string> const DecoderAccel::ToStrings(int32_t flags) const
{
    auto predicate = [&](auto const & a){
        return (a.first & flags) != 0;
    };
    
    auto found = std::unordered_map<int32_t, std::string>{};
    auto inserter = std::inserter(found, found.end());
    std::copy_if(accelOptions.begin(), accelOptions.end(), inserter, predicate);

    return found;
}

int32_t DecoderAccel::Flags(std::vector<std::string> const & flagStrings) const
{
    auto predicate = [](auto const& a, auto const& b){
        return a == b.second;
    };

    auto select = [&](auto const& str){
        auto finder = [&](auto const& a){
            return predicate(str, a);
        };

        auto found = std::find_if(accelOptions.begin(), accelOptions.end(), finder);
        if(found != accelOptions.end())
            return found->first;
        else
            return 0;
    };
    auto foundFlags = std::vector<int32_t>{};
    auto inserter = std::back_inserter(foundFlags);
    std::transform(flagStrings.begin(), flagStrings.end(), inserter, select);

    auto accumulator = [](auto const& a, auto const& b) {
        return a | b;
    };
    return std::accumulate(foundFlags.begin(), foundFlags.end(), 0, accumulator);
}

std::unordered_map<int32_t, std::string> const DecoderAccel::ToDescriptions(int32_t flags) const
{
    auto predicate = [&](auto const & a){
        return (a.first & flags) != 0;
    };
    
    auto found = std::unordered_map<int32_t, std::string>{};
    auto inserter = std::inserter(found, found.end());
    std::copy_if(accelDesc.begin(), accelDesc.end(), inserter, predicate);

    return found;
}
