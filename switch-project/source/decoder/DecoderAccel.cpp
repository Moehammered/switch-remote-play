#include "DecoderAccel.h"
#include "../utils/EnumMapper.h"
#include "../utils/FlagMapUtils.h"
#include <algorithm>
#include <numeric>

namespace decoder
{
    std::unordered_map<int32_t, std::string> const AccelFlagsToStrings(int32_t flags)
    {
        return ToStrings(flags, accelOptions);
    }

    std::unordered_map<int32_t, std::string> const AccelFlagsToDescriptions(int32_t flags)
    {
        return ToStrings(flags, accelDesc);
    }

    int32_t const ParseAccelFlagStrings(std::vector<std::string> const & strings)
    {
        return ParseFlagStrings(strings, accelOptions);
    }
}
