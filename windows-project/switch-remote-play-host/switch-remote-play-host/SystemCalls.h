#pragma once
#include <WinUser.h>
#include <vector>
#include <string>
#include "Broadcast.h"
#include "Connection.h"

extern PROCESS_INFORMATION streamProcessInfo;
extern PROCESS_INFORMATION audioProcessInfo;
extern Broadcast* switchBroadcastListener;
extern Connection* switchHandshakeConnection;
extern Connection* switchCommandListener;

MONITORINFOEX DefaultMonitorInfo();

void PrintMonitorInfo(MONITORINFOEX const& monitor);

int ChangeResolution(int width, int height);
int ChangeResolution(std::wstring device, int width, int height);
bool ResolutionChangeSuccessful(int const result);
void PrintResolutionChangeResult(int const result);

void StopStreamProcesses();

BOOL WINAPI ConsoleWindowEventHandler(DWORD eventType);

bool WinsockReady();

bool VirtualControllerDriverAvailable();

bool StartupTouchContext();

std::string GetLastErrorAsString();

std::string PointerFlagToStr(POINTER_FLAGS pointerFlags);

std::string CreateDiagnosticOutput(std::vector<POINTER_TOUCH_INFO> const& contacts);

void minimiseToTray(HWND hWnd);
