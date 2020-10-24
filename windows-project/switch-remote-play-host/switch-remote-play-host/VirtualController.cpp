#include "VirtualController.h"
#include <iostream>
#include <sstream>
#include <string>

const Switch_HidControllerKeys ButtonsToCheck[] = {
	KEY_A, KEY_B, KEY_Y, KEY_X,
	KEY_L, KEY_R, KEY_MINUS, KEY_PLUS,
	KEY_LSTICK, KEY_RSTICK
};

std::unordered_map<Switch_HidControllerKeys, USHORT, std::hash<uint32_t>> SWITCH_TO_DS4_BTNS{
	{ KEY_A, DS4_BUTTON_CIRCLE },
	{ KEY_B, DS4_BUTTON_CROSS },
	{ KEY_X, DS4_BUTTON_TRIANGLE },
	{ KEY_Y, DS4_BUTTON_SQUARE },
	{ KEY_L, DS4_BUTTON_SHOULDER_LEFT },
	{ KEY_R, DS4_BUTTON_SHOULDER_RIGHT },
	{ KEY_MINUS, DS4_BUTTON_SHARE },
	{ KEY_PLUS , DS4_BUTTON_OPTIONS },
	{ KEY_LSTICK , DS4_BUTTON_THUMB_LEFT },
	{ KEY_RSTICK , DS4_BUTTON_THUMB_RIGHT }
};

const std::unordered_map < DS4_BUTTONS, std::string , std::hash<uint32_t> > DS4_BTN_STRINGS{
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
	{ DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_EAST, "L" },
	{ DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_WEST, "R" },
	{ DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_NORTH, "U" },
	{ DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_SOUTH, "D" },
	{ DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_NORTHEAST, "UL" },
	{ DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_NORTHWEST, "UR" },
	{ DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_SOUTHEAST, "DL" },
	{ DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_SOUTHWEST, "DR" }
};


DS4InputData ConvertToDS4(GamepadDataPayload data)
{
	DS4InputData converted = {};

	converted.directions = ConvertDpad(data);
	converted.buttons = ConvertButtons(data);
	converted.lx = ConvertAnalog(data.ljx);
	converted.ly = ConvertAnalog(-data.ljy);
	converted.rx = ConvertAnalog(data.rjx);
	converted.ry = ConvertAnalog(-data.rjy);

	converted.lt = 0; converted.rt = 0;
	if (data.keys & KEY_ZL)
		converted.lt = 255;
	if (data.keys & KEY_ZR)
		converted.rt = 255;

	return converted;
}

void PrintDS4Conversion(DS4InputData data)
{
	auto dpadStream = std::stringstream();

	dpadStream << "Direction: ";
	auto dpadFind = DS4_DIR_STRINGS.find(data.directions);
	if (dpadFind != DS4_DIR_STRINGS.end())
	{
		dpadStream << dpadFind->second;
	}

	auto btns = std::stringstream();
	btns << "Buttons: ";
	for (auto b : DS4_BTN_STRINGS)
	{
		auto mask = (USHORT)b.first & data.buttons;
		if (mask != 0)
			btns << b.second << " ";
	}

	auto axis = std::stringstream();
	axis << "Axis(L,R): ";
	axis << "(" << (USHORT)data.lx << "," << (USHORT)data.ly << ") , ";
	axis << "(" << (USHORT)data.rx << "," << (USHORT)data.ry << ")";

	std::cout << dpadStream.str() << std::endl << btns.str() << std::endl << axis.str() << std::endl << std::endl;
}

DS4_DPAD_DIRECTIONS ConvertDpad(GamepadDataPayload data)
{
	USHORT dpadInput = 0;
	//handle dpad
	if (data.keys & KEY_DDOWN)
	{
		if (data.keys & KEY_DLEFT)
			dpadInput = DS4_BUTTON_DPAD_SOUTHEAST;
		else if (data.keys & KEY_DRIGHT)
			dpadInput = DS4_BUTTON_DPAD_SOUTHWEST;
		else
			dpadInput = DS4_BUTTON_DPAD_SOUTH;
	}
	else if (data.keys & KEY_DUP)
	{
		if (data.keys & KEY_DLEFT)
			dpadInput = DS4_BUTTON_DPAD_NORTHEAST;
		else if (data.keys & KEY_DRIGHT)
			dpadInput = DS4_BUTTON_DPAD_NORTHWEST;
		else
			dpadInput = DS4_BUTTON_DPAD_NORTH;
	}
	else
	{
		// check for left / right
		if (data.keys & KEY_DLEFT)
			dpadInput = DS4_BUTTON_DPAD_WEST;
		else if (data.keys & KEY_DRIGHT)
			dpadInput = DS4_BUTTON_DPAD_EAST;
		else
			dpadInput = DS4_BUTTON_DPAD_NONE;
	}

	return (DS4_DPAD_DIRECTIONS)dpadInput;
}

USHORT ConvertButtons(GamepadDataPayload data)
{
	USHORT btns = {};
	//handle pressed buttons
	for (auto btn : ButtonsToCheck)
	{
		if ((btn & data.keys) != 0x0)
			btns |= SWITCH_TO_DS4_BTNS[btn];
	}

	return btns;
}

BYTE ConvertAnalog(int32_t switchAnalog)
{
	//std::cout << "Raw switch analog: " << switchAnalog;
	auto absolute = switchAnalog < 0 ? -switchAnalog : switchAnalog;
	const auto scale = (double)(0xFFFF / 2.0);
	double magnitude = absolute / scale;
	double scaled = magnitude * 128;
	BYTE result = 0x80;

	if (switchAnalog < 0)
		result -= (BYTE)scaled;
	else
		result += (BYTE)scaled;

	//std::cout << ", scaled: " << scaled << "(" << (USHORT)result << ")" << std::endl;
	return result;
}

bool VirtualController::Create()
{
	using namespace std;
	controllerClient = vigem_alloc();

	if (controllerClient == nullptr)
	{
		cout << "Failed to connect to driver" << endl;
		return false;
	}

	const auto result = vigem_connect(controllerClient);
	if (!VIGEM_SUCCESS(result))
	{
		vigem_free(controllerClient);
		cout << "Failed to connect bus for virtual controller with error code: 0x" << std::hex << result << endl;
		return false;
	}

	controllerPad = vigem_target_ds4_alloc();

	const auto pluginEvent = vigem_target_add(controllerClient, controllerPad);

	if (!VIGEM_SUCCESS(pluginEvent))
	{
		cout << "Failed to plugin for virtual controller with error code: 0x" << std::hex << pluginEvent << endl;
		vigem_disconnect(controllerClient);
		vigem_free(controllerClient);
		vigem_target_free(controllerPad);

		return false;
	}

	ZeroMemory(&controllerState, sizeof(DS4_REPORT));
	DS4_REPORT_INIT(&controllerState);

	return true;
}

void VirtualController::SetDpad(DS4_DPAD_DIRECTIONS directions)
{
	DS4_SET_DPAD(&controllerState, directions);
}

void VirtualController::SetButtons(USHORT btns)
{
	controllerState.wButtons = btns;
}

void VirtualController::SetAnalogAxis(BYTE leftX, BYTE leftY, BYTE rightX, BYTE rightY)
{
	controllerState.bThumbLX = leftX; controllerState.bThumbLY = leftY;
	controllerState.bThumbRX = rightX; controllerState.bThumbRY = rightY;
}

void VirtualController::SetShoulderTriggers(BYTE l, BYTE r)
{
	controllerState.bTriggerL = l; controllerState.bTriggerR = r;
}

void VirtualController::UpdateController()
{
	vigem_target_ds4_update(controllerClient, controllerPad, controllerState);
}

void VirtualController::ResetController()
{
	DS4_REPORT_INIT(&controllerState);
}

void VirtualController::Disconnect()
{
	vigem_target_remove(controllerClient, controllerPad);
	vigem_target_free(controllerPad);

	vigem_disconnect(controllerClient);
	vigem_free(controllerClient);
}
