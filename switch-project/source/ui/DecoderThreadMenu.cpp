#include "DecoderThreadMenu.h"
#include "../utils/Colours.h"
#include "../system/SoftwareKeyboard.h"
#include <algorithm>

DecoderThreadMenu::DecoderThreadMenu() : Menu(),
textElements{}, itemCursor{decoderUtils::ThreadProperties},
typeCursor{decoder::threadDesc}, threadCount{minThreadCount}
{
    title.value = "Decoder Thread Behaviour";
    title.y += 45;

    SetupText();
}

void DecoderThreadMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Up)
        --itemCursor;
    else if (kDown & HidNpadButton::HidNpadButton_Down)
        ++itemCursor;

    if(kDown & HidNpadButton::HidNpadButton_A)
    {
        ShiftParam(*itemCursor, 1);
        UpdateUI(*itemCursor);
    }
    else if(kDown & HidNpadButton::HidNpadButton_B)
    {
        ShiftParam(*itemCursor, -1);
        UpdateUI(*itemCursor);
    }
}

void DecoderThreadMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);

    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto const & t : textElements)
    {
        if(t.first == *itemCursor)
            t.second.Render(renderer, font, highlightColour);
        else
            t.second.Render(renderer, font);
    }
}

void DecoderThreadMenu::SetTypeFlag(int32_t flag)
{
    cycleMap(typeCursor, flag);
    UpdateUI(decoderUtils::DecoderThreadProp::Type);
}

void DecoderThreadMenu::SetThreadCount(int32_t count)
{
    threadCount = std::clamp(count, minThreadCount, maxThreadCount);
    UpdateUI(decoderUtils::DecoderThreadProp::Count);
}

int32_t DecoderThreadMenu::TypeFlag() const
{
    return typeCursor.KeyPair().first;
}

int32_t DecoderThreadMenu::ThreadCount() const
{
    return threadCount;
}

void DecoderThreadMenu::UpdateUI(decoderUtils::DecoderThreadProp prop)
{
    switch(prop)
    {
        case decoderUtils::DecoderThreadProp::Type:
            textElements[prop].value = *typeCursor;
            break;

        case decoderUtils::DecoderThreadProp::Count:
            textElements[prop].value = "Thread Count: " + std::to_string(threadCount);
            break;
    }
}

void DecoderThreadMenu::ShiftParam(decoderUtils::DecoderThreadProp prop, int direction)
{
    switch(prop)
    {
        case decoderUtils::DecoderThreadProp::Type:
            typeCursor += direction;
            break;

        case decoderUtils::DecoderThreadProp::Count:
            threadCount = keyboardNumber(minThreadCount, maxThreadCount, threadCount);
            break;
    }
}

void DecoderThreadMenu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;

    auto params = decoderUtils::ThreadProperties;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = colours::white;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}
