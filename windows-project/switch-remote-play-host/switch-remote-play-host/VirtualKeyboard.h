#pragma once

#include <stdint.h>
#include <vector>

class VirtualKeyboard
{
public:
    VirtualKeyboard();

    void Press(uint8_t const virtualKey);
    void Press(std::vector<uint8_t> const & virtualKeys);

    void Release(uint8_t const virtualKey);
    void Release(std::vector<uint8_t> const& virtualKeys);
    void Release();

    void Commit();

private:
    std::vector<uint8_t> pressedKeys;
    std::vector<uint8_t> nextKeysPressed;

    //std::vector<uint8_t> releasedKeys;
    std::vector<uint8_t> nextKeysReleased;
};