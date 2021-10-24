#ifndef __SYSTEMSETUP_H__
#define __SYSTEMSETUP_H__

#include "../SDL_FontCache.h"
#include <SDL2/SDL_render.h>

void initialiseSwitch();
FC_Font* loadSystemFont(SDL_Renderer* renderer, Uint32 fontSize, SDL_Color defaultCol);
void freeFont(FC_Font* font);
void cleanupSystem();
void printOutAtomicLockInfo();

void setMainSystemFont(FC_Font * const font);
FC_Font * const mainSystemFont();

#endif
