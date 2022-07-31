#include "MouseOptions.h"
#include <sstream>
#include <algorithm>

namespace mouse
{
    std::unordered_map<MouseParameters, std::string> const mouseParamsToStr(MouseConfig const config)
    {
        auto values = std::unordered_map<MouseParameters, std::string>{};

        values[MouseParameters::LeftMouseButton] = controller::switchButtonToString(config.leftClickButton);
        values[MouseParameters::RightMouseButton] = controller::switchButtonToString(config.rightClickButton);
        values[MouseParameters::MiddleMouseButton] = controller::switchButtonToString(config.middleClickButton);
        values[MouseParameters::MouseWheelAnalog] = controller::analogStickToString(config.mouseWheelAnalog);
        values[MouseParameters::MouseSensitivity] = std::to_string(config.mouseSensitivity);
        values[MouseParameters::MouseOnConnect] = config.mouseOnConnect ? "yes" : "no";

        auto btns = controller::switchButtonsToString(config.mouseModeToggleKey);
        if(btns.size() != 0)
        {
            auto str = btns[0];
            for(auto i = 1U; i < btns.size(); ++i)
                str += "," + btns[i];
            
            values[MouseParameters::MouseModeToggleKey] = str;
        }

        auto const toggleTimeSeconds = timeutil::nanoToSecond(config.mouseModeToggleTime);
        auto secondsStr = std::to_string(toggleTimeSeconds);
        values[MouseParameters::MouseModeToggleTime] = secondsStr;

        return values;
    }

    MouseConfig const mouseParamsFromStr(std::unordered_map<MouseParameters, std::string> const & map)
    {
        auto config = MouseConfig{};

        auto parseMouseButton = [&](auto mouseBtn, auto & member, auto defVal)
        {
            auto entry = map.find(mouseBtn);
            if(entry != map.end())
            {
                auto btnStr = entry->second;
                auto btn = controller::parseSwitchButtonString(btnStr);
                member = btn;
            }
            else
                member = defVal;
        };

        parseMouseButton(MouseParameters::LeftMouseButton, config.leftClickButton, defaultLeftClickButton);
        parseMouseButton(MouseParameters::RightMouseButton, config.rightClickButton, defaultRightClickButton);
        parseMouseButton(MouseParameters::MiddleMouseButton, config.middleClickButton, defaultMiddleClickButton);

        {
            auto entry = map.find(MouseParameters::MouseOnConnect);
            if(entry != map.end())
            {
                auto str = entry->second;
                config.mouseOnConnect = str == "yes";
            }
            else
                config.mouseOnConnect = false;
        }

        
        {
            auto entry = map.find(MouseParameters::MouseSensitivity);
            if(entry != map.end())
            {
                auto str = entry->second;
                auto val = std::atoi(str.c_str());
                auto sens = std::min(val, (int)maxMouseSensitivity);
                sens = std::max(sens, (int)minMouseSensitivity);
                config.mouseSensitivity = (int16_t)sens;
            }
            else
                config.mouseSensitivity = defaultMouseSensitivity;
        }

        {
            auto entry = map.find(MouseParameters::MouseWheelAnalog);
            if(entry != map.end())
            {
                auto str = entry->second;
                auto analog = controller::parseAnalogStickString(str);
                config.mouseWheelAnalog = analog;
            }
            else
                config.mouseWheelAnalog = defaultMouseWheelAnalog;
        }

        auto csvToList = [](auto const & csv)
        {
            std::stringstream ss(csv);
            std::string val{};
            auto list = std::vector<std::string>{};
            while(std::getline(ss, val, ','))
                list.emplace_back(val);
            
            return list;
        };

        {
            auto entry = map.find(MouseParameters::MouseModeToggleKey);
            if(entry != map.end())
            {
                auto btnList = csvToList(entry->second);
                auto btns = controller::parseSwitchButtonStrings(btnList);
                auto btnFlags = 0U;
                for(auto const & btn : btns)
                    btnFlags |= btn;
                
                config.mouseModeToggleKey = btnFlags != 0 ? btnFlags : defaultMouseModeToggleKey;
            }
            else
                config.mouseModeToggleKey = defaultMouseModeToggleKey;
        }

        {
            auto entry = map.find(MouseParameters::MouseModeToggleTime);
            if(entry != map.end())
            {
                auto secondsStr = entry->second;
                auto const seconds = std::atof(secondsStr.c_str());
                auto const nanoseconds = timeutil::secondToNano(seconds);
                config.mouseModeToggleTime = std::clamp(nanoseconds, minMouseModeToggleTime, maxMouseModeToggleTime);
            }
            else
                config.mouseModeToggleTime = defaultMouseModeToggleTime;
        }

        return config;
    }

}