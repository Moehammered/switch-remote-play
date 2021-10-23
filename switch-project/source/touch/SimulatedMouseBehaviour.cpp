#include "SimulatedMouseBehaviour.h"
#include "../utils/EnumMapper.h"

namespace touch
{
    std::string const SimulatedMouseBehaviourToStr(SimulatedMouseBehaviour const behaviour)
    {
        return enumToStr(SimulatedMouseBehaviourParamsDesc, behaviour);
    }

    SimulatedMouseBehaviour const SimulatedMouseBehaviourFromStr(std::string const str)
    {
        return strToEnum(SimulatedMouseBehaviourParamsDesc, str);
    }
}