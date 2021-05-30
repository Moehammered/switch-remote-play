#include "DecoderDiscardMenu.h"
#include "../decoder/DecoderOptions.h"

DecoderDiscardMenu::DecoderDiscardMenu() : Menu(),
textElements{}, cursor{AvailableDiscardFilters}, selected{AVDiscard::AVDISCARD_DEFAULT}
{
    title.value = "Frame Discard Behaviour";
    title.y += 45;

    SetupText();
    UpdateUI(selected);
}


void DecoderDiscardMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Up)
        --cursor;
    else if (kDown & HidNpadButton::HidNpadButton_Down)
        ++cursor;

    if(kDown & (HidNpadButton::HidNpadButton_A | HidNpadButton::HidNpadButton_B))
    {
        selected = *cursor;
        UpdateUI(selected);
    }
}

void DecoderDiscardMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);

    auto highlighted = *cursor;
    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto const & t : textElements)
    {
        if(t.first == highlighted)
            t.second.Render(renderer, font, highlightColour);
        else
            t.second.Render(renderer, font);
    }
}

void DecoderDiscardMenu::SetFlag(AVDiscard flag)
{
    selected = flag;
    UpdateUI(selected);
}

AVDiscard DecoderDiscardMenu::Flag() const
{
    return selected;
}

void DecoderDiscardMenu::UpdateUI(AVDiscard flag)
{
    auto& all = decoder::discardDesc;
    auto const selectedDesc = decoder::AVDiscardToDescription(flag);
    for(auto& item : textElements)
    {
        if(item.first == flag)
            item.second.value = "[x] " + selectedDesc;
        else
        {
            auto inactiveDesc = all.find(item.first);
            if(inactiveDesc != all.end())
                item.second.value = "[ ] " + inactiveDesc->second;
            else
                item.second.value = "[ ] UNKNOWN VALUE: " + std::to_string(flag);
        }
    }
}

void DecoderDiscardMenu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = AvailableDiscardFilters;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
    }

    UpdateUI(selected);
}
