#include "VirtualController.h"

IVirtualController::IVirtualController() 
	: client{ nullptr }, pad{ nullptr },
	leftAxis{ 1, 1 }, rightAxis{ 1, 1 }
{
}

void IVirtualController::MapAnalogAxis(controller::ControllerAnalogMap left, controller::ControllerAnalogMap right)
{
	auto parseMap = [](controller::ControllerAnalogMap map)
	{
		switch (map)
		{
		default:
		case controller::ControllerAnalogMap::None:
			return std::pair<int32_t, int32_t>(1, 1);

		case controller::ControllerAnalogMap::InvertX:
			return std::pair<int32_t, int32_t>(-1, 1);

		case controller::ControllerAnalogMap::InvertY:
			return std::pair<int32_t, int32_t>(1, -1);

		case controller::ControllerAnalogMap::InvertXY:
			return std::pair<int32_t, int32_t>(-1, -1);
		}
	};

	leftAxis = parseMap(left);
	rightAxis = parseMap(right);
}
