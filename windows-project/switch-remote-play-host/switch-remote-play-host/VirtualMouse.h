#pragma once

enum class SupportedMouseButtons : int
{
	Left = 0,
	Right,
	Middle
};

enum class SupportedMouseMovement : int
{
	Relative = 0,
	Absolute,
};

struct VirtualMouseConfiguration
{
	int leftButtonPressFlag;
	int rightButtonPressFlag;
	int middleButtonPressFlag;
	int movementSensitivity;
};

class VirtualMouse
{
public:
	VirtualMouse(int sensitivity);

	void Press(SupportedMouseButtons button);
	void Release(SupportedMouseButtons button);
	void ScrollWheel(int delta);
	void ScrollWheel(int x, int y);
	void Move(int x, int y);
	void Move(SupportedMouseMovement moveType, int x, int y);

	void Commit();

private:
	int prevBtnFlags;
	int mouseBtnFlags;
	int mouseSensitivity;
	long mouseX, mouseY;
	long scrollX, scrollY;

	int ButtonToPressFlag(SupportedMouseButtons button);
	int ButtonToReleaseFlag(SupportedMouseButtons button);
	unsigned long BuildEventFlags();

	void ClearScrollMovement();
	void ClearButtonReleaseFlags();
	long PickScrollValue(long x, long y);
};