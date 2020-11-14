#ifndef __MENU_H__
#define __MENU_H__

#include <vector>
#include "Text.h"
#include <functional>
#include <SDL2/SDL_render.h>

class Menu
{
    public:
        SDL_Color highlightColour;

        Menu();
        
        void Next();
        void Prev();
        void Accept();
        void Cancel();

        Text& AddElement(Text element);
        void OnAccept(std::function<void(int, Text&)> delegate);
        void OnCancel(std::function<void(int, Text&)> delegate);
        void OnNext(std::function<void(int, Text&)> delegate);
        void OnPrev(std::function<void(int, Text&)> delegate);

        void ClearAcceptEvents();
        void ClearNextEvents();
        void ClearPrevEvents();

        void Render(SDL_Renderer * const renderer, FC_Font * const font);

    private:
        size_t position;
        std::vector<Text> renderElements;
        std::vector<std::function<void(int, Text&)>> acceptEvents;
        std::vector<std::function<void(int, Text&)>> cancelEvents;
        std::vector<std::function<void(int, Text&)>> nextEvents;
        std::vector<std::function<void(int, Text&)>> prevEvents;
};

#endif
