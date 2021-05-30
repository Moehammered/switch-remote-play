#ifndef __DISCARDTYPE_H__
#define __DISCARDTYPE_H__

#include <unordered_map>
#include <string>

extern "C"
{
    #include <libavcodec/avcodec.h>
}

namespace decoder
{
    std::string const AVDiscardToString(AVDiscard type);
    std::string const AVDiscardToDescription(AVDiscard type);
    AVDiscard ParseAVDiscardString(std::string const type);

    static std::unordered_map<AVDiscard, std::string> const discardOptions
    {
        { AVDiscard::AVDISCARD_DEFAULT, "discard default" },
        { AVDiscard::AVDISCARD_NONE, "discard none" },
        { AVDiscard::AVDISCARD_NONREF, "discard nonref" },
        { AVDiscard::AVDISCARD_BIDIR, "discard bidir" },
        { AVDiscard::AVDISCARD_NONINTRA, "discard nonintra" },
        { AVDiscard::AVDISCARD_NONKEY, "discard nonkey" },
        { AVDiscard::AVDISCARD_ALL, "discard all" }
    };

    static std::unordered_map<AVDiscard, std::string> const discardDesc
    {
        { AVDiscard::AVDISCARD_DEFAULT, "Discard Default (useless packets like 0 size packets)" },
        { AVDiscard::AVDISCARD_NONE, "Discard None" },
        { AVDiscard::AVDISCARD_NONREF, "Discard Non-ref (all non reference)" },
        { AVDiscard::AVDISCARD_BIDIR, "Discard Bi-dir (all bidirectional frames)" },
        { AVDiscard::AVDISCARD_NONINTRA, "Discard Non-intra (all non intra frames)" },
        { AVDiscard::AVDISCARD_NONKEY, "Discard Non-key (all frames except keyframes)" },
        { AVDiscard::AVDISCARD_ALL, "Discard All" }
    };
}

#endif
