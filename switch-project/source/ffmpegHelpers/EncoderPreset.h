#ifndef __ENCODERPRESET_H__
#define __ENCODERPRESET_H__

#include <stdint.h>
#include <string>

enum EncoderPreset : int16_t
{
    ULTRAFAST = 0,
    VERYFAST,
    FAST,
    MEDIUM,
    SLOW,
    VERYSLOW,
    ENCODER_PRESET_COUNT
};

std::string EncoderPresetToString(EncoderPreset preset);
EncoderPreset ParseEncoderPresetString(std::string s);
std::string EncoderPresetDescription(EncoderPreset preset);

#endif