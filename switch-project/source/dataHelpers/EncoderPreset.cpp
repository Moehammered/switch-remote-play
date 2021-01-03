#include "EncoderPreset.h"

std::string EncoderPresetToString(EncoderPreset preset)
{
    switch(preset)
    {
        default:
        case EncoderPreset::ULTRAFAST:
            return "ultrafast";
        case EncoderPreset::VERYFAST:
            return "veryfast";
        case EncoderPreset::FAST:
            return "fast";
        case EncoderPreset::MEDIUM:
            return "medium";
        case EncoderPreset::SLOW:
            return "slow";
        case EncoderPreset::VERYSLOW:
            return "veryslow";
    }
}

EncoderPreset ParseEncoderPresetString(std::string s)
{
    if(s == "ultrafast")
        return EncoderPreset::ULTRAFAST;
    else if(s == "veryfast")
        return EncoderPreset::VERYFAST;
    else if(s == "fast")
        return EncoderPreset::FAST;
    else if(s == "medium")
        return EncoderPreset::MEDIUM;
    else if(s == "slow")
        return EncoderPreset::SLOW;
    else if(s == "veryslow")
        return EncoderPreset::VERYSLOW;
    else
        return EncoderPreset::ULTRAFAST;
}

std::string EncoderPresetDescription(EncoderPreset preset)
{
    switch(preset)
    {
        case EncoderPreset::ULTRAFAST:
            return "ultra fast (bad quality)";
        case EncoderPreset::VERYFAST:
            return "very fast";
        case EncoderPreset::FAST:
            return "fast";
        case EncoderPreset::MEDIUM:
            return "medium (balanced quality)";
        case EncoderPreset::SLOW:
            return "slow";
        case EncoderPreset::VERYSLOW:
            return "very slow (best quality)";

        default:
            return "unknown";
    }
}
