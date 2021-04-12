#ifndef __ENUMMAPTEMPLATE_H__
#define __ENUMMAPTEMPLATE_H__

#include <unordered_map>
#include <string>

template<typename e>
e strToEnum(std::unordered_map<const char*, e> const & map, const char* key)
{
    auto pair = map.find(key);
    if(pair != map.end())
        return pair->second;
    else
        return map.begin()->second;
}

template<typename e>
std::string enumToStr(std::unordered_map<const char*, e> const & map, e enumeration)
{
    for(auto const & pair : map)
    {
        if(pair.second == enumeration)
            return pair.first;
    }

    return map.begin()->first;
}

#endif