#ifndef __KEYBOARDSTREAM_H__
#define __KEYBOARDSTREAM_H__

#include "VirtualKeyboard.h"
#include "srp/network/NetworkData.h"
#include "srp/keyboard/KeyboardOptions.h"
#include <unordered_map>

class KeyboardStream
{
public:
    KeyboardStream(keyboard::KeyboardConfig const config, int64_t const buttonHeldTime);

    void Process(InputDataPayload const& inputData, int64_t const nanoDelta);

    void Reset();

private:
    keyboard::KeyboardConfig const settings;
    std::unordered_map<uint32_t, uint16_t> const keyboardMap;
    std::unordered_map<uint32_t, int64_t> buttonTimers;
    int64_t const buttonHeldTime;
    VirtualKeyboard keyboard;

    std::unordered_map<uint32_t, uint16_t> const MapKeyParams(keyboard::KeyboardConfig const& config);
    void ProcessVirtualKeyboard(VirtualKeyboard& keyboard,
                                GamepadDataPayload const& padData,
                                int64_t const deltaTime);
};

#endif
