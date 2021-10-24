#ifndef __DECODEROPTIONS_H__
#define __DECODEROPTIONS_H__

#include <vector>
#include <unordered_map>
#include <string>

extern "C"
{
    #include <libavcodec/avcodec.h>
}

enum class DecoderParameters
{
    Flags1,
    Flags2,
    AccelFlags,
    DiscardFilter,
    ThreadType,
    ThreadCount
};

static std::unordered_map<DecoderParameters, std::string> const decoderParamsDesc
{
    { DecoderParameters::Flags1, "Flags 1" },
    { DecoderParameters::Flags2, "Flags 2" },
    { DecoderParameters::AccelFlags, "Accel Flags" },
    { DecoderParameters::DiscardFilter, "Discard Filtler" },
    { DecoderParameters::ThreadType, "Thread Type" },
    { DecoderParameters::ThreadCount, "Thread Count" }
};

static std::vector<DecoderParameters> const decoderParameterList
{
    DecoderParameters::Flags1,
    DecoderParameters::Flags2,
    DecoderParameters::AccelFlags,
    DecoderParameters::DiscardFilter,
    DecoderParameters::ThreadType
};

struct DecoderData
{
    int32_t flags1;
    int32_t flags2;
    int32_t accelFlags;
    AVDiscard discardFilter;
    int32_t threadType;
    int32_t threadCount;
};

auto constexpr decoderDataSize = sizeof(DecoderData);

static std::vector<int32_t> const availableFlags1
{
    AV_CODEC_FLAG_LOW_DELAY, AV_CODEC_FLAG_DROPCHANGED, 
    AV_CODEC_FLAG_PASS1, AV_CODEC_FLAG_PASS2,
    AV_CODEC_FLAG_LOOP_FILTER
};

static std::vector<int32_t> const availableFlags2
{
    AV_CODEC_FLAG2_FAST,
    AV_CODEC_FLAG2_NO_OUTPUT,
    AV_CODEC_FLAG2_IGNORE_CROP
};

static std::vector<int32_t> const availableAccelFlags
{
    AV_HWACCEL_CODEC_CAP_EXPERIMENTAL,
    AV_HWACCEL_FLAG_IGNORE_LEVEL,
    AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH
};

static std::vector<AVDiscard> const availableDiscardFilters
{
    AVDiscard::AVDISCARD_NONE, AVDiscard::AVDISCARD_DEFAULT,
    AVDiscard::AVDISCARD_NONREF, AVDiscard::AVDISCARD_BIDIR,
    AVDiscard::AVDISCARD_NONINTRA, AVDiscard::AVDISCARD_NONKEY,
    AVDiscard::AVDISCARD_ALL
};

static std::vector<int32_t> const availableThreadTypes
{
    FF_THREAD_FRAME, FF_THREAD_SLICE
};

int32_t constexpr minThreadCount = 1;
int32_t constexpr defaultThreadCount = 4;
int32_t constexpr maxThreadCount = 8;

std::unordered_map<DecoderParameters, std::string> decoderParamsToStr(DecoderData const data);
DecoderData decoderParamsFromStr(std::unordered_map<DecoderParameters, std::string> const & map);


#endif
