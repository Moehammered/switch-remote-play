#include "Menu.h"

Menu::Menu()
    : highlightColour{.r = 200, .g = 200, .b = 50, .a = 255}, position{0},
    renderElements{}, acceptEvents{}, nextEvents{}, prevEvents{}
{ }

void Menu::Next()
{
    if(++position >= renderElements.size())
        position = 0;

    for(auto& call : nextEvents)
        call(position, renderElements[position]);
}

void Menu::Prev()
{
    if(position == 0)
        position = renderElements.size()-1;
    else
        --position;

    for(auto& call : prevEvents)
        call(position, renderElements[position]);
}

void Menu::Accept()
{
    for(auto& call : acceptEvents)
        call(position, renderElements[position]);
}

void Menu::Cancel()
{
    for(auto& call : cancelEvents)
        call(position, renderElements[position]);
}

Text& Menu::AddElement(Text element)
{
    renderElements.push_back(element);
    return *(renderElements.end()-1);
}

void Menu::OnAccept(std::function<void(int, Text&)> delegate)
{
    acceptEvents.push_back(delegate);
}

void Menu::OnCancel(std::function<void(int, Text&)> delegate)
{
    cancelEvents.push_back(delegate);
}

void Menu::OnNext(std::function<void(int, Text&)> delegate)
{
    nextEvents.push_back(delegate);
}

void Menu::OnPrev(std::function<void(int, Text&)> delegate)
{
    prevEvents.push_back(delegate);
}

void Menu::ClearAcceptEvents()
{
    acceptEvents.clear();
}

void Menu::ClearNextEvents()
{
    nextEvents.clear();
}

void Menu::ClearPrevEvents()
{
    prevEvents.clear();
}

void Menu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    for(size_t i = 0; i < renderElements.size(); ++i)
    {
        if(i != position)
            renderElements[i].Render(renderer, font);
        else
            renderElements[i].Render(renderer, font, highlightColour);
    }
}
