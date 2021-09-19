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
}
