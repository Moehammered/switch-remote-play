#pragma once
#include "SwitchStream.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Xinput.h>
#include <ViGEm/Client.h>
//#pragma comment(lib, "setupapi.lib")

void initTest();

#include "SwitchControlsDefinitions.h"
#include <unordered_map>

struct DS4InputData
{
	USHORT buttons;
	DS4_DPAD_DIRECTIONS directions;
	BYTE lt, rt;
	BYTE lx, ly;
	BYTE rx, ry;
};

DS4InputData ConvertToDS4(GamepadDataPayload data);
void PrintDS4Conversion(DS4InputData data);

DS4_DPAD_DIRECTIONS ConvertDpad(GamepadDataPayload data);
USHORT ConvertButtons(GamepadDataPayload data);
BYTE ConvertAnalog(int32_t switchAnalog);


class VirtualController
{
public:
	bool Create();

	void SetDpad(DS4_DPAD_DIRECTIONS directions);
	void SetButtons(USHORT btns);
	void SetAnalogAxis(BYTE leftX, BYTE leftY, BYTE rightX, BYTE rightY);
	void SetShoulderTriggers(BYTE l, BYTE r);
	void UpdateController();
	void ResetController();
	void Disconnect();

private:
	DS4_REPORT controllerState;
	PVIGEM_CLIENT controllerClient;
	PVIGEM_TARGET controllerPad;
};