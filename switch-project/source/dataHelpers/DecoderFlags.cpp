#include "DecoderFlags.h"

std::unordered_map<int32_t, std::string> AllDecoderFlag1Descriptions()
{
    // done this way to deal with narrowing conversion compile error
    auto allFlagged {
          AV_CODEC_FLAG_UNALIGNED | AV_CODEC_FLAG_QSCALE
        | AV_CODEC_FLAG_4MV | AV_CODEC_FLAG_OUTPUT_CORRUPT
        | AV_CODEC_FLAG_QPEL | AV_CODEC_FLAG_DROPCHANGED
        | AV_CODEC_FLAG_PASS1 | AV_CODEC_FLAG_PASS2
        | AV_CODEC_FLAG_LOOP_FILTER | AV_CODEC_FLAG_GRAY
        | AV_CODEC_FLAG_PSNR | AV_CODEC_FLAG_TRUNCATED
        | AV_CODEC_FLAG_INTERLACED_DCT | AV_CODEC_FLAG_LOW_DELAY
        | AV_CODEC_FLAG_GLOBAL_HEADER | AV_CODEC_FLAG_BITEXACT
        | AV_CODEC_FLAG_AC_PRED | AV_CODEC_FLAG_INTERLACED_ME
        | AV_CODEC_FLAG_CLOSED_GOP
    };

    int32_t const * reinterp = (int32_t*)&allFlagged;
    return DecoderFlags1Descriptions(*reinterp);
}

std::unordered_map<int32_t, std::string> DecoderFlags1Descriptions(int32_t flags)
{
    std::unordered_map<int32_t, std::string> found{};
    std::unordered_map<int, std::string> flagToString{
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

    for(auto item : flagToString)
    {
        if(flags & item.first)
            found[item.first] = item.second;
    }

    return found;
}

std::unordered_map<int32_t, std::string> DecoderFlags1ToStrings(int32_t flags)
{
    std::unordered_map<int32_t, std::string> found{};
    std::unordered_map<int, std::string> flagToString{
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

    for(auto item : flagToString)
    {
        if(flags & item.first)
            found[item.first] = item.second;
    }

    return found;
}

int32_t ParseDecoderFlags1Strings(std::vector<std::string> const & flagStrings)
{
    int32_t flags = 0;

    std::unordered_map<std::string, int32_t> stringToFlag{
        {"unaligned", AV_CODEC_FLAG_UNALIGNED},
        {"qscale", AV_CODEC_FLAG_QSCALE},
        {"4mv", AV_CODEC_FLAG_4MV},
        {"output corrupt", AV_CODEC_FLAG_OUTPUT_CORRUPT},
        {"qpel", AV_CODEC_FLAG_QPEL},
        {"drop changed", AV_CODEC_FLAG_DROPCHANGED},
        {"pass1", AV_CODEC_FLAG_PASS1},
        {"pass2", AV_CODEC_FLAG_PASS2},
        {"loop filter", AV_CODEC_FLAG_LOOP_FILTER},
        {"gray", AV_CODEC_FLAG_GRAY},
        {"psnr", AV_CODEC_FLAG_PSNR},
        {"truncated", AV_CODEC_FLAG_TRUNCATED},
        {"interlaced dct", AV_CODEC_FLAG_INTERLACED_DCT},
        {"low delay", AV_CODEC_FLAG_LOW_DELAY},
        {"global header", AV_CODEC_FLAG_GLOBAL_HEADER},
        {"bitexact", AV_CODEC_FLAG_BITEXACT},
        {"ac pred", AV_CODEC_FLAG_AC_PRED},
        {"interlaced me", AV_CODEC_FLAG_INTERLACED_ME},
        {"closed gop", AV_CODEC_FLAG_CLOSED_GOP}
    };

    for(auto& s : flagStrings)
    {
        if(stringToFlag.find(s) != stringToFlag.end())
            flags |= stringToFlag[s];
    }

    return flags;
}

std::unordered_map<int32_t, std::string> AllDecoderFlag2Descriptions()
{
    int32_t constexpr allFlagged = {
        AV_CODEC_FLAG2_FAST | AV_CODEC_FLAG2_NO_OUTPUT
        | AV_CODEC_FLAG2_LOCAL_HEADER | AV_CODEC_FLAG2_DROP_FRAME_TIMECODE
        | AV_CODEC_FLAG2_CHUNKS | AV_CODEC_FLAG2_IGNORE_CROP
        | AV_CODEC_FLAG2_SHOW_ALL | AV_CODEC_FLAG2_EXPORT_MVS
        | AV_CODEC_FLAG2_SKIP_MANUAL | AV_CODEC_FLAG2_RO_FLUSH_NOOP
    };

    return DecoderFlags2Descriptions(allFlagged);
}

std::unordered_map<int32_t, std::string> DecoderFlags2Descriptions(int32_t flags)
{
    std::unordered_map<int32_t, std::string> found{};
    std::unordered_map<int, std::string> flagToString{
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

    for(auto item : flagToString)
    {
        if(flags & item.first)
            found[item.first] = item.second;
    }

    return found;
}

std::unordered_map<int32_t, std::string> DecoderFlags2ToStrings(int32_t flags)
{
    std::unordered_map<int32_t, std::string> found{};
    std::unordered_map<int, std::string> flagToString{
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

    for(auto item : flagToString)
    {
        if(flags & item.first)
            found[item.first] = item.second;
    }

    return found;
}

int32_t ParseDecoderFlags2Strings(std::vector<std::string> const & flagStrings)
{
    int32_t flags = 0;

    std::unordered_map<std::string, int32_t> stringToFlag{
        {"fast", AV_CODEC_FLAG2_FAST},
        {"no output", AV_CODEC_FLAG2_NO_OUTPUT},
        {"local header", AV_CODEC_FLAG2_LOCAL_HEADER},
        {"drop frame timecode", AV_CODEC_FLAG2_DROP_FRAME_TIMECODE},
        {"chunks", AV_CODEC_FLAG2_CHUNKS},
        {"ignore crop", AV_CODEC_FLAG2_IGNORE_CROP},
        {"show all", AV_CODEC_FLAG2_SHOW_ALL},
        {"export mvs", AV_CODEC_FLAG2_EXPORT_MVS},
        {"skip manual", AV_CODEC_FLAG2_SKIP_MANUAL},
        {"ro flush noop", AV_CODEC_FLAG2_RO_FLUSH_NOOP}
    };

    for(auto& s : flagStrings)
    {
        if(stringToFlag.find(s) != stringToFlag.end())
            flags |= stringToFlag[s];
    }

    return flags;
}

std::unordered_map<int32_t, std::string> AllHWAccelFlagsDescriptions()
{
    int32_t constexpr allFlagged = {
        AV_HWACCEL_CODEC_CAP_EXPERIMENTAL | AV_HWACCEL_FLAG_IGNORE_LEVEL
        | AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH | AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH
    };

    return HWAccelFlagsDescriptions(allFlagged);
}

std::unordered_map<int32_t, std::string> HWAccelFlagsDescriptions(int32_t flags)
{
    std::unordered_map<int32_t, std::string> found{};
    std::unordered_map<int, std::string> flagToString{
        {AV_HWACCEL_CODEC_CAP_EXPERIMENTAL,  "EXPERIMENTAL - Allow experimental decoding methods"},
        {AV_HWACCEL_FLAG_IGNORE_LEVEL,  "IGNORE LEVEL - Ignore hardware support level"},
        {AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH,  "ALLOW HIGH DEPTH - Allow output greater than YUV420"},
        {AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH,  "ALLOW PROFILE MISMATCH - Attempt HW accel even if mismatched"}
    };

    for(auto item : flagToString)
    {
        if(flags & item.first)
            found[item.first] = item.second;
    }

    return found;
}

std::unordered_map<int32_t, std::string> HWAccelFlagsToStrings(int32_t flags)
{
    std::unordered_map<int32_t, std::string> found{};

    std::unordered_map<int, std::string> flagToString{
        {AV_HWACCEL_CODEC_CAP_EXPERIMENTAL,  "experimental"},
        {AV_HWACCEL_FLAG_IGNORE_LEVEL,  "ignore level"},
        {AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH,  "allow high depth"},
        {AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH,  "allow profile mismatch"}
    };

    for(auto item : flagToString)
    {
        if(flags & item.first)
            found[item.first] = item.second;
    }

    return found;
}

int32_t ParseHWAccelFlagStrings(std::vector<std::string> const & flagStrings)
{
    int32_t flags = 0;

    std::unordered_map<std::string, int32_t> stringToFlag{
        {"experimental", AV_HWACCEL_CODEC_CAP_EXPERIMENTAL},
        {"ignore level", AV_HWACCEL_FLAG_IGNORE_LEVEL},
        {"allow high depth", AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH},
        {"allow profile mismatch", AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH}
    };

    for(auto& s : flagStrings)
    {
        if(stringToFlag.find(s) != stringToFlag.end())
            flags |= stringToFlag[s];
    }

    return flags;
}

std::unordered_map<AVDiscard, std::string> AllAVDiscardDescriptions()
{
    return std::unordered_map<AVDiscard, std::string>{
        { AVDISCARD_NONE, "Discard None" },
        { AVDISCARD_DEFAULT, "Discard Default (useless packets like 0 size packets)" },
        { AVDISCARD_NONREF, "Discard Non-ref (all non reference)" },
        { AVDISCARD_BIDIR, "Discard Bi-dir (all bidirectional frames)" },
        { AVDISCARD_NONINTRA, "Discard Non-intra (all non intra frames)" },
        { AVDISCARD_NONKEY, "Discard Non-key (all frames except keyframes)" },
        { AVDISCARD_ALL, "Discard All" }
    };
}

std::string AVDiscardDescription(AVDiscard discard)
{
    switch(discard)
    {
        case AVDISCARD_NONE:
            return "Discard None";

        case AVDISCARD_DEFAULT: 
            return "Discard Default (useless packets like 0 size packets)";

        case AVDISCARD_NONREF:
            return "Discard Non-ref (all non reference)";

        case AVDISCARD_BIDIR:
            return "Discard Bi-dir (all bidirectional frames)";
        
        case AVDISCARD_NONINTRA: 
            return "Discard Non-intra (all non intra frames)";

        case AVDISCARD_NONKEY:
            return "Discard Non-key (all frames except keyframes)";

        case AVDISCARD_ALL:
            return "Discard All";

        default:
            return "unknown";
    }
}

std::string AVDiscardToString(AVDiscard discard)
{
    switch(discard)
    {
        case AVDISCARD_NONE:
            return "discard none";

        default:
        case AVDISCARD_DEFAULT: 
            return "discard default";

        case AVDISCARD_NONREF:
            return "discard nonref";

        case AVDISCARD_BIDIR:
            return "discard bidir";
        
        case AVDISCARD_NONINTRA: 
            return "discard nonintra";

        case AVDISCARD_NONKEY:
            return "discard nonkey";

        case AVDISCARD_ALL:
            return "discard all";
    }
}

AVDiscard ParseAVDiscardString(std::string s)
{
    if(s == "discard none")
        return AVDiscard::AVDISCARD_NONE;
    else if(s == "discard default")
        return AVDiscard::AVDISCARD_DEFAULT;
    else if(s == "discard nonref")
        return AVDiscard::AVDISCARD_NONREF;
    else if(s == "discard bidir")
        return AVDiscard::AVDISCARD_BIDIR;
    else if(s == "discard nonintra")
        return AVDiscard::AVDISCARD_NONINTRA;
    else if(s == "discard nonkey")
        return AVDiscard::AVDISCARD_NONKEY;
    else if(s == "discard all")
        return AVDiscard::AVDISCARD_ALL;
    else
        return AVDISCARD_DEFAULT;
}

std::string ThreadTypeDescription(int32_t type)
{
    switch(type)
    {
        case FF_THREAD_FRAME:
            return "Thread Frame (1 thread per frame - incurs a delay per thread)";

        case FF_THREAD_SLICE:
            return "Thread Slice (many threads per frame)";

        default:
            return "unknown";
    }
}

std::string ThreadTypeToString(int32_t type)
{
    switch(type)
    {
        case FF_THREAD_FRAME:
            return "thread frame";

        case FF_THREAD_SLICE:
            return "thread slice";

        default:
            return "unknown";
    }
}

int32_t ParseThreadTypeString(std::string s)
{
    if(s == "thread frame")
        return FF_THREAD_FRAME;
    else if(s == "thread slice")
        return FF_THREAD_SLICE;
    else
        return FF_THREAD_SLICE;
}
