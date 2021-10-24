#ifndef __DECODERDISCARDMENU_H__
#define __DECODERDISCARDMENU_H__

#include "Menu.h"
#include "srp/decoder/DiscardType.h"
#include "srp/utils/ArrayCirculator.h"
#include <unordered_map>
#include <vector>

class DecoderDiscardMenu : public Menu
{
public:
    DecoderDiscardMenu();

    void ProcessInput(PadState const& pad) override;
    void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    void SetFlag(AVDiscard flag);
    AVDiscard Flag() const;

private:
    std::unordered_map<AVDiscard, Text> textElements;
    ArrayCirculator<AVDiscard, std::vector> cursor;
    AVDiscard selected;

    void UpdateUI(AVDiscard flag);
    void SetupText();
};

#endif
