#include "X360Controller.h"
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

const HidControllerKeys ButtonsToCheck_X360[] = {
   KEY_A, KEY_B, KEY_X, KEY_Y,
   KEY_L, KEY_R, KEY_MINUS, KEY_PLUS,
   KEY_LSTICK, KEY_RSTICK,
   KEY_DUP, KEY_DDOWN, KEY_DLEFT, KEY_DRIGHT
};

std::unordered_map<HidControllerKeys, USHORT, std::hash<uint32_t>> const SWITCH_TO_X360_FACE_BTNS{
	{ KEY_A, XUSB_GAMEPAD_B },
	{ KEY_B, XUSB_GAMEPAD_A },
	{ KEY_X, XUSB_GAMEPAD_Y },
	{ KEY_Y, XUSB_GAMEPAD_X }
};

std::unordered_map<HidControllerKeys, USHORT, std::hash<uint32_t>> SWITCH_TO_XUSB_BTNS{
   { KEY_A, XUSB_GAMEPAD_B },
   { KEY_B, XUSB_GAMEPAD_A },
   { KEY_X, XUSB_GAMEPAD_Y },
   { KEY_Y, XUSB_GAMEPAD_X },
   { KEY_L, XUSB_GAMEPAD_LEFT_SHOULDER },
   { KEY_R, XUSB_GAMEPAD_RIGHT_SHOULDER },
   { KEY_MINUS, XUSB_GAMEPAD_BACK },
   { KEY_PLUS , XUSB_GAMEPAD_START },
   { KEY_LSTICK , XUSB_GAMEPAD_LEFT_THUMB },
   { KEY_RSTICK , XUSB_GAMEPAD_RIGHT_THUMB },
   { KEY_DUP   , XUSB_GAMEPAD_DPAD_UP },
   { KEY_DDOWN , XUSB_GAMEPAD_DPAD_DOWN },
   { KEY_DLEFT , XUSB_GAMEPAD_DPAD_LEFT },
   { KEY_DRIGHT, XUSB_GAMEPAD_DPAD_RIGHT }
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
	rx{}, ry{}, state{}
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

void X360Controller::MapFaceButtons(ControllerButtonMap map)
{
	auto const& faceButtons = SWITCH_TO_X360_FACE_BTNS;
	switch (map)
	{
		default:
		case ControllerButtonMap::CONTROLLER_MAP_DEFAULT:
		{
			auto x = KEY_X;
			auto y = KEY_Y;
			auto originalX = faceButtons.at(x);
			auto originalY = faceButtons.at(y);
			SWITCH_TO_XUSB_BTNS[x] = originalX;
			SWITCH_TO_XUSB_BTNS[y] = originalY;

			auto a = KEY_A;
			auto b = KEY_B;
			auto originalA = faceButtons.at(a);
			auto originalB = faceButtons.at(b);
			SWITCH_TO_XUSB_BTNS[a] = originalA;
			SWITCH_TO_XUSB_BTNS[b] = originalB;
		}
		break;

		case ControllerButtonMap::CONTROLLER_MAP_INV_AB:
		{
			auto a = KEY_A;
			auto b = KEY_B;
			auto originalA = faceButtons.at(a);
			auto originalB = faceButtons.at(b);
			SWITCH_TO_XUSB_BTNS[a] = originalB;
			SWITCH_TO_XUSB_BTNS[b] = originalA;
		}
		break;

		case ControllerButtonMap::CONTROLLER_MAP_INV_XY:
		{
			auto x = KEY_X;
			auto y = KEY_Y;
			auto originalX = faceButtons.at(x);
			auto originalY = faceButtons.at(y);
			SWITCH_TO_XUSB_BTNS[x] = originalY;
			SWITCH_TO_XUSB_BTNS[y] = originalX;
		}
		break;

		case ControllerButtonMap::CONTROLLER_MAP_INV_XYAB:
		{
			auto x = KEY_X;
			auto y = KEY_Y;
			auto originalX = faceButtons.at(x);
			auto originalY = faceButtons.at(y);
			SWITCH_TO_XUSB_BTNS[x] = originalY;
			SWITCH_TO_XUSB_BTNS[y] = originalX;

			auto a = KEY_A;
			auto b = KEY_B;
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
	lx = ConvertAnalog(data.ljx);
	ly = ConvertAnalog(data.ljy);
	rx = ConvertAnalog(data.rjx);
	ry = ConvertAnalog(data.rjy);

	lt = 0; rt = 0;
	if (data.keys & KEY_ZL)
		lt = 255;
	if (data.keys & KEY_ZR)
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
