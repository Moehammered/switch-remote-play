#include "H264AmfMenu.h"
#include "../utils/Colours.h"
#include "srp/codec/h264_amf/H264AmfConfiguration.h"
#include "../system/SoftwareKeyboard.h"

namespace
{
    int modifyParameter(h264amf::H264AmfParameters const param, int direction, h264amf::H264AmfData const currentData)
    {
        switch (param)
        {
        default:
        case h264amf::H264AmfParameters::Usage:
        case h264amf::H264AmfParameters::Profile:
        case h264amf::H264AmfParameters::Quality:
        case h264amf::H264AmfParameters::RateControl:
        case h264amf::H264AmfParameters::Level:
        case h264amf::H264AmfParameters::FrameQuant_BDelta:
        case h264amf::H264AmfParameters::FrameQuant_BRefDelta:
        case h264amf::H264AmfParameters::EnforceHRD:
        case h264amf::H264AmfParameters::FillerData:
        case h264amf::H264AmfParameters::VBAQ:
        case h264amf::H264AmfParameters::Frameskip:
        case h264amf::H264AmfParameters::BFrameRef:
        case h264amf::H264AmfParameters::LogToDbg:
            return direction;

        case h264amf::H264AmfParameters::FrameQuant_I:
            return keyboardNumber(h264amf::qpFrameMin, h264amf::qpFrameMax, currentData.qp_i);

        case h264amf::H264AmfParameters::FrameQuant_P:
            return keyboardNumber(h264amf::qpFrameMin, h264amf::qpFrameMax, currentData.qp_p);
            
        case h264amf::H264AmfParameters::FrameQuant_B:
            return keyboardNumber(h264amf::qpFrameMin, h264amf::qpFrameMax, currentData.qp_b);
        }
    }
}

H264AmfMenu::H264AmfMenu() : Menu(),
textElements{}, codec{}, selected{}
{
    title.value = "H264 Codec - AMD GPU";
    title.y += 30;
    selected = codec.Current();

    auto config = H264AmfConfiguration{};
    codec.Set(config.Data());

    SetupText();
}

void H264AmfMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Up)
        selected = codec.Prev();
    else if (kDown & HidNpadButton::HidNpadButton_Down)
        selected = codec.Next();

    auto const modifyH264AmfParam = [=](auto const param, auto direction)
    {
        return modifyParameter(param, direction, codec.Data());
    };

    if(kDown & HidNpadButton::HidNpadButton_A)
        codec.Increase(modifyH264AmfParam);
    else if(kDown & HidNpadButton::HidNpadButton_B)
        codec.Decrease(modifyH264AmfParam);
    
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

h264amf::H264AmfData const H264AmfMenu::Settings() const
{
    return codec.Data();
}

void H264AmfMenu::UpdateUI(h264amf::H264AmfParameters param)
{
    auto prefix = h264amf::h264AmfParamsDesc.at(param);
    auto data = codec.Data();

    switch(param)
    {
        case h264amf::H264AmfParameters::Usage:
            {
                auto str = h264amf::amfUsageToDescription(data.usage);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::Profile:
            {
                auto str = h264amf::amfProfileToStr(data.profile);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::Level:
            {
                // auto str = h264amf::amfLevelToStr(data.level);
                // textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::Quality:
            {
                auto str = h264amf::amfQualityToStr(data.quality);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::RateControl:
            {
                auto str = h264amf::amfRateControlToStr(data.rateControl);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::FrameQuant_I:
            {
                auto str = h264amf::amfFrameQPToStr(data.qp_i);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::FrameQuant_P:
            {
                auto str = h264amf::amfFrameQPToStr(data.qp_p);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::FrameQuant_B:
            {
                auto str = h264amf::amfFrameQPToStr(data.qp_b);
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::FrameQuant_BDelta:
            {
                // auto str = h264amf::amfBFrameDeltaQPToStr(data.qp_bfDelta);
                // textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::FrameQuant_BRefDelta:
            {
                // auto str = h264amf::amfBFrameDeltaQPToStr(data.qp_bfRefDelta);
                // textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::EnforceHRD:
            {
                // auto str = data.enforceHRD ? "yes" : "no";
                // textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::FillerData:
            {
                // auto str = data.fillerData ? "yes" : "no";
                // textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::VBAQ:
            {
                // auto str = data.vbaq ? "yes" : "no";
                // textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::Frameskip:
            {
                auto str = data.frameskip ? "yes" : "no";
                textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::BFrameRef:
            {
                // auto str = data.bfRef ? "yes" : "no";
                // textElements[param].value = prefix + ": " + str;
            }
        break;

        case h264amf::H264AmfParameters::LogToDbg:
            {
                // auto str = data.logToDbg ? "yes" : "no";
                // textElements[param].value = prefix + ": " + str;
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

    auto params = h264amf::h264AmfParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = colours::white;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        textElements[p].value = "unintialised";
        UpdateUI(p);
    }
}