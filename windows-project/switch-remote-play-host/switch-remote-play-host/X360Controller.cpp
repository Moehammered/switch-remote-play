#include "X360Controller.h"
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

const HidNpadButton ButtonsToCheck_X360[] = {
   HidNpadButton::HidNpadButton_A, HidNpadButton::HidNpadButton_B,
   HidNpadButton::HidNpadButton_X, HidNpadButton::HidNpadButton_Y,
   HidNpadButton::HidNpadButton_L, HidNpadButton::HidNpadButton_R,
   HidNpadButton::HidNpadButton_Minus, HidNpadButton::HidNpadButton_Plus,
   HidNpadButton::HidNpadButton_StickL, HidNpadButton::HidNpadButton_StickR,
   HidNpadButton::HidNpadButton_Up, HidNpadButton::HidNpadButton_Down,
   HidNpadButton::HidNpadButton_Left, HidNpadButton::HidNpadButton_Right,
   HidNpadButton::HidNpadButton_Palma
};

std::unordered_map<HidNpadButton, USHORT, std::hash<uint32_t>> const SWITCH_TO_X360_FACE_BTNS{
	{ HidNpadButton::HidNpadButton_A, XUSB_GAMEPAD_B },
	{ HidNpadButton::HidNpadButton_B, XUSB_GAMEPAD_A },
	{ HidNpadButton::HidNpadButton_X, XUSB_GAMEPAD_Y },
	{ HidNpadButton::HidNpadButton_Y, XUSB_GAMEPAD_X }
};

std::unordered_map<HidNpadButton, USHORT, std::hash<uint32_t>> SWITCH_TO_XUSB_BTNS{
   { HidNpadButton::HidNpadButton_A, XUSB_GAMEPAD_B },
   { HidNpadButton::HidNpadButton_B, XUSB_GAMEPAD_A },
   { HidNpadButton::HidNpadButton_X, XUSB_GAMEPAD_Y },
   { HidNpadButton::HidNpadButton_Y, XUSB_GAMEPAD_X },
   { HidNpadButton::HidNpadButton_L, XUSB_GAMEPAD_LEFT_SHOULDER },
   { HidNpadButton::HidNpadButton_R, XUSB_GAMEPAD_RIGHT_SHOULDER },
   { HidNpadButton::HidNpadButton_Minus, XUSB_GAMEPAD_BACK },
   { HidNpadButton::HidNpadButton_Plus, XUSB_GAMEPAD_START },
   { HidNpadButton::HidNpadButton_StickL, XUSB_GAMEPAD_LEFT_THUMB },
   { HidNpadButton::HidNpadButton_StickR, XUSB_GAMEPAD_RIGHT_THUMB },
   { HidNpadButton::HidNpadButton_Up, XUSB_GAMEPAD_DPAD_UP },
   { HidNpadButton::HidNpadButton_Down, XUSB_GAMEPAD_DPAD_DOWN },
   { HidNpadButton::HidNpadButton_Left, XUSB_GAMEPAD_DPAD_LEFT },
   { HidNpadButton::HidNpadButton_Right, XUSB_GAMEPAD_DPAD_RIGHT },
   { HidNpadButton::HidNpadButton_Palma, XUSB_GAMEPAD_GUIDE }
};

const std::unordered_map < XUSB_BUTTON, std::string, std::hash<uint32_t> > XUSB_BTN_STRINGS{
   { XUSB_GAMEPAD_A, "A" },
   { XUSB_GAMEPAD_B, "B" },
   { XUSB_GAMEPAD_X, "X" },
   { XUSB_GAMEPAD_Y, "Y" },
   { XUSB_GAMEPAD_LEFT_SHOULDER , "L1" },
   { XUSB_GAMEPAD_RIGHT_SHOULDER, "R1" },
   { XUSB_GAMEPAD_GUIDE, "GUIDE" },
   { XUSB_GAMEPAD_BACK, "BACK" },
   { XUSB_GAMEPAD_START, "START" },
   { XUSB_GAMEPAD_LEFT_THUMB , "LEFT STICK" },
   { XUSB_GAMEPAD_RIGHT_THUMB, "RIGHT STICK" },
   { XUSB_GAMEPAD_DPAD_UP   , "DPAD UP" },
   { XUSB_GAMEPAD_DPAD_DOWN , "DPAD DOWN" },
   { XUSB_GAMEPAD_DPAD_LEFT , "DPAD LEFT" },
   { XUSB_GAMEPAD_DPAD_RIGHT, "DPAD RIGHT" }
};

X360Controller::X360Controller()
	: buttons{}, lt{}, rt{}, lx{}, ly{},
	rx{}, ry{}, 
	state{}
{
}

bool X360Controller::Create()
{
	using namespace std;
	client = vigem_alloc();

	if (client == nullptr)
	{
		cout << "Failed to connect to driver" << endl;
		return false;
	}

	const auto result = vigem_connect(client);
	if (!VIGEM_SUCCESS(result))
	{
		vigem_free(client);
		cout << "Failed to connect bus for virtual controller with error code: 0x" << std::hex << result << endl;
		return false;
	}

	pad = vigem_target_x360_alloc();

	const auto pluginEvent = vigem_target_add(client, pad);

	if (!VIGEM_SUCCESS(pluginEvent))
	{
		cout << "Failed to plugin for virtual controller with error code: 0x" << std::hex << pluginEvent << endl;
		vigem_disconnect(client);
		vigem_free(client);
		vigem_target_free(pad);

		return false;
	}

	ZeroMemory(&state, sizeof(XUSB_REPORT));
	XUSB_REPORT_INIT(&state);

	return true;
}

void X360Controller::MapFaceButtons(controller::ControllerButtonMap map)
{
	auto const& faceButtons = SWITCH_TO_X360_FACE_BTNS;
	switch (map)
	{
		default:
		case controller::ControllerButtonMap::None:
		{
			auto x = HidNpadButton::HidNpadButton_X;
			auto y = HidNpadButton::HidNpadButton_Y;
			auto originalX = faceButtons.at(x);
			auto originalY = faceButtons.at(y);
			SWITCH_TO_XUSB_BTNS[x] = originalX;
			SWITCH_TO_XUSB_BTNS[y] = originalY;

			auto a = HidNpadButton::HidNpadButton_A;
			auto b = HidNpadButton::HidNpadButton_B;
			auto originalA = faceButtons.at(a);
			auto originalB = faceButtons.at(b);
			SWITCH_TO_XUSB_BTNS[a] = originalA;
			SWITCH_TO_XUSB_BTNS[b] = originalB;
		}
		break;

		case controller::ControllerButtonMap::InvertAB:
		{
			auto a = HidNpadButton::HidNpadButton_A;
			auto b = HidNpadButton::HidNpadButton_B;
			auto originalA = faceButtons.at(a);
			auto originalB = faceButtons.at(b);
			SWITCH_TO_XUSB_BTNS[a] = originalB;
			SWITCH_TO_XUSB_BTNS[b] = originalA;
		}
		break;

		case controller::ControllerButtonMap::InvertXY:
		{
			auto x = HidNpadButton::HidNpadButton_X;
			auto y = HidNpadButton::HidNpadButton_Y;
			auto originalX = faceButtons.at(x);
			auto originalY = faceButtons.at(y);
			SWITCH_TO_XUSB_BTNS[x] = originalY;
			SWITCH_TO_XUSB_BTNS[y] = originalX;
		}
		break;

		case controller::ControllerButtonMap::InvertXYAB:
		{
			auto x = HidNpadButton::HidNpadButton_X;
			auto y = HidNpadButton::HidNpadButton_Y;
			auto originalX = faceButtons.at(x);
			auto originalY = faceButtons.at(y);
			SWITCH_TO_XUSB_BTNS[x] = originalY;
			SWITCH_TO_XUSB_BTNS[y] = originalX;

			auto a = HidNpadButton::HidNpadButton_A;
			auto b = HidNpadButton::HidNpadButton_B;
			auto originalA = faceButtons.at(a);
			auto originalB = faceButtons.at(b);
			SWITCH_TO_XUSB_BTNS[a] = originalB;
			SWITCH_TO_XUSB_BTNS[b] = originalA;
		}
		break;
	}
}

void X360Controller::ConvertPayload(GamepadDataPayload const data)
{
	buttons = ConvertButtons(data);
	lx = ConvertAnalog(data.ljx * leftAxis.first);
	ly = ConvertAnalog(data.ljy * leftAxis.second);
	rx = ConvertAnalog(data.rjx * rightAxis.first);
	ry = ConvertAnalog(data.rjy * rightAxis.second);

	lt = 0; rt = 0;
	if (data.keys & HidNpadButton::HidNpadButton_ZL)
		lt = 255;
	if (data.keys & HidNpadButton::HidNpadButton_ZR)
		rt = 255;
}

void X360Controller::UpdateState()
{
	state.wButtons = buttons;
	state.sThumbLX = lx; state.sThumbLY = ly;
	state.sThumbRX = rx; state.sThumbRY = ry;
	state.bLeftTrigger = lt; state.bRightTrigger = rt;
}

void X360Controller::UpdateController()
{
	vigem_target_x360_update(client, pad, state);
}

void X360Controller::ResetController()
{
	XUSB_REPORT_INIT(&state);
}

void X360Controller::Disconnect()
{
	vigem_target_remove(client, pad);
	vigem_target_free(pad);

	vigem_disconnect(client);
	vigem_free(client);
}

void X360Controller::Print()
{
	auto dpadStream = std::stringstream();

	auto btns = std::stringstream();
	btns << "Buttons: ";
	for (auto& b : XUSB_BTN_STRINGS)
	{
		auto mask = (USHORT)b.first & buttons;
		if (mask != 0)
			btns << b.second << " ";
	}

	auto axis = std::stringstream();
	axis << "Axis(L,R): ";
	axis << "(" << (USHORT)lx << "," << (USHORT)ly << ") , ";
	axis << "(" << (USHORT)rx << "," << (USHORT)ry << ")";

	std::cout << dpadStream.str() << std::endl << btns.str() << std::endl << axis.str() << std::endl << std::endl;
}

USHORT X360Controller::ConvertButtons(GamepadDataPayload const data)
{
	USHORT btns = {};
	//handle pressed buttons
	for (auto btn : ButtonsToCheck_X360)
	{
		if ((btn & data.keys) != 0x0)
			btns |= SWITCH_TO_XUSB_BTNS[btn];
	}

	return btns;
}

SHORT X360Controller::ConvertAnalog(int32_t const switchAnalog)
{
	//std::cout << "Raw switch analog: " << switchAnalog;
	auto absolute = switchAnalog < 0 ? -switchAnalog : switchAnalog;
	const auto scale = (double)SHRT_MAX;
	double magnitude = absolute / scale;
	double scaled = magnitude * SHRT_MAX;
	SHORT result = 0x0000;

	if (switchAnalog < 0)
		result -= (SHORT)scaled;
	else
		result += (SHORT)scaled;

	//std::cout << ", scaled: " << scaled << "(" << (USHORT)result << ")" << std::endl;
	return result;
}
