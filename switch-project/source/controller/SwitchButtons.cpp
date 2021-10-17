#include "SwitchButtons.h"
#include "../utils/EnumMapper.h"

namespace controller
{
    std::string AnalogStickToString(AnalogStick stick)
    {
        return enumToStr(analogStickOptions, stick);
    }

    AnalogStick ParseAnalogStickString(std::string s)
    {
        return strToEnum(analogStickOptions, s);
    }

    std::string SwitchButtonToString(HidNpadButton btn)
    {
        return enumToStr(switchButtonOptions, btn);
    }

    HidNpadButton ParseSwitchButtonString(std::string s)
    {
        return strToEnum(switchButtonOptions, s);
    }

    std::vector<std::string> const SwitchButtonsToString(uint64_t const keys)
    {
        auto btns = std::vector<std::string>{};

        for(auto const & entry : switchButtonOptions)
        {
            auto const btnMask = entry.first & keys;
            if(entry.first == btnMask)
                btns.push_back(SwitchButtonToString(entry.first));
        }

        return btns;
    }

    std::vector<HidNpadButton> const ParseSwitchButtonStrings(std::vector<std::string> const & strings)
    {
        auto btns = std::vector<HidNpadButton>{};

        for(auto const & entry : strings)
            btns.push_back(ParseSwitchButtonString(entry));

        return btns;
    }
}
