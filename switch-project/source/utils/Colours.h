#ifndef __COLOURS_H__
#define __COLOURS_H__

extern "C"
{
    #include <SDL2/SDL_pixels.h>
}

namespace colours
{
    SDL_Color constexpr black = { 0, 0, 0, 255};
    SDL_Color constexpr green = { 100, 200, 100, 255 };
    SDL_Color constexpr orange = { 255, 190, 90, 255 };
    SDL_Color constexpr red = { 200, 100, 100, 255 };
    SDL_Color constexpr blue = {100, 100, 200, 255};
    SDL_Color constexpr skyblue = {50, 180, 220, 255};
    SDL_Color constexpr white = {255, 255, 255, 255};
    SDL_Color constexpr yellow = {200, 200, 50, 255};
    SDL_Color constexpr offblack = {20, 20, 20, 255};
    SDL_Color constexpr grey = { 60, 60, 60, 255 };
    SDL_Color constexpr lightgrey = {200, 200, 200, 255};
    SDL_Color constexpr pink = {200, 140, 120, 255};
    SDL_Color constexpr lavendar = {120, 120, 200, 255};
}

#endif
