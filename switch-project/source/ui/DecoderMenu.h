#ifndef __DECODERMENU_H__
#define __DECODERMENU_H__

#include "Menu.h"
#include "../ffmpegHelpers/DecoderFlags.h"
#include <unordered_map>
#include <array>
#include <vector>

enum DecoderMenuPage : int32_t
{
    DMP_FLAG1 = 0,
    DMP_FLAG2,
    DMP_HWACCEL,
    DMP_EXTRA,
    DMP_COUNT
};

enum DecoderMenuExtraElements : int32_t
{
    DME_THREAD_TYPE = 0,
    DME_THREAD_COUNT,
    DME_SKIP_LOOP_FILTER,
    DME_COUNT
};

class DecoderMenu : public Menu
{
    public:
        DecoderMenu();

        void ProcessInput(PadState const & pad) override;

        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        DecoderConfiguration const DecoderSettings() const;
        
    private:
        std::vector<std::pair<int32_t, Text>> flag1Buttons;
        int32_t decoderFlags1;

        std::vector<std::pair<int32_t, Text>> flag2Buttons;
        int32_t decoderFlags2;

        std::vector<std::pair<int32_t, Text>> hwaccelFlagButtons;
        int32_t hwAccelFlags;

        std::array<AVDiscard, 7> skipLoopFilterOptions;
        int32_t selectedSkipLoopFilter;

        std::array<int32_t, 2> threadTypeOptions;
        int32_t selectedThreadType;
        int32_t threadCount;

        std::array<Text, DME_COUNT> extraButtons;
        int32_t selectedItem;
        int32_t maxItems;

        DecoderMenuPage currentPage;
        Text currentPageHeader;

        void OrderAvailableFlags();

        void Update();
        void UpdateFlags1();
        void UpdateFlags2();
        void UpdateHWAccelFlags();
        void UpdateExtraOptions();

        void ApplyOption();
};

#endif
