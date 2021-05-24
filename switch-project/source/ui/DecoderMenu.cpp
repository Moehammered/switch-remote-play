#include "DecoderMenu.h"
#include "../system/Configuration_Old.h"
#include <iostream>

int32_t constexpr maxThreadCountOption {4};
bool firstMenu {false};

DecoderMenu::DecoderMenu() : Menu(), 
    flag1Buttons{}, decoderFlags1{},
    flag2Buttons{}, decoderFlags2{},
    hwaccelFlagButtons{}, hwAccelFlags{},
    skipLoopFilterOptions{}, selectedSkipLoopFilter{1}, 
    threadTypeOptions{FF_THREAD_FRAME, FF_THREAD_SLICE},
    selectedThreadType {1}, threadCount {4},
    extraButtons{}, selectedItem {0}, maxItems{0},
    currentPage {DMP_FLAG1}, currentPageHeader{},
    newFlag1Menu{}, newFlag2Menu{}
{
    title.value = "Decoder Configuration_Old";
    SDL_Color constexpr white {255,255,255,255};
    SDL_Color constexpr orange = { 255, 190, 90, 255 };
    currentPageHeader.y = title.y + 60;
    currentPageHeader.colour = orange;
    currentPageHeader.x = 50;

    OrderAvailableFlags();

    // spacing
    auto const yOffset = title.y + 100;
    auto constexpr ySpacing = 45;

    auto allDiscards = AllAVDiscardDescriptions();
    auto counter = 0;
    for(auto& item : allDiscards)
    {
        skipLoopFilterOptions[counter] = item.first;
        ++counter;
    }

    auto miscY = yOffset;
    for(auto i = 0U; i < extraButtons.size(); ++i)
    {
        extraButtons[i].colour = white;
        extraButtons[i].x = 100;
        extraButtons[i].y = miscY;
        miscY += ySpacing;
    }
    
    maxItems = flag1Buttons.size();

    LoadSettings();
    Update();
}

void DecoderMenu::OrderAvailableFlags()
{
    SDL_Color constexpr white {255,255,255,255};
    auto constexpr xOffset = 100;
    auto constexpr yOffset = 260;
    auto constexpr ySpacing = 45;

    auto flag1Order = {
        AV_CODEC_FLAG_LOW_DELAY, AV_CODEC_FLAG_DROPCHANGED, 
        AV_CODEC_FLAG_PASS1, AV_CODEC_FLAG_PASS2,
        AV_CODEC_FLAG_LOOP_FILTER
    };

    auto flag1Y = yOffset;
    for(auto& flag : flag1Order)
    {
        flag1Buttons.emplace_back(flag, Text{});
        auto& last = flag1Buttons.back();
        last.second.x = xOffset;
        last.second.y = flag1Y;
        last.second.colour = white;
        flag1Y += ySpacing;
    }

    auto flag2Order = {
        AV_CODEC_FLAG2_FAST,
        AV_CODEC_FLAG2_NO_OUTPUT,
        AV_CODEC_FLAG2_IGNORE_CROP
    };

    auto flag2Y = yOffset;
    for(auto& flag : flag2Order)
    {
        flag2Buttons.emplace_back(flag, Text{});
        auto& last = flag2Buttons.back();
        last.second.x = xOffset;
        last.second.y = flag2Y;
        last.second.colour = white;
        flag2Y += ySpacing;
    }

    auto hwFlagOrder = {
        AV_HWACCEL_CODEC_CAP_EXPERIMENTAL,
        AV_HWACCEL_FLAG_IGNORE_LEVEL,
        AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH
    };

    auto hwY = yOffset;
    for(auto& flag : hwFlagOrder)
    {
        hwaccelFlagButtons.emplace_back(flag, Text{});
        auto& last = hwaccelFlagButtons.back();
        last.second.x = xOffset;
        last.second.y = hwY;
        last.second.colour = white;
        hwY += ySpacing;
    }
}

void DecoderMenu::LoadSettings()
{
    auto config = Configuration_Old{};
    auto decoderSettings = config.DecoderData();

    decoderFlags1 = decoderSettings.flag1;
    decoderFlags2 = decoderSettings.flag2;
    hwAccelFlags = decoderSettings.hwAccelFlags;
    
    for(auto i = 0U; i < skipLoopFilterOptions.size(); ++i)
    {
        if(skipLoopFilterOptions[i] == decoderSettings.skipLoopFilter)
        {
            selectedSkipLoopFilter = i;
            break;
        }
    }

    for(auto i = 0U; i < threadTypeOptions.size(); ++i)
    {
        if(threadTypeOptions[i] == decoderSettings.threadType)
        {
            selectedThreadType = i;
            break;
        }
    }

    threadCount = decoderSettings.threadCount;
}

void DecoderMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);
    if(kDown & (KEY_DLEFT | KEY_DRIGHT))
        firstMenu = !firstMenu;

    if(firstMenu)
        newFlag1Menu.ProcessInput(pad);
    else
        newFlag2Menu.ProcessInput(pad);

    return;


    if(kDown & KEY_DLEFT)
    {
        auto selected = (int32_t)currentPage;
        --selected;
        if(selected < 0)
            selected = DecoderMenuPage::DMP_COUNT - 1;
        currentPage = (DecoderMenuPage)selected;

        selectedItem = 0;
        Update();
    }
    else if(kDown & KEY_DRIGHT)
    {
        auto selected = (int32_t)currentPage;
        ++selected;
        if(selected >= DecoderMenuPage::DMP_COUNT)
            selected = 0;
        currentPage = (DecoderMenuPage)selected;

        selectedItem = 0;
        Update();
    }
    else if(kDown & KEY_DDOWN)
    {
        ++selectedItem;
        if(selectedItem >= maxItems)
            selectedItem = 0;
    }
    else if(kDown & KEY_DUP)
    {
        --selectedItem;
        if(selectedItem < 0)
            selectedItem = maxItems - 1;
    }
    else if(kDown & KEY_A)
    {
        ApplyOption();
        Update();
    }
}

void DecoderMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    if(firstMenu)
        newFlag1Menu.Render(renderer, font);
    else
        newFlag2Menu.Render(renderer, font);
    return;
    currentPageHeader.Render(renderer, font);

    switch(currentPage)
    {
        case DecoderMenuPage::DMP_FLAG1:
        {
            auto itr = flag1Buttons.begin();
            for(auto i = 0U; i < flag1Buttons.size(); ++i, ++itr)
            {
                if(i == selectedItem)
                    itr->second.Render(renderer, font, highlightColour);
                else
                    itr->second.Render(renderer, font);
            }
        }
        break;

        case DecoderMenuPage::DMP_FLAG2:
        {
            auto itr = flag2Buttons.begin();
            for(auto i = 0U; i < flag2Buttons.size(); ++i, ++itr)
            {
                if(i == selectedItem)
                    itr->second.Render(renderer, font, highlightColour);
                else
                    itr->second.Render(renderer, font);
            }
        }
        break;

        case DecoderMenuPage::DMP_HWACCEL:
        {
            auto itr = hwaccelFlagButtons.begin();
            for(auto i = 0U; i < hwaccelFlagButtons.size(); ++i, ++itr)
            {
                if(i == selectedItem)
                    itr->second.Render(renderer, font, highlightColour);
                else
                    itr->second.Render(renderer, font);
            }
        }
        break;

        case DecoderMenuPage::DMP_EXTRA:
        {
            for(auto i = 0U; i < extraButtons.size(); ++i)
            {
                if(i == selectedItem)
                    extraButtons[i].Render(renderer, font, highlightColour);
                else
                    extraButtons[i].Render(renderer, font);
            }
        }
        break;
    }
}

DecoderConfiguration const DecoderMenu::DecoderSettings() const
{
    return DecoderConfiguration{
        .flag1 = decoderFlags1,
        .flag2 = decoderFlags2,
        .hwAccelFlags = hwAccelFlags,
        .skipLoopFilter = skipLoopFilterOptions[selectedSkipLoopFilter],
        .threadType = threadTypeOptions[selectedThreadType],
        .threadCount = threadCount
    };
}

void DecoderMenu::Update()
{
    switch(currentPage)
    {
        case DecoderMenuPage::DMP_FLAG1:
        UpdateFlags1();
        maxItems = flag1Buttons.size();
        currentPageHeader.value = "Flag Set 1 (pg 1/4) - Dpad left/right";
        break;

        case DecoderMenuPage::DMP_FLAG2:
        UpdateFlags2();
        maxItems = flag2Buttons.size();
        currentPageHeader.value = "Flag Set 2 (pg 2/4) - Dpad left/right";
        break;

        case DecoderMenuPage::DMP_HWACCEL:
        UpdateHWAccelFlags();
        maxItems = hwaccelFlagButtons.size();
        currentPageHeader.value = "HW Accel Flags (pg 3/4) - Dpad left/right";
        break;

        case DecoderMenuPage::DMP_EXTRA:
        UpdateExtraOptions();
        maxItems = extraButtons.size();
        currentPageHeader.value = "Extra Options (pg 4/4) - Dpad left/right";
        break;
    }
}

void DecoderMenu::UpdateFlags1()
{
    auto currentFlags = DecoderFlags1Descriptions(decoderFlags1);
    auto allF1s = AllDecoderFlag1Descriptions();
    for(auto& item : flag1Buttons)
    {
        item.second.value = allF1s[item.first];
        if(currentFlags.find(item.first) != currentFlags.end())
            item.second.value = "[x] " + item.second.value;
        else
            item.second.value = "[ ] " + item.second.value;
    }
}

void DecoderMenu::UpdateFlags2()
{
    auto currentFlags = DecoderFlags2Descriptions(decoderFlags2);
    auto allF2s = AllDecoderFlag2Descriptions();
    for(auto& item : flag2Buttons)
    {
        item.second.value = allF2s[item.first];
        if(currentFlags.find(item.first) != currentFlags.end())
            item.second.value = "[x] " + item.second.value;
        else
            item.second.value = "[ ] " + item.second.value;
    }
}

void DecoderMenu::UpdateHWAccelFlags()
{
    auto currentFlags = HWAccelFlagsDescriptions(hwAccelFlags);
    auto allHwFlags = AllHWAccelFlagsDescriptions();
    for(auto& item : hwaccelFlagButtons)
    {
        item.second.value = allHwFlags[item.first];
        if(currentFlags.find(item.first) != currentFlags.end())
            item.second.value = "[x] " + item.second.value;
        else
            item.second.value = "[ ] " + item.second.value;
    }
}

void DecoderMenu::UpdateExtraOptions()
{
    auto currentThreadType = threadTypeOptions[selectedThreadType];
    extraButtons[DME_THREAD_TYPE].value = "Thread Type: " + ThreadTypeDescription(currentThreadType);

    extraButtons[DME_THREAD_COUNT].value = "Thread Count: " + std::to_string(threadCount);

    auto currentSkipFilter = skipLoopFilterOptions[selectedSkipLoopFilter];
    extraButtons[DME_SKIP_LOOP_FILTER].value = "Skip Loop Filter: " + AVDiscardDescription(currentSkipFilter);
}

void DecoderMenu::ApplyOption()
{
    switch(currentPage)
    {
        case DecoderMenuPage::DMP_FLAG1:
        {
            auto itr = flag1Buttons.begin();
            for(auto i = 0; i < selectedItem; ++i)
                ++itr;
            auto flag = itr->first;

            auto flagState = decoderFlags1 & flag;
            flagState ^= flag; //flip it

            //clear original state
            decoderFlags1 &= ~flag;
            //apply toggled flag
            decoderFlags1 |= flagState;
        }
        break;

        case DecoderMenuPage::DMP_FLAG2:
        {
            auto itr = flag2Buttons.begin();
            for(auto i = 0; i < selectedItem; ++i)
                ++itr;
            auto flag = itr->first;

            auto flagState = decoderFlags2 & flag;
            flagState ^= flag; //flip it

            //clear original state
            decoderFlags2 &= ~flag;
            decoderFlags2 |= flagState;
        }
        break;

        case DecoderMenuPage::DMP_HWACCEL:
        {
            auto itr = hwaccelFlagButtons.begin();
            for(auto i = 0; i < selectedItem; ++i)
                ++itr;
            auto flag = itr->first;

            auto flagState = hwAccelFlags & flag;
            flagState ^= flag; //flip it

            //clear original state
            hwAccelFlags &= ~flag;
            hwAccelFlags |= flagState;
        }
        break;

        case DecoderMenuPage::DMP_EXTRA:
        {
            switch(selectedItem)
            {
                case DecoderMenuExtraElements::DME_THREAD_TYPE:
                {
                    ++selectedThreadType;
                    if(selectedThreadType >= threadTypeOptions.size())
                        selectedThreadType = 0;
                }
                break;

                case DecoderMenuExtraElements::DME_THREAD_COUNT:
                {
                    ++threadCount;
                    if(threadCount > maxThreadCountOption)
                        threadCount = 1;
                }
                break;

                case DecoderMenuExtraElements::DME_SKIP_LOOP_FILTER:
                {
                    ++selectedSkipLoopFilter;
                    if(selectedSkipLoopFilter >= skipLoopFilterOptions.size())
                        selectedSkipLoopFilter = 0;
                }
                break;
            }
        }
        break;
    }
}

