#include "Menu.h"
#include "../utils/Colours.h"

Menu::Menu() : highlightColour{ colours::yellow }, 
    title{ .x = 440, .y = 130, .colour = colours::white }
{ 
    title.centered = true;
}

void Menu::ProcessInput(PadState const & pad)
{ }

void Menu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{ }
