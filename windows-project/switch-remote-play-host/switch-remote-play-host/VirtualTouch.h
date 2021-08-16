#pragma once
#include <Windows.h>
#include <WinUser.h>

class VirtualTouch
{
public:
    VirtualTouch(int deadzoneRadius, int radius);

    void Press();
    void Move(int x, int y);
    void Release();

    void Commit();

private:
    POINTER_TOUCH_INFO contact;
    int const deadzoneRadius;
    int const deadzoneMagSqr;
    int const radius;

    bool OutsideDeadzone(int x1, int y1, int x2, int y2);
};