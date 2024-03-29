#ifndef __VIRTUALCONTROLLER_H__
#define __VIRTUALCONTROLLER_H__

#include "srp/controller/ControllerButtonMap.h"
#include "srp/controller/ControllerAnalogMap.h"
#include "srp/network/NetworkData.h"
#include "srp/controller/SwitchHidDefinition.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include <ViGEm/Client.h>

class IVirtualController 
{
    public:
        IVirtualController();

        virtual bool Create() = 0;
        void MapAnalogAxis(controller::ControllerAnalogMap left, controller::ControllerAnalogMap right);
        virtual void MapFaceButtons(controller::ControllerButtonMap map) = 0;
        virtual void UpdateController() = 0;
        virtual void Print() = 0;
        virtual void ConvertPayload(GamepadDataPayload const data) = 0;
        virtual void UpdateState() = 0;
        virtual void ResetController() = 0;
        virtual void Disconnect() = 0;

        uint32_t const DeviceIndex() const;

    protected:
        PVIGEM_CLIENT client;
        PVIGEM_TARGET pad;

        std::pair<int32_t, int32_t> leftAxis;
        std::pair<int32_t, int32_t> rightAxis;
        uint32_t deviceIndex;

        virtual USHORT ConvertButtons(GamepadDataPayload const data) = 0;
        virtual SHORT ConvertAnalog(int32_t const switchAnalog) = 0;
};

#endif
