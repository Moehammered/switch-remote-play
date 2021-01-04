#ifndef __SWITCHBUTTONS_H__
#define __SWITCHBUTTONS_H__

#include <string>

std::string SwitchButtonToString(uint32_t btn);
uint32_t ParseSwitchButtonString(std::string s);

#endif
