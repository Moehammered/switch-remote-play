#include "H264AmfMenu.h"

H264AmfMenu::H264AmfMenu() : Menu(),
textElements{}, codec{}, selected{}
{
    title.value = "H264 Codec - AMD GPU";
    title.y += 30;
    SetupText();
    selected = codec.Current();
}

void H264AmfMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Up)
        selected = codec.Prev();
    else if (kDown & HidNpadButton::HidNpadButton_Down)
        selected = codec.Next();

    if(kDown & HidNpadButton::HidNpadButton_A)
        codec.Increase();
    else if(kDown & HidNpadButton::HidNpadButton_B)
        codec.Decrease();
    
    UpdateUI(selected);
}

void H264AmfMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);

    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto const & t : textElements)
    {
        if(t.first == selected)
            t.second.Render(renderer, font, highlightColour);
        else
            t.second.Render(renderer, font);
    }
}

void H264AmfMenu::UpdateUI(h264amf::Parameters param)
{
    auto prefix = h264amf::ParamsDesc.at(param);
    auto data = codec.Data();

    switch(param)
    {
        case h264amf::Parameters::Usage:
            {
                auto str = h264amf::amfUsageToDescription(data.usage);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::Profile:
            {
                auto str = h264amf::amfProfileToStr(data.profile);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::Level:
            {
                auto str = h264amf::amfLevelToStr(data.level);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::Quality:
            {
                auto str = h264amf::amfQualityToStr(data.quality);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::RateControl:
            {
                auto str = h264amf::amfRateControlToStr(data.rateControl);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::FrameQuant_I:
            {
                auto str = h264amf::amfFrameQPToStr(data.qp_i);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::FrameQuant_P:
            {
                auto str = h264amf::amfFrameQPToStr(data.qp_p);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::FrameQuant_B:
            {
                auto str = h264amf::amfFrameQPToStr(data.qp_b);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::FrameQuant_BDelta:
            {
                auto str = h264amf::amfBFrameDeltaQPToStr(data.qp_bfDelta);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::FrameQuant_BRefDelta:
            {
                auto str = h264amf::amfBFrameDeltaQPToStr(data.qp_bfRefDelta);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::EnforceHRD:
            {
                auto str = data.enforceHRD ? "yes" : "no";
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::FillerData:
            {
                auto str = data.fillerData ? "yes" : "no";
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::VBAQ:
            {
                auto str = data.vbaq ? "yes" : "no";
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::Frameskip:
            {
                auto str = data.frameskip ? "yes" : "no";
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::BFrameRef:
            {
                auto str = data.bfRef ? "yes" : "no";
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::Parameters::LogToDbg:
            {
                auto str = data.logToDbg ? "yes" : "no";
                textElements[param].value = prefix + ": " + str;
            }
        break;
    }
}

void H264AmfMenu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 30;
    int counter = 1;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = h264amf::ParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        textElements[p].value = "unintialised";
        UpdateUI(p);
    }
}