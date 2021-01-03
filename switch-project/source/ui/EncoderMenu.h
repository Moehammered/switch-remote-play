#ifndef __ENCODERMENU_H__
#define __ENCODERMENU_H__

#include <array>
#include "Menu.h"
#include "../network/NetworkData.h"

enum EncoderMenuElements : int32_t
{
    FRAMERATE = 0,
    DESKTOP_RES,
    SWITCH_RES,
    BITRATE,
    VSYNC,
    CRF,
    HWACCEL,
    PRESET,
    CODEC,
    CONFIG_COUNT
};
class EncoderMenu : public Menu
{
    public:
        EncoderMenu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        void IncreaseSetting();
        void DecreaseSetting();
        void SelectNext();
        void SelectPrevious();
        EncoderConfig const Settings() const;

    private:
        int settingIndex;

        std::array<int, EncoderMenuElements::CONFIG_COUNT> settingsIndices;

        std::array<Text, EncoderMenuElements::CONFIG_COUNT> settingsText;

        void UpdateFramerate();
        void UpdateVideoRes();
        void UpdateVideoScale();
        void UpdateBitrate();
        void UpdateVsync();
        void UpdatePreset();
        void UpdateCRF();
        void UpdateCodec();
        void UpdateHWAccel();
};

#endif
