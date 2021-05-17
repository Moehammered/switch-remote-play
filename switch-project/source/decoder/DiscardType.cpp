#include "DiscardType.h"
#include "../utils/EnumMapper.h"

DiscardType::DiscardType()
    : discardOptions{
        { AVDiscard::AVDISCARD_DEFAULT, "discard default" },
        { AVDiscard::AVDISCARD_NONE, "discard none" },
        { AVDiscard::AVDISCARD_NONREF, "discard nonref" },
        { AVDiscard::AVDISCARD_BIDIR, "discard bidir" },
        { AVDiscard::AVDISCARD_NONINTRA, "discard nonintra" },
        { AVDiscard::AVDISCARD_NONKEY, "discard nonkey" },
        { AVDiscard::AVDISCARD_ALL, "discard all" }
    },
    discardDesc{
        { AVDiscard::AVDISCARD_DEFAULT, "Discard Default (useless packets like 0 size packets)" },
        { AVDiscard::AVDISCARD_NONE, "Discard None" },
        { AVDiscard::AVDISCARD_NONREF, "Discard Non-ref (all non reference)" },
        { AVDiscard::AVDISCARD_BIDIR, "Discard Bi-dir (all bidirectional frames)" },
        { AVDiscard::AVDISCARD_NONINTRA, "Discard Non-intra (all non intra frames)" },
        { AVDiscard::AVDISCARD_NONKEY, "Discard Non-key (all frames except keyframes)" },
        { AVDiscard::AVDISCARD_ALL, "Discard All" }
    }
{
}

std::unordered_map<AVDiscard, std::string> const& DiscardType::Options() const
{
    return discardOptions;
}

std::unordered_map<AVDiscard, std::string> const& DiscardType::Options() const
{
    return discardDesc;
}

std::string const DiscardType::ToString(AVDiscard type) const
{
    return enumToStr(discardOptions, type);
}

AVDiscard DiscardType::Value(std::string const type) const
{
    return strToEnum(discardOptions, type);
}

std::string const DiscardType::ToDescription(AVDiscard type) const
{
    return enumToStr(discardDesc, type);
}
