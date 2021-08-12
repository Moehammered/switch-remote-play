#include "VirtualMouse.h"
#include <windows.h>

int ClearFlag(int original, int flag)
{
	return original & ~flag;
}

VirtualMouse::VirtualMouse(int sensitivity)
	: prevBtnFlags{ 0 }, mouseBtnFlags{0}, 
	mouseSensitivity{sensitivity},
	mouseX{ 0 }, mouseY{ 0 }, 
	scrollX{ 0 }, scrollY{ 0 }
{
}

void VirtualMouse::Press(SupportedMouseButtons button)
{
	auto buttonFlag = ButtonToPressFlag(button);
	auto const notPressed = (mouseBtnFlags & buttonFlag) != buttonFlag;
	if (notPressed)
	{
		mouseBtnFlags &= ~ButtonToReleaseFlag(button);
		mouseBtnFlags |= buttonFlag;
	}
}

void VirtualMouse::Release(SupportedMouseButtons button)
{
	auto pressFlag = ButtonToPressFlag(button);
	auto const pressed = (mouseBtnFlags & pressFlag) == pressFlag;
	if (pressed)
	{
		mouseBtnFlags &= ~pressFlag;
		mouseBtnFlags |= ButtonToReleaseFlag(button);
	}
}

void VirtualMouse::ScrollWheel(int delta)
{
	ScrollWheel(0, delta);
}

void VirtualMouse::ScrollWheel(int x, int y)
{
	scrollX = x;
	scrollY = y;
}

void VirtualMouse::Move(int x, int y)
{
	Move(SupportedMouseMovement::Relative, x, y);
}

void VirtualMouse::Move(SupportedMouseMovement moveType, int x, int y)
{
	mouseX = x;
	mouseY = y;
}

void VirtualMouse::Commit()
{
	INPUT inputPayload{ 0 };

	inputPayload.type = INPUT_MOUSE;

	inputPayload.mi.dwFlags = BuildEventFlags();
	inputPayload.mi.dx = mouseX;
	inputPayload.mi.dy = mouseY;

	inputPayload.mi.mouseData = PickScrollValue(scrollX, scrollY);

	SendInput(1, &inputPayload, sizeof(INPUT));

	ClearButtonReleaseFlags();
	ClearScrollMovement();
	prevBtnFlags = mouseBtnFlags;
}

int VirtualMouse::ButtonToPressFlag(SupportedMouseButtons button)
{
	switch (button)
	{
	case SupportedMouseButtons::Left:
		return MOUSEEVENTF_LEFTDOWN;

	case SupportedMouseButtons::Right:
		return MOUSEEVENTF_RIGHTDOWN;

	case SupportedMouseButtons::Middle:
		return MOUSEEVENTF_MIDDLEDOWN;

	default: 
		return 0;
	}
}

int VirtualMouse::ButtonToReleaseFlag(SupportedMouseButtons button)
{
	switch (button)
	{
	case SupportedMouseButtons::Left:
		return MOUSEEVENTF_LEFTUP;

	case SupportedMouseButtons::Right:
		return MOUSEEVENTF_RIGHTUP;

	case SupportedMouseButtons::Middle:
		return MOUSEEVENTF_MIDDLEUP;

	default:
		return 0;
	}
}

unsigned long VirtualMouse::BuildEventFlags()
{
	auto const moved = (mouseX || mouseY);
	auto const btnsChanged = prevBtnFlags != mouseBtnFlags;
	auto const scrolled = (scrollX || scrollY);

	unsigned long eventFlags = moved ? MOUSEEVENTF_MOVE : 0;
	eventFlags |= btnsChanged ? mouseBtnFlags : 0;

	if (scrolled)
	{
		auto const scrollDir = abs(scrollX) > abs(scrollY) ? MOUSEEVENTF_HWHEEL : MOUSEEVENTF_WHEEL;
		eventFlags |= scrollDir;
	}

	return eventFlags;
}

void VirtualMouse::ClearScrollMovement()
{
	scrollX = 0;
	scrollY = 0;
}

void VirtualMouse::ClearButtonReleaseFlags()
{
	auto copy = ClearFlag(mouseBtnFlags, ButtonToReleaseFlag(SupportedMouseButtons::Left));
	copy = ClearFlag(copy, ButtonToReleaseFlag(SupportedMouseButtons::Right));
	copy = ClearFlag(copy, ButtonToReleaseFlag(SupportedMouseButtons::Middle));

	mouseBtnFlags = copy;
}

long VirtualMouse::PickScrollValue(long x, long y)
{
	auto const a = abs(x);
	auto const b = abs(y);

	return a > b ? x : y;
}
