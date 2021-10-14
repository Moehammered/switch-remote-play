#ifndef __SYSTEMSETUP_H__
#define __SYSTEMSETUP_H__

#include <SDL2/SDL_render.h>
#include "../SDL_FontCache.h"

void initialiseSwitch();
FC_Font* LoadSystemFont(SDL_Renderer* renderer, Uint32 fontSize, SDL_Color defaultCol);
void FreeFont(FC_Font* font);
void CleanupSystem();
void PrintOutAtomicLockInfo();

void SetMainSystemFont(FC_Font * const font);
FC_Font * const MainSystemFont();

#endif
