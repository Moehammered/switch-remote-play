#ifndef __MOUSEMENU_H__
#define __MOUSEMENU_H__

#include "Menu.h"
#include "../mouse/MouseOptions.h"
#include "../utils/ArrayCirculator.h"
#include "../utils/UnorderedMapCirculator.h"

class MouseMenu : public Menu
{
public:
    MouseMenu();

    void ProcessInput(PadState const & pad) override;
    void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    mouse::MouseConfig const Settings() const;

private:
    ArrayCirculator<mouse::Parameters, std::vector> selectionCursor;
    std::unordered_map<mouse::Parameters, Text> textElements;

    UnorderedMapCirculator<HidNpadButton, std::string> leftMouseBtnCursor;
    UnorderedMapCirculator<HidNpadButton, std::string> rightMouseBtnCursor;
    UnorderedMapCirculator<HidNpadButton, std::string> middleMouseBtnCursor;
    UnorderedMapCirculator<controller::AnalogStick, std::string> mouseWheelAnalogCursor;
    bool mouseOnConnect;
    int32_t mouseSensitivity;

    bool keyAssigned(HidNpadButton const desired, std::initializer_list<HidNpadButton> const taken);
    void cycleMouseBtnCursor(UnorderedMapCirculator<HidNpadButton, std::string> & cursor, int direction, std::initializer_list<HidNpadButton> const others);
    void ChangeParam(mouse::Parameters param, int value);
    void UpdateUI(mouse::Parameters param);
    void SetupText();
};

#endif