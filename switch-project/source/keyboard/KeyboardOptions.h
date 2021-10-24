#ifndef __KEYBOARDOPTIONS_H__
#define __KEYBOARDOPTIONS_H__

#include "../controller/SwitchButtons.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace keyboard
{
    enum class KeyParameter : int32_t
    {
        Control,
        OSKey,
        Alt,
        Shift,
        Tab,
        Escape,
        Space,
        Backspace,
        Enter,
        F4,
        Home,
        End,
        PageUp,
        PageDown,
        UpArrow,
        DownArrow,
        LeftArrow,
        RightArrow
    };

    auto constexpr totalSupportedKeys = (size_t)KeyParameter::RightArrow + 1;

    static std::vector<KeyParameter> const keyParamsList
    {
        KeyParameter::Control, KeyParameter::OSKey, KeyParameter::Alt,
        KeyParameter::Shift, KeyParameter::Tab, KeyParameter::Escape,
        KeyParameter::Space, KeyParameter::Backspace, KeyParameter::Enter,
        KeyParameter::F4, KeyParameter::Home, KeyParameter::End,
        KeyParameter::PageUp, KeyParameter::PageDown,
        KeyParameter::UpArrow, KeyParameter::DownArrow,
        KeyParameter::LeftArrow, KeyParameter::RightArrow
    };

    static std::unordered_map<KeyParameter, std::string> const keyParamsDesc
    {
        { KeyParameter::Control, "Control" },
        { KeyParameter::OSKey, "OS Key" },
        { KeyParameter::Alt, "Alt" },
        { KeyParameter::Shift, "Shift" },
        { KeyParameter::Tab, "Tab" },
        { KeyParameter::Escape, "Escape" },
        { KeyParameter::Space, "Space" },
        { KeyParameter::Backspace, "Backspace" },
        { KeyParameter::Enter, "Enter" },
        { KeyParameter::Home, "Home" },
        { KeyParameter::F4, "F4" },
        { KeyParameter::End, "End" },
        { KeyParameter::PageUp, "PageUp" },
        { KeyParameter::PageDown, "PageDown" },
        { KeyParameter::UpArrow, "UpArrow" },
        { KeyParameter::DownArrow, "DownArrow" },
        { KeyParameter::LeftArrow, "LeftArrow" },
        { KeyParameter::RightArrow, "RightArrow" }
    };

    struct alignas(4) KeyboardBinding
    {
        KeyParameter  key;
        uint32_t button;
    };

    auto constexpr keyboardBindingSize = sizeof(KeyboardBinding);

    KeyboardBinding const defaultBindings[] = 
    {
        { KeyParameter::OSKey, HidNpadButton::HidNpadButton_Y },
        { KeyParameter::Escape, HidNpadButton::HidNpadButton_X },
        { KeyParameter::Enter, HidNpadButton::HidNpadButton_A },
        { KeyParameter::Backspace, HidNpadButton::HidNpadButton_B },
        { KeyParameter::UpArrow, HidNpadButton::HidNpadButton_Up },
        { KeyParameter::DownArrow, HidNpadButton::HidNpadButton_Down },
        { KeyParameter::LeftArrow, HidNpadButton::HidNpadButton_Left },
        { KeyParameter::RightArrow, HidNpadButton::HidNpadButton_Right }
    };

    struct alignas(4) KeyboardConfig
    {
        KeyboardBinding bindings[totalSupportedKeys];
    };

    auto constexpr keyboardConfigSize = sizeof(KeyboardConfig);

    std::unordered_map<KeyParameter, std::string> const keybindingsToString(KeyboardConfig const config);
    KeyboardConfig const keybindingsFromStr(std::unordered_map<KeyParameter, std::string> const & map);
}

#endif