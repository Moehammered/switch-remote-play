#ifndef __ENUMMAPPER_H__
#define __ENUMMAPPER_H__

#include <unordered_map>
#include <string>

template<typename e>
std::string enumToStr(std::unordered_map<std::string, e> const& map, e val)
{
    if (map.size() == 0)
        return std::string{};
    else
    {
        for (auto const& pair : map)
        {
            if (pair.second == val)
                return pair.first;
        }

        return map.begin()->first;
    }
}

template<typename e>
std::string enumToStr(std::unordered_map<e, std::string> const& map, e val)
{
    if (map.size() == 0)
        return std::string{};
    else
    {
        auto pair = map.find(val);
        if (pair != map.end())
            return pair->second;
        else
            return map.begin()->second;
    }
}

template<typename e>
e strToEnum(std::unordered_map<std::string, e> const& map, std::string str)
{
    if (map.size() == 0)
        return (e)0;
    else
    {
        auto pair = map.find(str);
        if (pair != map.end())
            return pair->second;
        else
            return map.begin()->second;
    }
}

template<typename e>
e strToEnum(std::unordered_map<e, std::string> const& map, std::string str)
{
    if (map.size() == 0)
        return (e)0;
    else
    {
        for (auto const& pair : map)
        {
            if (pair.second == str)
                return pair.first;
        }

        return map.begin()->first;
    }
}

#endif
