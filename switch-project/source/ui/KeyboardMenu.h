#ifndef __KEYBOARDMENU_H__
#define __KEYBOARDMENU_H__

#include "Menu.h"
#include "srp/keyboard/KeyboardOptions.h"
#include "srp/utils/ArrayCirculator.h"
#include "srp/utils/UnorderedMapCirculator.h"

class KeyboardMenu : public Menu
{
public:
    KeyboardMenu();

    void ProcessInput(PadState const & pad) override;
    void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    keyboard::KeyboardConfig const Settings() const;

private:
    ArrayCirculator<keyboard::KeyParameter, std::vector> selectionCursor;
    std::unordered_map<keyboard::KeyParameter, Text> textElements;

    std::unordered_map<keyboard::KeyParameter, uint32_t> bindingMap;

    void ChangeParam(keyboard::KeyParameter param, bool clear);
    void UpdateUI(keyboard::KeyParameter param);
    void SetupText();
};

#endif