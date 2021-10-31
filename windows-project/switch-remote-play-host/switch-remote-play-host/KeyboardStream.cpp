#include "KeyboardStream.h"
#include "KeyboardEnumUtil.h"

KeyboardStream::KeyboardStream(keyboard::KeyboardConfig const config, int64_t const buttonHeldTime)
    : settings{ config },
    keyboardMap{ MapKeyParams(config) },
    buttonTimers{},
    buttonHeldTime{ buttonHeldTime }
{
}

void KeyboardStream::Process(InputDataPayload const& inputData, int64_t const nanoDelta)
{
    ProcessVirtualKeyboard(keyboard, inputData.gamepads[0], nanoDelta);
    keyboard.Commit();
}

void KeyboardStream::Reset()
{
    keyboard.Release();
    keyboard.Commit();
}

std::unordered_map<uint32_t, uint16_t> const KeyboardStream::MapKeyParams(keyboard::KeyboardConfig const& config)
{
    std::unordered_map<uint32_t, uint16_t> mapping{};

    for (auto const& binding : config.bindings)
    {
        if (binding.button != 0)
            mapping[binding.button] = KeyParamToVirtualCode(binding.key);
    }

    return mapping;
}

void KeyboardStream::ProcessVirtualKeyboard(VirtualKeyboard& keyboard, GamepadDataPayload const& padData, int64_t const deltaTime)
{
    auto pressed = std::vector<uint8_t>{};
    auto released = std::vector<uint8_t>{};

    for (auto const& entry : keyboardMap)
    {
        auto const heldTime = buttonTimers[entry.first];

        auto const btnMask = entry.first & padData.keys;

        if (entry.first == btnMask)
        {
            if (heldTime == 0 || heldTime > buttonHeldTime)
                pressed.push_back(entry.second);

            buttonTimers[entry.first] += deltaTime;
        }
        else
        {
            if (heldTime != 0)
                released.push_back(entry.second);
            buttonTimers[entry.first] = 0;
        }
    }

    keyboard.Press(pressed);
    keyboard.Release(released);
}
