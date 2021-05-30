#include "DiscardType.h"
#include "../utils/EnumMapper.h"

namespace decoder
{
    std::string const AVDiscardToString(AVDiscard type)
    {
        return enumToStr(discardOptions, type);
    }

    std::string const AVDiscardToDescription(AVDiscard type)
    {
        return enumToStr(discardDesc, type);
    }

    AVDiscard ParseAVDiscardString(std::string const type)
    {
        return strToEnum(discardOptions, type);
    }
}
