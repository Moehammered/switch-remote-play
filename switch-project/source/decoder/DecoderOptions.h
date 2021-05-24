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
    ThreadType
};

static std::vector<DecoderParameters> const DecoderParameterList
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

uint32_t constexpr DecoderDataSize = sizeof(DecoderData);

static std::vector<int32_t> const AvailableFlags1
{
    AV_CODEC_FLAG_LOW_DELAY, AV_CODEC_FLAG_DROPCHANGED, 
    AV_CODEC_FLAG_PASS1, AV_CODEC_FLAG_PASS2,
    AV_CODEC_FLAG_LOOP_FILTER
};

static std::vector<int32_t> const AvailableFlags2
{
    AV_CODEC_FLAG2_FAST,
    AV_CODEC_FLAG2_NO_OUTPUT,
    AV_CODEC_FLAG2_IGNORE_CROP
};

static std::vector<int32_t> const AvailableAccelFlags
{
    AV_HWACCEL_CODEC_CAP_EXPERIMENTAL,
    AV_HWACCEL_FLAG_IGNORE_LEVEL,
    AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH
};

static std::vector<AVDiscard> const AvailableDiscardFilters
{
    AVDiscard::AVDISCARD_NONE, AVDiscard::AVDISCARD_DEFAULT,
    AVDiscard::AVDISCARD_NONREF, AVDiscard::AVDISCARD_BIDIR,
    AVDiscard::AVDISCARD_NONINTRA, AVDiscard::AVDISCARD_NONKEY,
    AVDiscard::AVDISCARD_ALL
};

static std::vector<int32_t> const AvailableThreadTypes
{
    FF_THREAD_FRAME, FF_THREAD_SLICE
};

int32_t constexpr MinThreadCount = 1;
int32_t constexpr DefaultThreadCount = 4;
int32_t constexpr MaxThreadCount = 24;

#endif
