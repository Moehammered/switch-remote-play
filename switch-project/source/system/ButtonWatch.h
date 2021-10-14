#ifndef __BUTTONWATCH_H__
#define __BUTTONWATCH_H__

#include <functional>
#include <string>

uint64_t MonitorKeys(std::function<void(std::string)> renderFunction);

#endif