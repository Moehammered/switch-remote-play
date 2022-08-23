#ifndef __AUDIOMENU_H__
#define __AUDIOMENU_H__

#include "Menu.h"
#include "srp/audio/AudioOptions.h"
#include "srp/utils/ArrayCirculator.h"
#include <unordered_map>

class AudioMenu : public Menu
{
public:
    AudioMenu();

    void ProcessInput(PadState const & pad) override;
    void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

private:
    ArrayCirculator<audio::AudioParameters, std::vector> selectionCursor;
    std::unordered_map<audio::AudioParameters, Text> textElements;

    ArrayCirculator<uint32_t, std::vector> sampleRateFrequencyCursor;
    uint32_t channelCount;
    ArrayCirculator<audio::AudioFormat, std::vector> formatCursor;
    ArrayCirculator<uint32_t, std::vector> sampleCountCursor;

    void ChangeParam(audio::AudioParameters param, int value);
    void UpdateUI(audio::AudioParameters param);
    void SetupText();
};

#endif
