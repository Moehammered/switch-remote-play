#include "KeyboardOptions.h"

namespace keyboard
{
    std::unordered_map<KeyParameter, std::string> const KeybindingsToString(KeyboardConfig const config)
    {
        auto values = std::unordered_map<KeyParameter, std::string>{};

        for(auto const& binding : config.bindings)
        {
            if(binding.button != 0)
                values[binding.key] = controller::SwitchButtonToString((HidNpadButton)binding.button);
        }

        return values;
    }

    KeyboardConfig const KeybindingsFromStr(std::unordered_map<KeyParameter, std::string> const & map)
    {
        auto config = KeyboardConfig{};

        auto parseBinding = [&](KeyParameter const key)
        {
            auto entry = map.find(key);
            if(entry != map.end())
                return (uint32_t)controller::ParseSwitchButtonString(entry->second);
            else
                return 0U;
        };

        for(auto i = 0; i < TotalSupportedKeys; ++i)
        {
            auto key = (KeyParameter)i;
            config.bindings[i].key = key;
            config.bindings[i].button = parseBinding(key);
        }

        return config;
    }
}