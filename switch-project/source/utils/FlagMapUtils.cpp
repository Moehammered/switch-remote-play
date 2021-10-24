#include "FlagMapUtils.h"
#include <algorithm>

namespace bitflagutil
{
    std::unordered_map<int32_t, std::string> const toStrings(int32_t flags, std::unordered_map<int32_t, std::string> const & map)
    {
        auto found = std::unordered_map<int32_t, std::string>{};

        for(auto const & pair : map)
        {
            if(flags & pair.first)
                found[pair.first] = pair.second;
        }

        return found;
    }

    int32_t const parseFlagStrings(std::vector<std::string> const & values, std::unordered_map<int32_t, std::string> const & map)
    {
        int32_t flags = 0;

        auto checkString = [](auto const & str, auto const & pair)
        {
            return str == pair.second;
        };

        for(auto const & value : values)
        {
            auto itr = std::find_if(map.begin(), map.end(), [&](auto const & pair){
                return checkString(value, pair);
            });

            if(itr != map.end())
                flags |= itr->first;
        }

        return flags;
    }
}