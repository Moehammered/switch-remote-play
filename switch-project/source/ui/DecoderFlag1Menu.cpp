#include "DecoderFlag1Menu.h"
#include "../decoder/DecoderFlag.h"
#include "../decoder/DecoderOptions.h"

DecoderFlag1Menu::DecoderFlag1Menu() : Menu(),
    textElements{}, cursor{AvailableFlags1}, selected{}, flags{}
{
    title.value = "Decoder Flags 1 - Frame Behaviour";
    title.y += 45;

    selected = *cursor;
    SetupText();
}

void DecoderFlag1Menu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Up)
    {
        --cursor;
        selected = *cursor;
    }
    else if (kDown & HidNpadButton::HidNpadButton_Down)
    {
        ++cursor;
        selected = *cursor;
    }

    if(kDown & (HidNpadButton::HidNpadButton_A | HidNpadButton::HidNpadButton_B))
    {
        ToggleFlag(selected);
        UpdateUI(flags);
    }
}

void DecoderFlag1Menu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);

    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto const & t : textElements)
    {
        if(t.first == selected)
            t.second.Render(renderer, font, highlightColour);
        else
            t.second.Render(renderer, font);
    }
}

void DecoderFlag1Menu::SetFlags(int32_t flags)
{
    this->flags = flags;
    UpdateUI(this->flags);
}

int32_t DecoderFlag1Menu::Flags() const
{
    return flags;
}

void DecoderFlag1Menu::ToggleFlag(int32_t flag)
{
    auto targetFlag = flags & flag;
    targetFlag ^= flag; //flip it

    //clear original state
    flags &= ~flag;
    //apply toggled flag
    flags |= targetFlag;
}

void DecoderFlag1Menu::UpdateUI(int32_t flags)
{
    auto& all = decoder::flag1Desc;
    auto const found = decoder::Flags1ToDescriptions(flags);
    for(auto& item : textElements)
    {
        if(auto active = found.find(item.first); active != found.end())
        {
            textElements[item.first].value = "[x] " + active->second;
        }
        else if(auto inactive = all.find(item.first); inactive != all.end())
        {
            textElements[item.first].value = "[ ] " + inactive->second;
        }
    }
}

void DecoderFlag1Menu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = AvailableFlags1;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
    }

    UpdateUI(flags);
}
