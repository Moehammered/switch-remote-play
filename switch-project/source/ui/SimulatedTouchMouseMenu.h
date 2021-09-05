#ifndef __SIMULATEDTOUCHMOUSEMENU_H__
#define __SIMULATEDTOUCHMOUSEMENU_H__

#include "Menu.h"
#include "../touch/SimulatedMouseOptions.h"
#include "../utils/ArrayCirculator.h"
#include <unordered_map>

class SimulatedTouchMouseMenu : public Menu
{
    public:
        SimulatedTouchMouseMenu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        touch::SimulatedTouchConfig const Settings() const;

    private:
        Text helpText;
        std::unordered_map<touch::SimulatedTouchMouseParameters, Text> textElements;
        ArrayCirculator<touch::SimulatedTouchMouseParameters, std::vector> selected;
        int16_t deadzoneRadius;

        void PromptValueInput(touch::SimulatedTouchMouseParameters param);

        int16_t KeyboardValue(int16_t minValue, int16_t maxValue);

        void UpdateUI(touch::SimulatedTouchMouseParameters param);

        void SetupText();
};

#endif
