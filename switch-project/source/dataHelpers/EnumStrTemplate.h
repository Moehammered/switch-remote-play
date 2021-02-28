#ifndef __ENUMSTRTEMPLATE_H__
#define __ENUMSTRTEMPLATE_H__

#include <unordered_map>
#include <string>

template<typename e>
e strToEnum(std::unordered_map<e, std::string> const & map, std::string s)
{
    for(auto const & item : map)
    {
        if(item.second == s)
            return item.first;
    }
    return (e)0;
}

template<typename e>
std::string enumToStr(std::unordered_map<e, std::string> const & map, e val)
{
    auto item = map.find(val);
    if(item != map.end())
        return item->second;
    else
        return map.begin()->second;
}

#endif
