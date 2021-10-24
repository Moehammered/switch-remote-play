#include "SwitchButtons.h"
#include "../utils/EnumMapper.h"

namespace controller
{
    std::string analogStickToString(AnalogStick stick)
    {
        return enumToStr(analogStickOptions, stick);
    }

    AnalogStick parseAnalogStickString(std::string s)
    {
        return strToEnum(analogStickOptions, s);
    }

    std::string switchButtonToString(HidNpadButton btn)
    {
        return enumToStr(switchButtonOptions, btn);
    }

    HidNpadButton parseSwitchButtonString(std::string s)
    {
        return strToEnum(switchButtonOptions, s);
    }

    std::vector<std::string> const switchButtonsToString(uint64_t const keys)
    {
        auto btns = std::vector<std::string>{};

        for(auto const & entry : switchButtonOptions)
        {
            auto const btnMask = entry.first & keys;
            if(entry.first == btnMask)
                btns.push_back(switchButtonToString(entry.first));
        }

        return btns;
    }

    std::vector<HidNpadButton> const parseSwitchButtonStrings(std::vector<std::string> const & strings)
    {
        auto btns = std::vector<HidNpadButton>{};

        for(auto const & entry : strings)
            btns.push_back(parseSwitchButtonString(entry));

        return btns;
    }
}
