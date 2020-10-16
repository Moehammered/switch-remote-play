#include "ScreenRenderer.h"
#include <string>
#include <iostream>
#include <switch.h>

using namespace std;

bool ScreenRenderer::Initialise(unsigned short width, unsigned short height, unsigned int fontSize, bool vSync)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "Failed to initialise SDL. SDL ERROR: " << SDL_GetError() << endl;
        return false;
    }

    this->height = height;
    this->width = width;
    window = SDL_CreateWindow("switch-remote-play", 0, 0, (int)width, (int)height, SDL_WINDOW_FULLSCREEN);
    if (window == nullptr)
    {
        cout << "Attempted to create a window of size " << (int)width << ", " << (int)height << endl;
        cout << "Error when trying to create an SDL window. SDL ERROR: " << SDL_GetError() << endl;
        SDL_Quit();
        return false;
    }
    
    Uint32 renderFlags = SDL_RENDERER_ACCELERATED;
    if(vSync)
        renderFlags |= SDL_RENDERER_PRESENTVSYNC;

    renderer = SDL_CreateRenderer(window, 0, renderFlags);
    if (renderer == nullptr)
    {
        cout << "Error trying to create an SDL renderer. SDL ERROR: " << SDL_GetError() << endl;
        SDL_Quit();
        return false;
    }

    screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width, height);
    region.x = 0;
    region.y = 0;
    region.h = height;
    region.w = width;

    PlFontData switchFont, nintendoFont;
    plGetSharedFontByType(&switchFont, PlSharedFontType_Standard);
    plGetSharedFontByType(&nintendoFont, PlSharedFontType_NintendoExt);
    
    systemFont = FC_CreateFont();
    auto switchFontStream = SDL_RWFromMem((void *)switchFont.address, switchFont.size);
    auto nintFontStream = SDL_RWFromMem((void *)nintendoFont.address, nintendoFont.size);
    FC_LoadFont_RW(systemFont, renderer, switchFontStream, nintFontStream, 1, fontSize, FC_MakeColor(0, 0, 0, 255), TTF_STYLE_NORMAL);

    return true;
}

void ScreenRenderer::CleanupFont()
{
    FC_ClearFont(systemFont);
    FC_FreeFont(systemFont);
}

void ScreenRenderer::ClearScreen(SDL_Color bgCol)
{
    SDL_SetRenderDrawColor(renderer, bgCol.r, bgCol.g, bgCol.b, bgCol.a);
    SDL_RenderClear(renderer);
}

void ScreenRenderer::RenderScreenTexture()
{
    SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
    PresentScreen();
}

void ScreenRenderer::DrawText(std::string text, float x, float y, SDL_Color colour)
{
    FC_DrawColor(systemFont, renderer, x, y, colour, text.c_str());
}

void ScreenRenderer::PresentScreen()
{
    SDL_RenderPresent(renderer);
}

SDL_Renderer* ScreenRenderer::Renderer()
{
    return renderer;
}

SDL_Texture* ScreenRenderer::GetScreenTexture()
{
    return screenTexture;
}

SDL_Rect ScreenRenderer::GetRegion()
{
    return region;
}