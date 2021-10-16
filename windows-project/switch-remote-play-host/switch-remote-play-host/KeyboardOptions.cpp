#include "KeyboardOptions.h"
#include "StringUtils.h"
#include <sstream>

namespace keyboard
{
    std::unordered_map<KeyParameter, std::string> const KeybindingsToString(KeyboardConfig const config)
    {
        auto values = std::unordered_map<KeyParameter, std::string>{};

        for(auto const& binding : config.bindings)
        {
            if(binding.button != 0)
            {
                auto btns = controller::SwitchButtonsToString(binding.button);
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

    KeyboardConfig const KeybindingsFromStr(std::unordered_map<KeyParameter, std::string> const & map)
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
                auto btns = controller::ParseSwitchButtonStrings(btnList);
                auto btnFlags = 0U;
                for(auto const & btn : btns)
                    btnFlags |= btn;

                return btnFlags;
            }
            else
                return 0U;
        };

        for(auto i = 0U; i < TotalSupportedKeys; ++i)
        {
            auto key = (KeyParameter)i;
            config.bindings[i].key = key;
            config.bindings[i].button = parseBinding(key);
        }

        return config;
    }
}