#include "DecoderFlag.h"
#include "../utils/FlagMapUtils.h"

namespace decoder
{
    std::unordered_map<int32_t, std::string> const flags1ToStrings(int32_t flags)
    {
        return bitflagutil::toStrings(flags, flag1Options);
    }

    std::unordered_map<int32_t, std::string> const flags1ToDescriptions(int32_t flags)
    {
        return bitflagutil::toStrings(flags, flag1Desc);
    }
    
    int32_t const parseFlags1Strings(std::vector<std::string> const & strings)
    {
        return bitflagutil::parseFlagStrings(strings, flag1Options);
    }

    std::unordered_map<int32_t, std::string> const flags2ToStrings(int32_t flags)
    {
        return bitflagutil::toStrings(flags, flag2Options);
    }

    std::unordered_map<int32_t, std::string> const flags2ToDescriptions(int32_t flags)
    {
        return bitflagutil::toStrings(flags, flag2Desc);
    }
    
    int32_t const parseFlags2Strings(std::vector<std::string> const & strings)
    {
        return bitflagutil::parseFlagStrings(strings, flag2Options);
    }
}
