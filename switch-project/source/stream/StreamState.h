#ifndef __STREAMSTATE_H__
#define __STREAMSTATE_H__

#include <stdint.h>

enum StreamState : int32_t
{
    INACTIVE,
    REQUESTED,
    ACTIVE,
    QUIT
};

#endif
