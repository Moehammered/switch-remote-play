#pragma once
#include "SwitchStream.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Xinput.h>
#include <ViGEm/Client.h>
#include "SwitchControlsDefinitions.h"
#include <unordered_map>

struct XUSBInputData
{
	USHORT buttons;
	BYTE lt, rt;
	SHORT lx, ly;
	SHORT rx, ry;
};

XUSBInputData ConvertToXUSB(GamepadDataPayload const data);
void PrintXUSBConversion(XUSBInputData const data);

USHORT ConvertButtons(GamepadDataPayload const data);
SHORT ConvertAnalog(int32_t const switchAnalog);


class VirtualController
{
public:
	bool Create();

	void SetButtons(USHORT btns);
	void SetAnalogAxis(SHORT leftX, SHORT leftY, SHORT rightX, SHORT rightY);
	void SetShoulderTriggers(BYTE l, BYTE r);
	void UpdateController();
	void ResetController();
	void Disconnect();

private:
	XUSB_REPORT controllerState;
	PVIGEM_CLIENT controllerClient;
	PVIGEM_TARGET controllerPad;
};