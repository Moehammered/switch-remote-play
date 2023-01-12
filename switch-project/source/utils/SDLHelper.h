#ifndef __SDLHELPER_H__
#define __SDLHELPER_H__

#include <SDL2/SDL_audio.h>
#include <string>

namespace utils
{
    std::string AudioFormatToString(SDL_AudioFormat const& format);
    std::string to_string(SDL_AudioSpec const& spec);
}

#endif
