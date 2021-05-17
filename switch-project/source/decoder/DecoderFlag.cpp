#include "DecoderFlag.h"
#include <algorithm>
#include <numeric>

extern "C"
{
    #include <libavcodec/avcodec.h>
}

std::unordered_map<int32_t, std::string> const& DecoderFlag::Flag1Options() const
{
    return flag1Options;
}

std::unordered_map<int32_t, std::string> const& DecoderFlag::Flag1Descriptions() const
{
    return flag1Desc;
}

std::unordered_map<int32_t, std::string> const& DecoderFlag::Flag2Options() const
{
    return flag2Options;
}

std::unordered_map<int32_t, std::string> const& DecoderFlag::Flag2Descriptions() const
{
    return flag2Desc;
}

std::unordered_map<int32_t, std::string> const DecoderFlag::Flag1Strings(int32_t flags) const
{
    return ToStrings(flags, flag1Options);
}

std::unordered_map<int32_t, std::string> const DecoderFlag::Flag2Strings(int32_t flags) const
{
    return ToStrings(flags, flag2Options);
}

std::unordered_map<int32_t, std::string> const DecoderFlag::Flag1Descriptions(int32_t flags) const
{
    return ToStrings(flags, flag1Desc);
}

std::unordered_map<int32_t, std::string> const DecoderFlag::Flag2Descriptions(int32_t flags) const
{
    return ToStrings(flags, flag2Desc);
}

int32_t DecoderFlag::Flags1(std::vector<std::string> const& flagStrings) const
{
    return ToFlags(flagStrings, flag1Options);
}

int32_t DecoderFlag::Flags2(std::vector<std::string> const& flagStrings) const
{
    return ToFlags(flagStrings, flag1Options);
}

std::unordered_map<int32_t, std::string> const DecoderFlag::ToStrings(int32_t flags, std::unordered_map<int32_t, std::string> const & map) const
{
    auto found = std::unordered_map<int32_t, std::string>{};
    auto inserter = std::inserter(found, found.end());

    auto predicate = [&](auto const& a)
    {
        return (a.first & flags) != 0;
    };
    std::copy_if(map.begin(), map.end(), inserter, predicate);

    return found;
}

int32_t DecoderFlag::ToFlags(std::vector<std::string> const& flags, std::unordered_map<int32_t, std::string> const& map) const
{
    auto found = std::vector<int32_t>{};
    auto inserter = std::back_inserter(found);

    auto strSelect = [&](auto const& str)
    {
        auto itr = std::find_if(map.begin(), map.end(), [&](auto const& pair){
            return pair.second == str;
        });
        if(itr != map.end())
            return itr->first;
        else
            return 0;
    };

    std::transform(flags.begin(), flags.end(), inserter, strSelect);

    return std::accumulate(found.begin(), found.end(), 0, [](auto const& a, auto const& b){
        return a | b;
    });
}

DecoderFlag::DecoderFlag()
    : flag1Options{
        {AV_CODEC_FLAG_UNALIGNED,  "unaligned"},
        {AV_CODEC_FLAG_QSCALE,  "qscale"},
        {AV_CODEC_FLAG_4MV,  "4mv"},
        {AV_CODEC_FLAG_OUTPUT_CORRUPT,  "output corrupt"},
        {AV_CODEC_FLAG_QPEL,  "qpel"},
        {AV_CODEC_FLAG_DROPCHANGED,  "drop changed"},
        {AV_CODEC_FLAG_PASS1,  "pass1"},
        {AV_CODEC_FLAG_PASS2,  "pass2"},
        {AV_CODEC_FLAG_LOOP_FILTER,  "loop filter"},
        {AV_CODEC_FLAG_GRAY,  "gray"},
        {AV_CODEC_FLAG_PSNR,  "psnr"},
        {AV_CODEC_FLAG_TRUNCATED,  "truncated"},
        {AV_CODEC_FLAG_INTERLACED_DCT,  "interlaced dct"},
        {AV_CODEC_FLAG_LOW_DELAY,  "low delay"},
        {AV_CODEC_FLAG_GLOBAL_HEADER,  "global header"},
        {AV_CODEC_FLAG_BITEXACT,  "bitexact"},
        {AV_CODEC_FLAG_AC_PRED,  "ac pred"},
        {AV_CODEC_FLAG_INTERLACED_ME,  "interlaced me"},
        {AV_CODEC_FLAG_CLOSED_GOP,  "closed gop"}
    },
    flag1Desc{
        {AV_CODEC_FLAG_UNALIGNED,  "UNALIGNED (unaligned frames)"},
        {AV_CODEC_FLAG_QSCALE,  "QSCALE (fixed qscale)"},
        {AV_CODEC_FLAG_4MV,  "4MV (H263 codec prediction)"},
        {AV_CODEC_FLAG_OUTPUT_CORRUPT,  "OUTPUT CORRUPT (output corrupt frames)"},
        {AV_CODEC_FLAG_QPEL,  "QPEL (qpel mc)"},
        {AV_CODEC_FLAG_DROPCHANGED,  "DROP CHANGED (drop differing frames)"},
        {AV_CODEC_FLAG_PASS1,  "PASS1 (2 pass in 1st pass mode)"},
        {AV_CODEC_FLAG_PASS2,  "PASS2 (2 pass in 2nd pass mode)"},
        {AV_CODEC_FLAG_LOOP_FILTER,  "LOOP FILTER (loop filter)"},
        {AV_CODEC_FLAG_GRAY,  "GRAY (grayscale only)"},
        {AV_CODEC_FLAG_PSNR,  "PSNR (errors set in encoder)"},
        {AV_CODEC_FLAG_TRUNCATED,  "TRUNCATED (truncated frames)"},
        {AV_CODEC_FLAG_INTERLACED_DCT,  "INTERLACED DCT (interlaced dct)"},
        {AV_CODEC_FLAG_LOW_DELAY,  "LOW DELAY (force low delay)"},
        {AV_CODEC_FLAG_GLOBAL_HEADER,  "GLOBAL HEADER (global header in extra data)"},
        {AV_CODEC_FLAG_BITEXACT,  "BITEXACT (only bit exact data)"},
        {AV_CODEC_FLAG_AC_PRED,  "AC PRED (H263 adv. intracoding / prediction)"},
        {AV_CODEC_FLAG_INTERLACED_ME,  "INTERLACED ME (interlacted motion estimate)"},
        {AV_CODEC_FLAG_CLOSED_GOP,  "CLOSED GOP (closed gop)"}
    },
    flag2Options{
        {AV_CODEC_FLAG2_FAST,  "fast"},
        {AV_CODEC_FLAG2_NO_OUTPUT,  "no output"},
        {AV_CODEC_FLAG2_LOCAL_HEADER,  "local header"},
        {AV_CODEC_FLAG2_DROP_FRAME_TIMECODE,  "drop frame timecode"},
        {AV_CODEC_FLAG2_CHUNKS,  "chunks"},
        {AV_CODEC_FLAG2_IGNORE_CROP,  "ignore crop"},
        {AV_CODEC_FLAG2_SHOW_ALL,  "show all"},
        {AV_CODEC_FLAG2_EXPORT_MVS,  "export mvs"},
        {AV_CODEC_FLAG2_SKIP_MANUAL,  "skip manual"},
        {AV_CODEC_FLAG2_RO_FLUSH_NOOP,  "ro flush noop"}
    },
    flag2Desc{
        {AV_CODEC_FLAG2_FAST,  "FAST (enable speedups)"},
        {AV_CODEC_FLAG2_NO_OUTPUT,  "NO OUTPUT (skip bitstream encode)"},
        {AV_CODEC_FLAG2_LOCAL_HEADER,  "LOCAL HEADER (global headers in keyframe)"},
        {AV_CODEC_FLAG2_DROP_FRAME_TIMECODE,  "DROP FRAME TIMECODE (deprecated)"},
        {AV_CODEC_FLAG2_CHUNKS,  "CHUNKS (truncated frames)"},
        {AV_CODEC_FLAG2_IGNORE_CROP,  "IGNORE CROP (ignore frame cropping data)"},
        {AV_CODEC_FLAG2_SHOW_ALL,  "SHOW ALL (show all before 1st keyframe)"},
        {AV_CODEC_FLAG2_EXPORT_MVS,  "EXPORT MVS (export motion vector sidedata)"},
        {AV_CODEC_FLAG2_SKIP_MANUAL,  "SKIP MANUAL (export skip motion as side data)"},
        {AV_CODEC_FLAG2_RO_FLUSH_NOOP,  "RO FLUSH NOOP (don't reset ASS on flush (SUBTITLES))"}
    }
{
}