#ifndef __SCREENRENDERER__H_
#define __SCREENRENDERER__H_

#include <SDL2/SDL.h>
#include <string>
#include "SDL_FontCache.h"

class ScreenRenderer
{
    public:
        bool Initialise(unsigned short width, unsigned short height, bool vSync);
        void ClearScreen(SDL_Color clearColour);
        void RenderScreenTexture();
        void PresentScreen();
        
        SDL_Renderer* Renderer();
        SDL_Texture* GetScreenTexture();
        const SDL_Rect& Region();

    private:
        unsigned short width, height;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* screenTexture;
        SDL_Rect region;
};

#endif