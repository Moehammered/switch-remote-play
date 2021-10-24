#include "KeyboardOptions.h"
#include "../utils/StringUtils.h"
#include <sstream>

namespace keyboard
{
    std::unordered_map<KeyParameter, std::string> const keybindingsToString(KeyboardConfig const config)
    {
        auto values = std::unordered_map<KeyParameter, std::string>{};

        for(auto const& binding : config.bindings)
        {
            if(binding.button != 0)
            {
                auto btns = controller::switchButtonsToString(binding.button);
                if(btns.size() != 0)
                {
                    auto str = btns[0];
                    for(auto i = 1U; i < btns.size(); ++i)
                        str += "," + btns[i];
                    
                    values[binding.key] = str;
                }
            }
            else
                values[binding.key] = "none";
        }

        return values;
    }

    KeyboardConfig const keybindingsFromStr(std::unordered_map<KeyParameter, std::string> const & map)
    {
        auto config = KeyboardConfig{};

        auto csvToList = [](auto const & csv)
        {
            std::stringstream ss(csv);
            std::string val{};
            auto list = std::vector<std::string>{};
            while(std::getline(ss, val, ','))
                list.emplace_back(val);
            
            return list;
        };

        auto parseBinding = [&](KeyParameter const key)
        {
            auto entry = map.find(key);
            if(entry != map.end() && entry->second != "none")
            {
                auto btnList = csvToList(entry->second);
                auto btns = controller::parseSwitchButtonStrings(btnList);
                auto btnFlags = 0U;
                for(auto const & btn : btns)
                    btnFlags |= btn;

                return btnFlags;
            }
            else
                return 0U;
        };

        for(auto i = 0U; i < totalSupportedKeys; ++i)
        {
            auto key = (KeyParameter)i;
            config.bindings[i].key = key;
            config.bindings[i].button = parseBinding(key);
        }

        return config;
    }
}