#ifndef __DECODERMENU_H__
#define __DECODERMENU_H__

#include "Menu.h"
#include "../utils/ArrayCirculator.h"
#include "../decoder/DecoderOptions.h"
#include <unordered_map>
#include "DecoderFlag1Menu.h"
#include "DecoderFlag2Menu.h"
#include "DecoderAccelMenu.h"
#include "DecoderDiscardMenu.h"
#include "DecoderThreadMenu.h"

class DecoderMenu : public Menu
{
public:
    DecoderMenu();

    void ProcessInput(PadState const & pad) override;
    void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    DecoderData const Settings() const;

private:
    ArrayCirculator<DecoderParameters, std::vector> cursor;
    DecoderParameters selected;
    DecoderFlag1Menu flag1Menu;
    DecoderFlag2Menu flag2Menu;
    DecoderAccelMenu accelMenu;
    DecoderDiscardMenu discardMenu;
    DecoderThreadMenu threadMenu;
    
    std::unordered_map<DecoderParameters, Menu *> menus;
    Text pageText;

    void UpdatePageHeader(DecoderParameters param);
};

#endif
