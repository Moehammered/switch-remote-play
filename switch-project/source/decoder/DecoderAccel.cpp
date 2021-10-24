#include "DecoderAccel.h"
#include "../utils/EnumMapper.h"
#include "../utils/FlagMapUtils.h"

namespace decoder
{
    std::unordered_map<int32_t, std::string> const accelFlagsToStrings(int32_t flags)
    {
        return bitflagutil::toStrings(flags, accelOptions);
    }

    std::unordered_map<int32_t, std::string> const accelFlagsToDescriptions(int32_t flags)
    {
        return bitflagutil::toStrings(flags, accelDesc);
    }

    int32_t const parseAccelFlagStrings(std::vector<std::string> const & strings)
    {
        return bitflagutil::parseFlagStrings(strings, accelOptions);
    }
}
