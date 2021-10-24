#include "ControllerOptions.h"
#include <algorithm>
#include <sstream>

namespace controller
{
    std::unordered_map<controller::ControllerParameters, std::string> const controllerParamsToStr(ControllerConfig const data)
    {
        auto values = std::unordered_map<controller::ControllerParameters, std::string>{};

        values[ControllerParameters::Mode] = controllerModeToString(data.controllerMode);
        values[ControllerParameters::ButtonMapping] = controllerButtonMapToString(data.controllerMap);
        values[ControllerParameters::LeftAnalogMapping] = controllerAnalogMapToString(data.leftAnalogMap);
        values[ControllerParameters::RightAnalogMapping] = controllerAnalogMapToString(data.rightAnalogMap);
        values[ControllerParameters::ControllerCount] = std::to_string(data.controllerCount);

        auto btns = controller::switchButtonsToString(data.homeButton);
        if(btns.size() != 0)
        {
            auto str = btns[0];
            for(auto i = 1U; i < btns.size(); ++i)
                str += "," + btns[i];
            
            values[ControllerParameters::HomeButton] = str;
        }

        auto const triggerTimeSeconds = timeutil::nanoToSecond(data.homeButtonTriggerTime);
        auto secondsStr = std::to_string(triggerTimeSeconds);
        values[ControllerParameters::HomeButtonTriggerTime] = secondsStr;

        return values;
    }

    ControllerConfig const controllerParamsFromStr(std::unordered_map<controller::ControllerParameters, std::string> const & map)
    {
        auto data = ControllerConfig{};

        auto parse = [&](ControllerParameters p, auto& member, auto def, auto strToEnum)
        {
            auto itr = map.find(p);
            if (itr != map.end())
                member = strToEnum(itr->second);
            else
                member = def;
        };

        parse(ControllerParameters::Mode, data.controllerMode, defaultMode, parseControllerModeString);
        parse(ControllerParameters::ButtonMapping, data.controllerMap, defaultButtonMap, parseControllerButtonMapString);
        parse(ControllerParameters::LeftAnalogMapping, data.leftAnalogMap, defaultAnalogMap, parseControllerAnalogMapString);
        parse(ControllerParameters::RightAnalogMapping, data.rightAnalogMap, defaultAnalogMap, parseControllerAnalogMapString);

        {
            auto entry = map.find(ControllerParameters::ControllerCount);
            if(entry != map.end())
            {
                auto str = entry->second;
                auto num = (int16_t)std::atoi(str.c_str());
                data.controllerCount = std::clamp(num, minControllerCount, maxControllerCount);
            }
            else
                data.controllerCount = defaultControllerCount;
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
            auto entry = map.find(ControllerParameters::HomeButton);
            if(entry != map.end())
            {
                auto btnList = csvToList(entry->second);
                auto btns = controller::parseSwitchButtonStrings(btnList);
                auto btnFlags = 0U;
                for(auto const & btn : btns)
                    btnFlags |= btn;

                data.homeButton = btnFlags != 0 ? btnFlags : defaultHomeButton;
            }
            else
                data.homeButton = defaultHomeButton;
        }

        {
            auto entry = map.find(ControllerParameters::HomeButtonTriggerTime);
            if(entry != map.end())
            {
                auto secondsStr = entry->second;
                auto const seconds = std::atof(secondsStr.c_str());
                auto const nanoseconds = (uint32_t)timeutil::secondToNano(seconds);
                data.homeButtonTriggerTime = std::clamp(nanoseconds, minHomeButtonTriggerTime, maxHomeButtonTriggerTime);
            }
            else
                data.homeButtonTriggerTime = defaultHomeButtonTriggerTime;
        }

        return data;
    }
}