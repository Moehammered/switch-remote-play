#include "KeyboardEnumUtil.h"
#include <unordered_map>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinUser.h>

namespace 
{
    using namespace keyboard;
    std::unordered_map<KeyParameter, uint16_t> const VirtualCodeMap
    {
        { KeyParameter::Control, VK_CONTROL },
        { KeyParameter::OSKey,  VK_LWIN },
        { KeyParameter::Alt, VK_MENU },
        { KeyParameter::Shift, VK_SHIFT },
        { KeyParameter::Tab, VK_TAB },
        { KeyParameter::Escape, VK_ESCAPE },
        { KeyParameter::Space, VK_SPACE },
        { KeyParameter::Backspace, VK_BACK },
        { KeyParameter::Enter, VK_RETURN },
        { KeyParameter::F4, VK_F4 },
        { KeyParameter::Home, VK_HOME },
        { KeyParameter::End, VK_END },
        { KeyParameter::PageUp, VK_PRIOR },
        { KeyParameter::PageDown, VK_NEXT },
        { KeyParameter::UpArrow, VK_UP },
        { KeyParameter::DownArrow, VK_DOWN },
        { KeyParameter::LeftArrow, VK_LEFT },
        { KeyParameter::RightArrow, VK_RIGHT }
    };
}

uint16_t const KeyParamToVirtualCode(keyboard::KeyParameter const key)
{
    auto entry = VirtualCodeMap.find(key);
    if (entry != VirtualCodeMap.end())
        return entry->second;
    else
        return 0;
}
