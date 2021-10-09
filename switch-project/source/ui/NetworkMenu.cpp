#include "NetworkMenu.h"
#include "../network/NetworkConfiguration.h"
#include "../system/SoftwareKeyboard.h"
#include "../utils/StringUtils.h"

NetworkMenu::NetworkMenu() : Menu(),
    warningText{}, data{}, 
    selected{network::ParamsList}, textElements{}
{ 
    title.value = "Network Configuration";
    warningText.x = 75; 
    warningText.y = 620;
    warningText.colour = {255, 50, 0, 255};
    warningText.value = "Please make sure when using manual IP mode that the IP matches your PC IP.\nIf it is wrong, then you will need to close the app via the HOME button.";

    auto config = NetworkConfiguration{};
    data = config.Data();

    SetupText();
}

void NetworkMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton_Up)
        selected--;
    else if(kDown & HidNpadButton_Down)
        selected++;

    if(kDown & HidNpadButton_A)
        EditParam(*selected);
}

void NetworkMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    warningText.Render(renderer, font);
    for(auto const& item : textElements)
    {
        if(item.first == *selected)
            item.second.Render(renderer, font, highlightColour);
        else
            item.second.Render(renderer, font);
    }
}

network::NetworkConfig const NetworkMenu::Settings() const
{
    return data;
}

std::string const NetworkMenu::EnterIP(std::string const defaultIP) const
{
    auto settings = KeyboardParserProperties<std::string>{};
    settings.defaultValue = defaultIP;
    settings.parseMethod = [](std::string const inputText)
    {
        return Trim(inputText);
    };
    settings.predicate = [](std::string const ip)
    {
        auto stream = std::stringstream(ip);
        auto segments = std::vector<std::string>{};

        auto const ipSegmentMax = 255;
        auto const ipSegmentMin = 0;
        auto segment = std::string{};
        while(std::getline(stream, segment, '.'))
        {
            auto ipNumber = std::atoi(segment.c_str());
            if(ipNumber < ipSegmentMin || ipNumber > ipSegmentMax)
                return false;
            else
                segments.emplace_back(segment);
        }

        if(segments.size() != 4)
            return false;
        else
            return true;
    };

    auto const header = "Enter an IP Address";
    auto const maxIPLength = 16;
    settings.keyboardConfig.displayMessage = header;
    settings.keyboardConfig.inputLength = maxIPLength;
    settings.keyboardConfig.keyboardLayout = SwkbdType::SwkbdType_NumPad;
    settings.keyboardConfig.optionalLeftSymbol = ".";
    settings.keyboardConfig.optionalRightSymbol = ".";
    settings.keyboardConfig.initialText = defaultIP;

    return OpenKeyboard(settings);
}

uint16_t const NetworkMenu::EnterPort(uint16_t const defaultPort) const
{
    auto settings = KeyboardParserProperties<int32_t>{};
    settings.defaultValue = defaultPort;
    settings.parseMethod = [](std::string const inputText)
    {
        return std::atoi(inputText.c_str());
    };
    settings.predicate = [=](int32_t const port)
    {
        return port >= network::MinPortNumber && port <= network::MaxPortNumber;
    };

    auto const minStr = std::to_string(network::MinPortNumber);
    auto const maxStr = std::to_string(network::MaxPortNumber);
    auto const header = "Enter an unused port number between " + minStr + " and " + maxStr;
    settings.keyboardConfig.displayMessage = header;
    settings.keyboardConfig.inputLength = maxStr.size();
    settings.keyboardConfig.keyboardLayout = SwkbdType::SwkbdType_NumPad;
    settings.keyboardConfig.initialText = std::to_string(defaultPort);

    return (uint16_t)OpenKeyboard(settings);
}

void NetworkMenu::EditParam(network::Parameters param)
{
    auto portTaken = [&](auto const portTarget, auto const & portList)
    {
        for(auto const port : portList)
        {
            if(port == portTarget)
                return true;
        }

        return false;
    };

    auto editPort = [&](uint16_t & member)
    {
        auto const allPorts = {
            data.handshakePort, data.broadcastPort, data.commandPort, 
            data.gamepadPort, data.videoPort, data.audioPort
        };

        auto otherPorts = std::vector<uint16_t>{};
        for(auto const port : allPorts)
        {
            if(port != member)
                otherPorts.push_back(port);
        }

        auto const backup = member;
        auto newPort = backup;

        do
        {
            newPort = EnterPort(backup);
        } while (portTaken(newPort, otherPorts));

        member = newPort;
    };

    switch(param)
    {
        case network::Parameters::ManualIPAddress:
            data.manualIP = EnterIP(data.manualIP);
            break;
        
        case network::Parameters::BroadcastAddress:
            data.broadcastIP = EnterIP(data.broadcastIP);
            break;
        
        case network::Parameters::ManualIPEnabled:
            data.manualIPEnabled = data.manualIPEnabled;
            break;

        case network::Parameters::HandshakePort:
            editPort(data.handshakePort);
            break;

        case network::Parameters::BroadcastPort:
            editPort(data.broadcastPort);
            break;

        case network::Parameters::CommandPort:
            editPort(data.commandPort);
            break;

        case network::Parameters::GamepadPort:
            editPort(data.gamepadPort);
            break;

        case network::Parameters::VideoPort:
            editPort(data.videoPort);
            break;

        case network::Parameters::AudioPort:
            editPort(data.audioPort);
            break;
    }

    UpdateUI(param);
}

void NetworkMenu::UpdateUI(network::Parameters param)
{
    auto prefix = network::ParamsDesc.at(param);
    auto updateElement = [&](auto const & str){
        textElements[param].value = prefix + ": " + str;
    };

    auto updatePort = [&](auto const portNo)
    {
        auto str = std::to_string(portNo);
        updateElement(str);
    };

    switch(param)
    {
        case network::Parameters::ManualIPAddress:
            updateElement(data.manualIP);
            break;

        case network::Parameters::BroadcastAddress:
            updateElement(data.broadcastIP);
            break;

        case network::Parameters::ManualIPEnabled:
        {
            auto str = data.manualIPEnabled == false ? "no" : "yes";
            updateElement(str);
        }
        break;

        case network::Parameters::HandshakePort:
            updatePort(data.handshakePort);
            break;

        case network::Parameters::BroadcastPort:
            updatePort(data.broadcastPort);
            break;

        case network::Parameters::CommandPort:
            updatePort(data.commandPort);
            break;

        case network::Parameters::GamepadPort:
            updatePort(data.gamepadPort);
            break;

        case network::Parameters::VideoPort:
            updatePort(data.videoPort);
            break;

        case network::Parameters::AudioPort:
            updatePort(data.audioPort);
            break;
    }
}

void NetworkMenu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = network::ParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}