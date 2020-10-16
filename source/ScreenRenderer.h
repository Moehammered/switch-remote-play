#ifndef __SCREENRENDERER__H_
#define __SCREENRENDERER__H_

#include <SDL2/SDL.h>
#include <string>
#include "SDL_FontCache.h"

class ScreenRenderer
{
    public:
        bool Initialise(unsigned short width, unsigned short height, unsigned int fontSize, bool vSync);
        void ClearScreen(SDL_Color clearColour);
        void RenderScreenTexture();
        void DrawText(std::string text, float x, float y, SDL_Color colour);
        void PresentScreen();
        void CleanupFont();
        
        SDL_Renderer* Renderer();
        SDL_Texture* GetScreenTexture();
        SDL_Rect GetRegion();

    private:
        unsigned short width, height;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* screenTexture;
        SDL_Rect region;
        FC_Font* systemFont;
};

#endif