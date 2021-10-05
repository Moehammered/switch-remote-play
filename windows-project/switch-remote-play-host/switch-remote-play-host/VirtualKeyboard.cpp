#include "VirtualKeyboard.h"
#include <algorithm>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinUser.h>

VirtualKeyboard::VirtualKeyboard()
    : pressedKeys{}, nextKeysPressed{}, nextKeysReleased{}
{
}

void VirtualKeyboard::Press(uint8_t const virtualKey)
{
    auto entry = std::find(nextKeysPressed.begin(), nextKeysPressed.end(), virtualKey);
    if (entry == nextKeysPressed.end())
        nextKeysPressed.push_back(virtualKey);
}

void VirtualKeyboard::Press(std::vector<uint8_t> const& virtualKeys)
{
    for (auto const& key : virtualKeys)
        Press(key);
}

void VirtualKeyboard::Release(uint8_t const virtualKey)
{
    auto entry = std::find(nextKeysReleased.begin(), nextKeysReleased.end(), virtualKey);
    if (entry == nextKeysReleased.end())
        nextKeysReleased.push_back(virtualKey);
}

void VirtualKeyboard::Release(std::vector<uint8_t> const& virtualKeys)
{
    for (auto const& key : virtualKeys)
        Release(key);
}

void VirtualKeyboard::Release()
{
    for (auto const& key : pressedKeys)
        Release(key);

    nextKeysPressed.clear();
}

std::vector<uint8_t> const extendedKeys
{
    VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT,
    VK_MENU, VK_HOME, VK_SHIFT, VK_CONTROL,
    VK_END, VK_PRIOR, VK_NEXT, VK_INSERT, VK_DELETE
};

bool isExtendedKey(uint8_t const key)
{
    auto entry = std::find(extendedKeys.begin(), extendedKeys.end(), key);
    return entry != extendedKeys.end();
}

void VirtualKeyboard::Commit()
{
    std::vector<INPUT> inputQueue;
    for (auto const& key : nextKeysPressed)
    {
        //append to queue of inputs
        auto input = INPUT{ 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = key;
        input.ki.wVk = key;
        //input.ki.dwFlags = isExtendedKey(key) ? KEYEVENTF_EXTENDEDKEY : 0;
        input.ki.dwFlags = 0;
        inputQueue.emplace_back(input);

        auto entry = std::find(pressedKeys.begin(), pressedKeys.end(), key);
        if (entry == pressedKeys.end())
            pressedKeys.push_back(key);
    }

    auto releasedKeys = std::vector<uint8_t>{};

    for (auto const& key : nextKeysReleased)
    {
        auto input = INPUT{ 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wScan = key;
        input.ki.wVk = key;
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        /*if (isExtendedKey(key))
            input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;*/

        inputQueue.emplace_back(input);

        releasedKeys.emplace_back(key);
    }

    if (inputQueue.size() != 0)
    {
        auto const inputSize = sizeof(INPUT);
        auto sent = 0;
        do
        {
            sent = SendInput(inputQueue.size(), inputQueue.data(), inputSize);
        } while (sent != inputQueue.size());
    }

    nextKeysPressed.clear();
    nextKeysReleased.clear();

    if (releasedKeys.size() != 0 && pressedKeys.size() != 0)
    {
        auto keyWasReleased = [&](uint8_t const key)
        {
            auto entry = std::find(releasedKeys.begin(), releasedKeys.end(), key);
            return entry != releasedKeys.end();
        };

        auto newEnd = std::remove_if(pressedKeys.begin(), pressedKeys.end(), keyWasReleased);
        pressedKeys.erase(newEnd, pressedKeys.end());
    }
}
