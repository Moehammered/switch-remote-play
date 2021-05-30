#include "DecoderFlag.h"
#include "../utils/FlagMapUtils.h"

namespace decoder
{
    std::unordered_map<int32_t, std::string> const Flags1ToStrings(int32_t flags)
    {
        return ToStrings(flags, flag1Options);
    }

    std::unordered_map<int32_t, std::string> const Flags1ToDescriptions(int32_t flags)
    {
        return ToStrings(flags, flag1Desc);
    }
    
    int32_t const ParseFlags1Strings(std::vector<std::string> const & strings)
    {
        return ParseFlagStrings(strings, flag1Options);
    }

    std::unordered_map<int32_t, std::string> const Flags2ToStrings(int32_t flags)
    {
        return ToStrings(flags, flag2Options);
    }

    std::unordered_map<int32_t, std::string> const Flags2ToDescriptions(int32_t flags)
    {
        return ToStrings(flags, flag2Desc);
    }
    
    int32_t const ParseFlags2Strings(std::vector<std::string> const & strings)
    {
        return ParseFlagStrings(strings, flag2Options);
    }
}
