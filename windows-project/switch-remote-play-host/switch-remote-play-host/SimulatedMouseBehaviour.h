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

    static std::vector<SimulatedMouseBehaviour> const SimulatedMouseBehaviourParamsList
    {
        SimulatedMouseBehaviour::Trackpad, SimulatedMouseBehaviour::Absolute
    };

    static std::unordered_map<SimulatedMouseBehaviour, std::string> const SimulatedMouseBehaviourParamsDesc
    {
        { SimulatedMouseBehaviour::Trackpad, "Trackpad" },
        { SimulatedMouseBehaviour::Absolute, "Absolute" }
    };

    SimulatedMouseBehaviour constexpr DefaultMouseBehaviour = SimulatedMouseBehaviour::Trackpad;

    std::string const SimulatedMouseBehaviourToStr(SimulatedMouseBehaviour const behaviour);
    SimulatedMouseBehaviour const SimulatedMouseBehaviourFromStr(std::string const str);
}

#endif
