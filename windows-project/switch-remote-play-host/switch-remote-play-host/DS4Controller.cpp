#include "VirtualController.h"
#include <iostream>
#include <sstream>
#include <string>

const Switch_HidControllerKeys ButtonsToCheck[] = {
   KEY_A, KEY_B, KEY_Y, KEY_X,
   KEY_L, KEY_R, KEY_MINUS, KEY_PLUS,
   KEY_LSTICK, KEY_RSTICK
};

std::unordered_map<Switch_HidControllerKeys, USHORT, std::hash<uint32_t>> SWITCH_TO_DS4_BTNS_POS{
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

std::unordered_map<Switch_HidControllerKeys, USHORT, std::hash<uint32_t>> SWITCH_TO_DS4_BTNS_FUN{
   { KEY_A, DS4_BUTTON_CROSS },
   { KEY_B, DS4_BUTTON_CIRCLE },
   { KEY_X, DS4_BUTTON_SQUARE },
   { KEY_Y, DS4_BUTTON_TRIANGLE },
   { KEY_L, DS4_BUTTON_SHOULDER_LEFT },
   { KEY_R, DS4_BUTTON_SHOULDER_RIGHT },
   { KEY_MINUS, DS4_BUTTON_SHARE },
   { KEY_PLUS , DS4_BUTTON_OPTIONS },
   { KEY_LSTICK , DS4_BUTTON_THUMB_LEFT },
   { KEY_RSTICK , DS4_BUTTON_THUMB_RIGHT }
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
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_EAST, "L" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_WEST, "R" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_NORTH, "U" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_SOUTH, "D" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_NORTHEAST, "UL" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_NORTHWEST, "UR" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_SOUTHEAST, "DL" },
   { DS4_DPAD_DIRECTIONS::DS4_BUTTON_DPAD_SOUTHWEST, "DR" }
};

USHORT DS4Controller::ConvertButtons(GamepadDataPayload const data, ABXYMap abxyMap)
{
   auto& switcher = abxyMap == ABXYMap::FUN ?
      SWITCH_TO_DS4_BTNS_FUN :
      SWITCH_TO_DS4_BTNS_POS;

   USHORT btns = {};
   //handle pressed buttons
   for (auto btn : ButtonsToCheck)
   {
      if ((btn & data.keys) != 0x0)
         btns |= switcher[btn];
   }

   return btns;
}

BYTE DS4Controller::ConvertAnalog(int32_t const switchAnalog)
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

DS4_DPAD_DIRECTIONS DS4Controller::ConvertDpad(GamepadDataPayload const data)
{
   USHORT dpadInput = 0;
   //handle dpad
   if (data.keys & KEY_DDOWN)
   {
      if (data.keys & KEY_DLEFT)
         dpadInput = DS4_BUTTON_DPAD_SOUTHWEST;
      else if (data.keys & KEY_DRIGHT)
         dpadInput = DS4_BUTTON_DPAD_SOUTHEAST;
      else
         dpadInput = DS4_BUTTON_DPAD_SOUTH;
   }
   else if (data.keys & KEY_DUP)
   {
      if (data.keys & KEY_DLEFT)
         dpadInput = DS4_BUTTON_DPAD_NORTHWEST;
      else if (data.keys & KEY_DRIGHT)
         dpadInput = DS4_BUTTON_DPAD_NORTHEAST;
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

   return true;
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
   for (auto b : DS4_BTN_STRINGS)
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

void DS4Controller::ConvertPayload(GamepadDataPayload const data)
{
   directions = ConvertDpad(data);
   buttons = ConvertButtons(data, abxyMap);
   lx = ConvertAnalog(data.ljx);
   ly = ConvertAnalog(-data.ljy);
   rx = ConvertAnalog(data.rjx);
   ry = ConvertAnalog(-data.rjy);

   lt = 0; rt = 0;
   if (data.keys & KEY_ZL)
      lt = 255;
   if (data.keys & KEY_ZR)
      rt = 255;
}

void DS4Controller::UpdateState()
{
   DS4_SET_DPAD(&state, directions);
   state.wButtons = buttons;
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