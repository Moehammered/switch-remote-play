#include "SimulatedMouseBehaviour.h"
#include "../utils/EnumMapper.h"

namespace touch
{
    std::string const simulatedMouseBehaviourToStr(SimulatedMouseBehaviour const behaviour)
    {
        return enumToStr(simulatedMouseBehaviourParamsDesc, behaviour);
    }

    SimulatedMouseBehaviour const simulatedMouseBehaviourFromStr(std::string const str)
    {
        return strToEnum(simulatedMouseBehaviourParamsDesc, str);
    }
}