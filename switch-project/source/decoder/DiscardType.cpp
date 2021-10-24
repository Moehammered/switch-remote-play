#include "DiscardType.h"
#include "../utils/EnumMapper.h"

namespace decoder
{
    std::string const avDiscardToString(AVDiscard type)
    {
        return enumToStr(discardOptions, type);
    }

    std::string const avDiscardToDescription(AVDiscard type)
    {
        return enumToStr(discardDesc, type);
    }

    AVDiscard parseAVDiscardString(std::string const type)
    {
        return strToEnum(discardOptions, type);
    }
}
