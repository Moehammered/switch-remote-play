#ifndef __SIMULATEDMOUSEBEHAVIOUR_H__
#define __SIMULATEDMOUSEBEHAVIOUR_H__

#include <vector>
#include <unordered_map>
#include <string>

namespace touch
{
    enum class SimulatedMouseBehaviour
    {
        Trackpad,
        Absolute
    };

    static std::vector<SimulatedMouseBehaviour> const simulatedMouseBehaviourParamsList
    {
        SimulatedMouseBehaviour::Trackpad, SimulatedMouseBehaviour::Absolute
    };

    static std::unordered_map<SimulatedMouseBehaviour, std::string> const simulatedMouseBehaviourParamsDesc
    {
        { SimulatedMouseBehaviour::Trackpad, "Trackpad" },
        { SimulatedMouseBehaviour::Absolute, "Absolute" }
    };

    auto constexpr defaultMouseBehaviour = SimulatedMouseBehaviour::Trackpad;

    std::string const simulatedMouseBehaviourToStr(SimulatedMouseBehaviour const behaviour);
    SimulatedMouseBehaviour const simulatedMouseBehaviourFromStr(std::string const str);
}

#endif
