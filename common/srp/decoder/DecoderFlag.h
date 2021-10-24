#ifndef __DECODERFLAG_H__
#define __DECODERFLAG_H__

#include <unordered_map>
#include <string>
#include <vector>
extern "C"
{
    #include <libavcodec/avcodec.h>
}

namespace decoder
{
    std::unordered_map<int32_t, std::string> const flags1ToStrings(int32_t flags);
    std::unordered_map<int32_t, std::string> const flags1ToDescriptions(int32_t flags);
    int32_t const parseFlags1Strings(std::vector<std::string> const & strings);

    std::unordered_map<int32_t, std::string> const flags2ToStrings(int32_t flags);
    std::unordered_map<int32_t, std::string> const flags2ToDescriptions(int32_t flags);
    int32_t const parseFlags2Strings(std::vector<std::string> const & strings);

    static std::unordered_map<int32_t, std::string> const flag1Options
    {
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
    };

    static std::unordered_map<int32_t, std::string> const flag1Desc
    {
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
    };

    static std::unordered_map<int32_t, std::string> const flag2Options
    {
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
    };

    static std::unordered_map<int32_t, std::string> const flag2Desc
    {
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
    };
}

#endif
