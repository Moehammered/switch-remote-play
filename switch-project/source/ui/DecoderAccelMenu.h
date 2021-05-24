#ifndef __DECODERACCELMENU_H__
#define __DECODERACCELMENU_H__

#include "Menu.h"
#include "../decoder/DecoderAccel.h"
#include "../utils/ArrayCirculator.h"
#include <unordered_map>
#include <vector>

class DecoderAccelMenu : public Menu
{
public:
    DecoderAccelMenu();

    void ProcessInput(PadState const& pad) override;
    void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    int32_t Flags() const;

private:
    std::unordered_map<int32_t, Text> textElements;
    DecoderAccel accel;
    std::unordered_map<int32_t, std::string> const accelDesc;
    ArrayCirculator<int32_t, std::vector> cursor;
    int32_t selected;
    int32_t flags;

    void UpdateUI(int32_t flags);
    void SetupText();

    void ToggleFlag(int32_t flag);
};

#endif