#pragma once
#include "VirtualController.h"

class DS4Controller : public IVirtualController
{
public:
    DS4Controller();

    bool Create() override;
    void Print() override;
    void MapFaceButtons(controller::ControllerButtonMap map) override;
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
    DS4_DPAD_DIRECTIONS directions;
    BYTE lt, rt;
    BYTE lx, ly;
    BYTE rx, ry;

    DS4_REPORT state;

    DS4_DPAD_DIRECTIONS ConvertDpad(GamepadDataPayload const data);
};
