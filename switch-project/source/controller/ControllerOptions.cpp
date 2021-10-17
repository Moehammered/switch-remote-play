#include "ControllerOptions.h"
#include <algorithm>
#include <sstream>

namespace controller
{
    std::unordered_map<controller::Parameters, std::string> const ControllerParamsToStr(ControllerConfig const data)
    {
        auto values = std::unordered_map<controller::Parameters, std::string>{};

        values[Parameters::Mode] = ControllerModeToString(data.controllerMode);
        values[Parameters::ButtonMapping] = ControllerButtonMapToString(data.controllerMap);
        values[Parameters::LeftAnalogMapping] = ControllerAnalogMapToString(data.leftAnalogMap);
        values[Parameters::RightAnalogMapping] = ControllerAnalogMapToString(data.rightAnalogMap);
        values[Parameters::ControllerCount] = std::to_string(data.controllerCount);

        auto btns = controller::SwitchButtonsToString(data.homeButton);
        if(btns.size() != 0)
        {
            auto str = btns[0];
            for(auto i = 1U; i < btns.size(); ++i)
                str += "," + btns[i];
            
            values[Parameters::HomeButton] = str;
        }

        auto const triggerTimeSeconds = timeutil::nanoToSecond(data.homeButtonTriggerTime);
        auto secondsStr = std::to_string(triggerTimeSeconds);
        values[Parameters::HomeButtonTriggerTime] = secondsStr;

        return values;
    }

    ControllerConfig const ControllerParamsFromStr(std::unordered_map<controller::Parameters, std::string> const & map)
    {
        auto data = ControllerConfig{};

        auto parse = [&](Parameters p, auto& member, auto def, auto strToEnum)
        {
            auto itr = map.find(p);
            if (itr != map.end())
                member = strToEnum(itr->second);
            else
                member = def;
        };

        parse(Parameters::Mode, data.controllerMode, DefaultMode, ParseControllerModeString);
        parse(Parameters::ButtonMapping, data.controllerMap, DefaultButtonMap, ParseControllerButtonMapString);
        parse(Parameters::LeftAnalogMapping, data.leftAnalogMap, DefaultAnalogMap, ParseControllerAnalogMapString);
        parse(Parameters::RightAnalogMapping, data.rightAnalogMap, DefaultAnalogMap, ParseControllerAnalogMapString);

        {
            auto entry = map.find(Parameters::ControllerCount);
            if(entry != map.end())
            {
                auto str = entry->second;
                auto num = (int16_t)std::atoi(str.c_str());
                data.controllerCount = std::clamp(num, MinControllerCount, MaxControllerCount);
            }
            else
                data.controllerCount = DefaultControllerCount;
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
            auto entry = map.find(Parameters::HomeButton);
            if(entry != map.end())
            {
                auto btnList = csvToList(entry->second);
                auto btns = controller::ParseSwitchButtonStrings(btnList);
                auto btnFlags = 0U;
                for(auto const & btn : btns)
                    btnFlags |= btn;

                data.homeButton = btnFlags != 0 ? btnFlags : DefaultHomeButton;
            }
            else
                data.homeButton = DefaultHomeButton;
        }

        {
            auto entry = map.find(Parameters::HomeButtonTriggerTime);
            if(entry != map.end())
            {
                auto secondsStr = entry->second;
                auto const seconds = std::atof(secondsStr.c_str());
                auto const nanoseconds = (uint32_t)timeutil::secondToNano(seconds);
                data.homeButtonTriggerTime = std::clamp(nanoseconds, MinHomeButtonTriggerTime, MaxHomeButtonTriggerTime);
            }
            else
                data.homeButtonTriggerTime = DefaultHomeButtonTriggerTime;
        }

        return data;
    }
}