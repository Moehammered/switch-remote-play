#include "AudioMenu.h"
#include "../utils/Colours.h"
#include "../system/SoftwareKeyboard.h"
#include "srp/audio/AudioConfiguration.h"

AudioMenu::AudioMenu() : Menu(),
    selectionCursor{audio::audioParamsList},
    textElements{},
    sampleRateFrequencyCursor{audio::supportedSampleRateFrequencies},
    channelCount{audio::defaultChannelCount},
    formatCursor{audio::supportedAudioFormats},
    sampleCountCursor{audio::supportedSampleCounts}
{
    title.y += 30;
    title.value = "Audio Configuration";

    auto config = AudioConfiguration{};
    auto storedData = config.Data();

    sampleRateFrequencyCursor.SeekTo(storedData.sampleRateFrequency);
    channelCount = storedData.channelCount;
    formatCursor.SeekTo(storedData.format);
    sampleCountCursor.SeekTo(storedData.sampleCount);

    SetupText();
}

void AudioMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);
    if(kDown & HidNpadButton_Down)
        ++selectionCursor;
    else if(kDown & HidNpadButton_Up)
        --selectionCursor;

    if(kDown & HidNpadButton_A)
    {
        ChangeParam(*selectionCursor, 1);
        UpdateUI(*selectionCursor);
    }
    else if(kDown & HidNpadButton_B)
    {
        ChangeParam(*selectionCursor, -1);
        UpdateUI(*selectionCursor);
    }
}

void AudioMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);

    for(auto const& item : textElements)
    {
        if(item.first == *selectionCursor)
            item.second.Render(renderer, font, highlightColour);
        else
            item.second.Render(renderer, font);
    }
}

audio::AudioConfig const AudioMenu::Settings() const
{
    auto data = audio::AudioConfig{};

    data.sampleRateFrequency = *sampleRateFrequencyCursor;
    data.channelCount = channelCount;
    data.format = *formatCursor;
    data.sampleCount = *sampleCountCursor;

    return data;
}

void AudioMenu::ChangeParam(audio::AudioParameters param, int value)
{
    switch(param)
    {
        case audio::AudioParameters::SampleRateFrequency:
            sampleRateFrequencyCursor += value;
            break;
        
        case audio::AudioParameters::ChannelCount:
            //channelCount = keyboardNumber(audio::minChannelCount, audio::maxChannelCount, channelCount);
            break;

        case audio::AudioParameters::Format:
            formatCursor += value;
            break;

        case audio::AudioParameters::SampleCount:
            sampleCountCursor += value;
            break;
    }
}

void AudioMenu::UpdateUI(audio::AudioParameters param)
{
    auto prefix = audio::audioParamsDesc.at(param);

    auto updateElementText = [&](auto const& str) {
        textElements[param].value = prefix + ": " + str;
    };

    switch (param)
    {
        case audio::AudioParameters::SampleRateFrequency:
        {
            auto str = std::to_string(*sampleRateFrequencyCursor);
            updateElementText(str);
        }
        break;

        case audio::AudioParameters::ChannelCount:
        {
            updateElementText(std::to_string(channelCount));
        }
        break;

        case audio::AudioParameters::Format:
        {
            auto str = audio::audioFormatToDesc(*formatCursor);
            updateElementText(str);
        }
        break;

        case audio::AudioParameters::SampleCount:
        {
            auto str = std::to_string(*sampleCountCursor);
            updateElementText(str);
        }
        break;
    }
}

void AudioMenu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;

    auto params = audio::audioParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = colours::white;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}