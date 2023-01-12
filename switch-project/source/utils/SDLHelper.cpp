#include "SDLHelper.h"
#include <sstream>

std::string utils::AudioFormatToString(SDL_AudioFormat const& format)
{
    switch(format)
    {
        case AUDIO_S16:
        return "S16";

        case AUDIO_U16:
        return "U16";
        
        default:
        return std::string{"Unknown "} + std::to_string(format);
    }
    return "";
}

std::string utils::to_string(SDL_AudioSpec const& spec)
{
    auto ss = std::stringstream{};

    ss << "Channels: " << (int)spec.channels << "\n";
    ss << "Frequency: " << spec.freq << "\n";
    ss << "Samples: " << spec.samples << "\n";
    ss << "Size: " << spec.size << "\n";
    ss << "Format: " << AudioFormatToString(spec.format) << "\n";

    return ss.str();
}