#pragma once
#include "SwitchStream.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Xinput.h>
#include <ViGEm/Client.h>
#include "ControllerButtonMap.h"
#include "SwitchControlsDefinitions.h"

class IVirtualController 
{
    public:
        IVirtualController();

        virtual bool Create() = 0;
        virtual void MapFaceButtons(ControllerButtonMap map) = 0;
        virtual void UpdateController() = 0;
        virtual void Print() = 0;
        virtual void ConvertPayload(GamepadDataPayload const data) = 0;
        virtual void UpdateState() = 0;
        virtual void ResetController() = 0;
        virtual void Disconnect() = 0;

    protected:
        PVIGEM_CLIENT client;
        PVIGEM_TARGET pad;

        virtual USHORT ConvertButtons(GamepadDataPayload const data) = 0;
        virtual SHORT ConvertAnalog(int32_t const switchAnalog) = 0;
};