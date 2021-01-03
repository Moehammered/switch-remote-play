#pragma once
#include "VirtualController.h"

class X360Controller : public IVirtualController
{
	public:
		X360Controller();

		bool Create() override;
		void Print() override;
		void MapFaceButtons(ControllerButtonMap map) override;
		void ConvertPayload(GamepadDataPayload const data) override;
		void UpdateState() override;
		void UpdateController() override;
		void ResetController() override;
		void Disconnect() override;

	protected:
		USHORT ConvertButtons(GamepadDataPayload const data) override;
		SHORT ConvertAnalog(int32_t const switchAnalog) override;

	private:
		USHORT buttons;
		BYTE lt, rt;
		SHORT lx, ly;
		SHORT rx, ry;

		XUSB_REPORT state;
};