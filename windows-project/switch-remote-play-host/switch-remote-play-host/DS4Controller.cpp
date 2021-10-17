#include "DS4Controller.h"
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

const HidNpadButton ButtonsToCheck_DS4[] = {
   HidNpadButton::HidNpadButton_A, HidNpadButton::HidNpadButton_B,
   HidNpadButton::HidNpadButton_Y, HidNpadButton::HidNpadButton_X,
   HidNpadButton::HidNpadButton_L, HidNpadButton::HidNpadButton_R, 
   HidNpadButton::HidNpadButton_Minus, HidNpadButton::HidNpadButton_Plus,
   HidNpadButton::HidNpadButton_StickL, HidNpadButton::HidNpadButton_StickR
};

std::unordered_map<HidNpadButton, DS4_SPECIAL_BUTTONS, std::hash<uint32_t>> const SWITCH_TO_SPECIAL_BTNS{
	{ HidNpadButton::HidNpadButton_Palma, DS4_SPECIAL_BUTTONS::DS4_SPECIAL_BUTTON_PS },
	{ HidNpadButton::HidNpadButton_29, DS4_SPECIAL_BUTTONS::DS4_SPECIAL_BUTTON_TOUCHPAD }
};

std::unordered_map<HidNpadButton, USHORT, std::hash<uint32_t>> const SWITCH_TO_DS4_FACE_BTNS{
   { HidNpadButton::HidNpadButton_A, DS4_BUTTON_CIRCLE },
   { HidNpadButton::HidNpadButton_B, DS4_BUTTON_CROSS },
   { HidNpadButton::HidNpadButton_X, DS4_BUTTON_TRIANGLE },
   { HidNpadButton::HidNpadButton_Y, DS4_BUTTON_SQUARE }
};

std::unordered_map<HidNpadButton, USHORT, std::hash<uint32_t>> SWITCH_TO_DS4_BTNS{
   { HidNpadButton::HidNpadButton_A, DS4_BUTTON_CIRCLE },
   { HidNpadButton::HidNpadButton_B, DS4_BUTTON_CROSS },
   { HidNpadButton::HidNpadButton_X, DS4_BUTTON_TRIANGLE },
   { HidNpadButton::HidNpadButton_Y, DS4_BUTTON_SQUARE },
   { HidNpadButton::HidNpadButton_L, DS4_BUTTON_SHOULDER_LEFT },
   { HidNpadButton::HidNpadButton_R, DS4_BUTTON_SHOULDER_RIGHT },
   { HidNpadButton::HidNpadButton_Minus, DS4_BUTTON_SHARE },
   { HidNpadButton::HidNpadButton_Plus, DS4_BUTTON_OPTIONS },
   { HidNpadButton::HidNpadButton_StickL, DS4_BUTTON_THUMB_LEFT },
   { HidNpadButton::HidNpadButton_StickR, DS4_BUTTON_THUMB_RIGHT }
};

const std::unordered_map < DS4_BUTTONS, std::string, std::hash<uint32_t> > DS4_BTN_STRINGS{
   { DS4_BUTTON_CIRCLE, "CIRCLE" },
   { DS4_BUTTON_CROSS, "CROSS" },
   { DS4_BUTTON_TRIANGLE, "TRIANGLE" },
   { DS4_BUTTON_SQUARE, "SQUARE" },
   { DS4_BUTTON_SHOULDER_LEFT, "L1" },
   { DS4_BUTTON_SHOULDER_RIGHT, "R1" },
   { DS4_BUTTON_SHARE, "SHARE" },
   { DS4_BUTTON_OPTIONS, "OPTIONS" },
   { DS4_BUTTON_THUMB_LEFT, "LEFT STICK" },
   { DS4_BUTTON_THUMB_RIGHT, "RIGHT STICK" }
};

const std::unordered_map < DS4_DPAD_DIRECTIONS, std::string, std::hash<uint32_t> > DS4_DIR_STRINGS{
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_NONE, "NONE" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_WEST, "L" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_EAST, "R" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_NORTH, "U" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_SOUTH, "D" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_NORTHWEST, "UL" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_NORTHEAST, "UR" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_SOUTHWEST, "DL" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_SOUTHEAST, "DR" }
};

DS4Controller::DS4Controller() 
	: buttons{}, directions{},
	lt{}, rt{}, lx{}, ly{},
	rx{}, ry{}, state{}, specialButtons{}
{
}

bool DS4Controller::Create()
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

	pad = vigem_target_ds4_alloc();

	const auto pluginEvent = vigem_target_add(client, pad);

	if (!VIGEM_SUCCESS(pluginEvent))
	{
		cout << "Failed to plugin for virtual controller with error code: 0x" << std::hex << pluginEvent << endl;
		vigem_disconnect(client);
		vigem_free(client);
		vigem_target_free(pad);

		return false;
	}

	ZeroMemory(&state, sizeof(DS4_REPORT));
	DS4_REPORT_INIT(&state);

	deviceIndex = vigem_target_get_index(pad);

	return true;
}

void DS4Controller::MapFaceButtons(controller::ControllerButtonMap map)
{
	auto& faceButtons = SWITCH_TO_DS4_FACE_BTNS;
	switch (map)
	{
		default:
		case controller::ControllerButtonMap::None:
		{
			auto x = HidNpadButton::HidNpadButton_X;
			auto y = HidNpadButton::HidNpadButton_Y;
			auto originalX = faceButtons.at(x);
			auto originalY = faceButtons.at(y);
			SWITCH_TO_DS4_BTNS[x] = originalX;
			SWITCH_TO_DS4_BTNS[y] = originalY;

			auto a = HidNpadButton::HidNpadButton_A;
			auto b = HidNpadButton::HidNpadButton_B;
			auto originalA = faceButtons.at(a);
			auto originalB = faceButtons.at(b);
			SWITCH_TO_DS4_BTNS[a] = originalA;
			SWITCH_TO_DS4_BTNS[b] = originalB;
		}
		break;

		case controller::ControllerButtonMap::InvertAB:
		{
			auto a = HidNpadButton::HidNpadButton_A;
			auto b = HidNpadButton::HidNpadButton_B;
			auto originalA = faceButtons.at(a);
			auto originalB = faceButtons.at(b);
			SWITCH_TO_DS4_BTNS[a] = originalB;
			SWITCH_TO_DS4_BTNS[b] = originalA;
		}
		break;

		case controller::ControllerButtonMap::InvertXY:
		{
			auto x = HidNpadButton::HidNpadButton_X;
			auto y = HidNpadButton::HidNpadButton_Y;
			auto originalX = faceButtons.at(x);
			auto originalY = faceButtons.at(y);
			SWITCH_TO_DS4_BTNS[x] = originalY;
			SWITCH_TO_DS4_BTNS[y] = originalX;
		}
		break;

		case controller::ControllerButtonMap::InvertXYAB:
		{
			auto x = HidNpadButton::HidNpadButton_X;
			auto y = HidNpadButton::HidNpadButton_Y;
			auto originalX = faceButtons.at(x);
			auto originalY = faceButtons.at(y);
			SWITCH_TO_DS4_BTNS[x] = originalY;
			SWITCH_TO_DS4_BTNS[y] = originalX;

			auto a = HidNpadButton::HidNpadButton_A;
			auto b = HidNpadButton::HidNpadButton_B;
			auto originalA = faceButtons.at(a);
			auto originalB = faceButtons.at(b);
			SWITCH_TO_DS4_BTNS[a] = originalB;
			SWITCH_TO_DS4_BTNS[b] = originalA;
		}
		break;
	}
}

void DS4Controller::ConvertPayload(GamepadDataPayload const data)
{
	directions = ConvertDpad(data);
	buttons = ConvertButtons(data);
	lx = (BYTE)ConvertAnalog(data.ljx * leftAxis.first);
	ly = (BYTE)ConvertAnalog(-data.ljy * leftAxis.second);
	rx = (BYTE)ConvertAnalog(data.rjx * rightAxis.first);
	ry = (BYTE)ConvertAnalog(-data.rjy * rightAxis.second);

	lt = 0; rt = 0;
	if (data.keys & HidNpadButton::HidNpadButton_ZL)
		lt = 255;
	if (data.keys & HidNpadButton::HidNpadButton_ZR)
		rt = 255;

	auto psButtonPressed = 0;
	for (auto& specials : SWITCH_TO_SPECIAL_BTNS)
	{
		if (specials.first & data.keys)
			psButtonPressed |= specials.second;
	}

	specialButtons = (DS4_SPECIAL_BUTTONS)psButtonPressed;
}

void DS4Controller::UpdateState()
{
	state.wButtons = buttons;
	DS4_SET_DPAD(&state, directions);
	state.bSpecial = specialButtons;
	state.bThumbLX = lx; state.bThumbLY = ly;
	state.bThumbRX = rx; state.bThumbRY = ry;
	state.bTriggerL = lt; state.bTriggerR = rt;
}

void DS4Controller::UpdateController()
{
	vigem_target_ds4_update(client, pad, state);
}

void DS4Controller::ResetController()
{
	DS4_REPORT_INIT(&state);
}

void DS4Controller::Disconnect()
{
	vigem_target_remove(client, pad);
	vigem_target_free(pad);

	vigem_disconnect(client);
	vigem_free(client);
}

USHORT DS4Controller::ConvertButtons(GamepadDataPayload const data)
{
	USHORT btns = {};
	//handle pressed buttons
	for (auto btn : ButtonsToCheck_DS4)
	{
		if ((btn & data.keys) != 0x0)
			btns |= SWITCH_TO_DS4_BTNS[btn];
	}

	return btns;
}

SHORT DS4Controller::ConvertAnalog(int32_t const switchAnalog)
{
	//std::cout << "Raw switch analog: " << switchAnalog;
	auto absolute = switchAnalog < 0 ? -switchAnalog : switchAnalog;
	const auto scale = (double)SHRT_MAX;
	double magnitude = absolute / scale;
	double scaled = magnitude * CHAR_MAX;
	BYTE result = 0x80;

	if (switchAnalog < 0)
		result -= (BYTE)scaled;
	else
		result += (BYTE)scaled;

	//std::cout << ", scaled: " << scaled << "(" << (USHORT)result << ")" << std::endl;
	return result;
}

DS4_DPAD_DIRECTIONS DS4Controller::ConvertDpad(GamepadDataPayload const data)
{
	USHORT dpadInput = 0;
	//handle dpad
	if (data.keys & HidNpadButton::HidNpadButton_Down)
	{
		if (data.keys & HidNpadButton::HidNpadButton_Left)
			dpadInput = DS4_BUTTON_DPAD_SOUTHWEST;
		else if (data.keys & HidNpadButton::HidNpadButton_Right)
			dpadInput = DS4_BUTTON_DPAD_SOUTHEAST;
		else
			dpadInput = DS4_BUTTON_DPAD_SOUTH;
	}
	else if (data.keys & HidNpadButton::HidNpadButton_Up)
	{
		if (data.keys & HidNpadButton::HidNpadButton_Left)
			dpadInput = DS4_BUTTON_DPAD_NORTHWEST;
		else if (data.keys & HidNpadButton::HidNpadButton_Right)
			dpadInput = DS4_BUTTON_DPAD_NORTHEAST;
		else
			dpadInput = DS4_BUTTON_DPAD_NORTH;
	}
	else
	{
		// check for left / right
		if (data.keys & HidNpadButton::HidNpadButton_Left)
			dpadInput = DS4_BUTTON_DPAD_WEST;
		else if (data.keys & HidNpadButton::HidNpadButton_Right)
			dpadInput = DS4_BUTTON_DPAD_EAST;
		else
			dpadInput = DS4_BUTTON_DPAD_NONE;
	}

	return (DS4_DPAD_DIRECTIONS)dpadInput;
}

void DS4Controller::Print()
{
	auto dpadStream = std::stringstream();

	dpadStream << "Direction: ";
	auto dpadFind = DS4_DIR_STRINGS.find(directions);
	if (dpadFind != DS4_DIR_STRINGS.end())
	{
		dpadStream << dpadFind->second;
	}

	auto btns = std::stringstream();
	btns << "Buttons: ";
	for (auto& b : DS4_BTN_STRINGS)
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
