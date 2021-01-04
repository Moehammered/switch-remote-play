#include "SwitchButtons.h"
#include <unordered_map>
#include <switch/services/hid.h>

std::string SwitchButtonToString(uint32_t btn)
{
    std::unordered_map<uint32_t, std::string> map{
        {KEY_L, "L"},
        {KEY_R, "R"},
        {KEY_ZL, "ZL"},
        {KEY_ZR, "ZR"}
    };

    if(map.find(btn) != map.end())
        return map[btn];
    else
        return "unknown";
}

uint32_t ParseSwitchButtonString(std::string s)
{
    std::unordered_map<std::string, uint32_t> map{
        {"L", KEY_L},
        {"R", KEY_R},
        {"ZL", KEY_ZL},
        {"ZR", KEY_ZR}
    };

    if(map.find(s) != map.end())
        return map[s];
    else
        return 0;
}