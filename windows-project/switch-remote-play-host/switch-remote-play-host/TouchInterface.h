#ifndef __TOUCHINTERFACE_H__
#define __TOUCHINTERFACE_H__

#include "VirtualFinger.h"
#include <vector>

class TouchInterface
{
public:
    virtual void Update(std::vector<VirtualFinger> const fingers) = 0;
    virtual void Release() = 0;

    virtual void Commit() = 0;
};

#endif
