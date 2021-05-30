#include "SwitchButtons.h"
#include "../utils/EnumMapper.h"

namespace controller
{
    std::string SwitchButtonToString(HidNpadButton btn)
    {
        return enumToStr(switchButtonOptions, btn);
    }

    HidNpadButton ParseSwitchButtonString(std::string s)
    {
        return strToEnum(switchButtonOptions, s);
    }
}
