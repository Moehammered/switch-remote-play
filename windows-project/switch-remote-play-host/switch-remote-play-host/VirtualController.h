#pragma once
#include "SwitchStream.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Xinput.h>
#include <ViGEm/Client.h>
#include "SwitchControlsDefinitions.h"
#include <unordered_map>

enum ABXYMap { POS, FUN, };

class IVirtualController {
public:
   virtual bool Create() = 0;
   virtual void UpdateController() = 0;
   virtual void Print() = 0;
   virtual void ConvertPayload(GamepadDataPayload) = 0;
   virtual void UpdateState() = 0;
   virtual void ResetController() = 0;
   virtual void Disconnect() = 0;

   void SetABXYMap(ABXYMap map) {
      abxyMap = map;
   }

protected:
   ABXYMap abxyMap;
   PVIGEM_CLIENT client;
   PVIGEM_TARGET pad;
};

class X360Controller : public IVirtualController {
public:
   bool Create();
   void Print();
   void ConvertPayload(GamepadDataPayload const data);
   void UpdateState();
   void UpdateController();
   void ResetController();
   void Disconnect();

private:
   static USHORT ConvertButtons(GamepadDataPayload const data, ABXYMap abxyMap);
   static SHORT ConvertAnalog(int32_t const switchAnalog);

   USHORT buttons;
   BYTE lt, rt;
   SHORT lx, ly;
   SHORT rx, ry;

   XUSB_REPORT state;
};

class DS4Controller : public IVirtualController {
public:
   bool Create();
   void Print();
   void ConvertPayload(GamepadDataPayload const data);
   void UpdateState();
   void UpdateController();
   void ResetController();
   void Disconnect();

private:
   static USHORT ConvertButtons(GamepadDataPayload const data, ABXYMap abxyMap);
   static BYTE ConvertAnalog(int32_t const switchAnalog);
   static DS4_DPAD_DIRECTIONS ConvertDpad(GamepadDataPayload const data);

   USHORT buttons;
   DS4_DPAD_DIRECTIONS directions;
   BYTE lt, rt;
   BYTE lx, ly;
   BYTE rx, ry;

   DS4_REPORT state;
};
