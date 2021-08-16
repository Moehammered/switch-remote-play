#include "VirtualTouch.h"

long constexpr magSqr(int x, int y) { return x * x + y * y; }

VirtualTouch::VirtualTouch(int deadzone, int radius)
    : contact{ 0 }, deadzoneRadius{ deadzone }, 
    deadzoneMagSqr{ magSqr(deadzone, deadzone) }, radius{ radius }
{
    contact.touchFlags = TOUCH_FLAG_NONE;
    contact.touchMask = TOUCH_MASK_CONTACTAREA;

    auto& pointer = contact.pointerInfo;
    pointer.pointerType = PT_TOUCH;
    pointer.pointerId = 3;
}

void VirtualTouch::Press()
{
    auto& pointer = contact.pointerInfo;
    auto contactFlags = (POINTER_FLAG_DOWN | POINTER_FLAG_UPDATE);
    if(!(pointer.pointerFlags & contactFlags))
        pointer.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
    else if(pointer.pointerFlags & POINTER_FLAG_DOWN)
        pointer.pointerFlags = POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
}

void VirtualTouch::Move(int x, int y)
{
    auto& pointer = contact.pointerInfo;
    auto x1 = pointer.ptPixelLocation.x;
    auto y1 = pointer.ptPixelLocation.y;

    if (OutsideDeadzone(x1, y1, x, y))
    {
        pointer.ptPixelLocation.x = x;
        pointer.ptPixelLocation.y = y;

        contact.rcContact.top = y - radius;
        contact.rcContact.bottom = y + radius;
        contact.rcContact.left = x - radius;
        contact.rcContact.right = x + radius;
    }
}

void VirtualTouch::Release()
{
    auto& pointer = contact.pointerInfo;
    auto ignoreFlags = (POINTER_FLAG_NONE | POINTER_FLAG_UP);
    auto flag = pointer.pointerFlags;
    if (flag != POINTER_FLAG_NONE)
        pointer.pointerFlags = POINTER_FLAG_UP;
    else if (flag & POINTER_FLAG_UP)
        pointer.pointerFlags = POINTER_FLAG_NONE;
}

void VirtualTouch::Commit()
{
    InjectTouchInput(1, &contact);
}

bool VirtualTouch::OutsideDeadzone(int x1, int y1, int x2, int y2)
{
    auto dx = x1 - x2;
    auto dy = y1 - y2;
    auto magnitude = magSqr(dx, dy);

    return magnitude > deadzoneMagSqr;
}