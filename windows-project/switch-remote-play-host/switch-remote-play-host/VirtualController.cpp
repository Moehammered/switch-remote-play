#include "VirtualController.h"
#include <iostream>
#include <sstream>
#include <string>

const Switch_HidControllerKeys ButtonsToCheck[] = {
	KEY_A, KEY_B, KEY_X, KEY_Y,
	KEY_L, KEY_R, KEY_MINUS, KEY_PLUS,
	KEY_LSTICK, KEY_RSTICK,
	KEY_DUP, KEY_DDOWN, KEY_DLEFT, KEY_DRIGHT
};

std::unordered_map<Switch_HidControllerKeys, USHORT, std::hash<uint32_t>> SWITCH_TO_XUSB_BTNS{
	{ KEY_A, XUSB_GAMEPAD_B },
	{ KEY_B, XUSB_GAMEPAD_A },
	{ KEY_X, XUSB_GAMEPAD_Y },
	{ KEY_Y, XUSB_GAMEPAD_X },
	{ KEY_L, XUSB_GAMEPAD_LEFT_SHOULDER },
	{ KEY_R, XUSB_GAMEPAD_RIGHT_SHOULDER },
	{ KEY_MINUS, XUSB_GAMEPAD_GUIDE },
	{ KEY_PLUS , XUSB_GAMEPAD_START },
	{ KEY_LSTICK , XUSB_GAMEPAD_LEFT_THUMB },
	{ KEY_RSTICK , XUSB_GAMEPAD_RIGHT_THUMB },
	{ KEY_DUP   , XUSB_GAMEPAD_DPAD_UP },
	{ KEY_DDOWN , XUSB_GAMEPAD_DPAD_DOWN },
	{ KEY_DLEFT , XUSB_GAMEPAD_DPAD_LEFT },
	{ KEY_DRIGHT, XUSB_GAMEPAD_DPAD_RIGHT },
};

const std::unordered_map < XUSB_BUTTON, std::string, std::hash<uint32_t> > XUSB_BTN_STRINGS{
	{ XUSB_GAMEPAD_A, "A" },
	{ XUSB_GAMEPAD_B, "B" },
	{ XUSB_GAMEPAD_X, "X" },
	{ XUSB_GAMEPAD_Y, "Y" },
	{ XUSB_GAMEPAD_LEFT_SHOULDER , "L1" },
	{ XUSB_GAMEPAD_RIGHT_SHOULDER, "R1" },
	{ XUSB_GAMEPAD_GUIDE, "GUIDE" },
	{ XUSB_GAMEPAD_START, "START" },
	{ XUSB_GAMEPAD_LEFT_THUMB , "LEFT STICK" },
	{ XUSB_GAMEPAD_RIGHT_THUMB, "RIGHT STICK" },
	{ XUSB_GAMEPAD_DPAD_UP   , "DPAD UP" },
	{ XUSB_GAMEPAD_DPAD_DOWN , "DPAD DOWN" },
	{ XUSB_GAMEPAD_DPAD_LEFT , "DPAD LEFT" },
	{ XUSB_GAMEPAD_DPAD_RIGHT, "DPAD RIGHT" },
};

XUSBInputData ConvertToXUSB(GamepadDataPayload const data)
{
	XUSBInputData converted = {};

	converted.buttons = ConvertButtons(data);
	converted.lx = ConvertAnalog(data.ljx);
	converted.ly = ConvertAnalog(data.ljy);
	converted.rx = ConvertAnalog(data.rjx);
	converted.ry = ConvertAnalog(data.rjy);

	converted.lt = 0; converted.rt = 0;
	if (data.keys & KEY_ZL)
		converted.lt = 255;
	if (data.keys & KEY_ZR)
		converted.rt = 255;

	return converted;
}

void PrintXUSBConversion(XUSBInputData const data)
{
	auto dpadStream = std::stringstream();

	auto btns = std::stringstream();
	btns << "Buttons: ";
	for (auto b : XUSB_BTN_STRINGS)
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

USHORT ConvertButtons(GamepadDataPayload const data)
{
	USHORT btns = {};
	//handle pressed buttons
	for (auto btn : ButtonsToCheck)
	{
		if ((btn & data.keys) != 0x0)
			btns |= SWITCH_TO_XUSB_BTNS[btn];
	}

	return btns;
}

SHORT ConvertAnalog(int32_t const switchAnalog)
{
#ifndef RELEASE
	std::cout << "Raw switch analog: " << switchAnalog;
#endif
	auto absolute = switchAnalog < 0 ? -switchAnalog : switchAnalog;
	const auto scale = (double)(0xFFFF / 2.0);
	double magnitude = absolute / scale;
	double scaled = magnitude * 32768;
	SHORT result = 0x0000;

	if (switchAnalog < 0)
		result -= (SHORT)scaled;
	else
		result += (SHORT)scaled;

#ifndef RELEASE
	std::cout << ", scaled: " << scaled << "(" << (USHORT)result << ")" << std::endl;
#endif
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

	controllerPad = vigem_target_x360_alloc();

	const auto pluginEvent = vigem_target_add(controllerClient, controllerPad);

	if (!VIGEM_SUCCESS(pluginEvent))
	{
		cout << "Failed to plugin for virtual controller with error code: 0x" << std::hex << pluginEvent << endl;
		vigem_disconnect(controllerClient);
		vigem_free(controllerClient);
		vigem_target_free(controllerPad);

		return false;
	}

	ZeroMemory(&controllerState, sizeof(XUSB_REPORT));
	XUSB_REPORT_INIT(&controllerState);

	return true;
}

void VirtualController::SetButtons(USHORT btns)
{
	controllerState.wButtons = btns;
}

void VirtualController::SetAnalogAxis(SHORT leftX, SHORT leftY, SHORT rightX, SHORT rightY)
{
	controllerState.sThumbLX = leftX; controllerState.sThumbLY = leftY;
	controllerState.sThumbRX = rightX; controllerState.sThumbRY = rightY;
}

void VirtualController::SetShoulderTriggers(BYTE l, BYTE r)
{
	controllerState.bLeftTrigger = l; controllerState.bRightTrigger = r;
}

void VirtualController::UpdateController()
{
	vigem_target_x360_update(controllerClient, controllerPad, controllerState);
}

void VirtualController::ResetController()
{
	XUSB_REPORT_INIT(&controllerState);
}

void VirtualController::Disconnect()
{
	vigem_target_remove(controllerClient, controllerPad);
	vigem_target_free(controllerPad);

	vigem_disconnect(controllerClient);
	vigem_free(controllerClient);
}
