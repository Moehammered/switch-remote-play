#ifndef __DECODERDISCARDMENU_H__
#define __DECODERDISCARDMENU_H__

#include "Menu.h"
#include "../decoder/DiscardType.h"
#include "../utils/ArrayCirculator.h"
#include <unordered_map>
#include <vector>

class DecoderDiscardMenu : public Menu
{
public:
    DecoderDiscardMenu();

    void ProcessInput(PadState const& pad) override;
    void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    AVDiscard Flag() const;

private:
    std::unordered_map<AVDiscard, Text> textElements;
    DiscardType discard;
    std::unordered_map<AVDiscard, std::string> const discardDesc;
    ArrayCirculator<AVDiscard, std::vector> cursor;
    AVDiscard selected;

    void UpdateUI(AVDiscard flag);
    void SetupText();
};

#endif
