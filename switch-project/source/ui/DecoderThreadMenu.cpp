#include "DecoderThreadMenu.h"

DecoderThreadMenu::DecoderThreadMenu() : Menu(),
textElements{}, itemCursor{decoderUtils::ThreadProperties},
type{}, typeCursor{type.Descriptions()}, threadCounter{DefaultThreadCount}
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

int32_t DecoderThreadMenu::TypeFlag() const
{
    return typeCursor.KeyPair().first;
}

int32_t DecoderThreadMenu::ThreadCount() const
{
    return *threadCounter;
}

void DecoderThreadMenu::UpdateUI(decoderUtils::DecoderThreadProp prop)
{
    switch(prop)
    {
        case decoderUtils::DecoderThreadProp::Type:
        {
            textElements[prop].value = *typeCursor;
        }
        break;

        case decoderUtils::DecoderThreadProp::Count:
        {
            textElements[prop].value = "Thread Count: " + std::to_string(*threadCounter);
        }
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
            threadCounter += direction;
        break;
    }
}

void DecoderThreadMenu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = decoderUtils::ThreadProperties;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}
