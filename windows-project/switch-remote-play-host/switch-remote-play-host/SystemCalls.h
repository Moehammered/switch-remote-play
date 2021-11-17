#pragma once
#include "Log.h"
#include "Broadcast.h"
#include "Connection.h"
#include <vector>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinUser.h>

extern PROCESS_INFORMATION streamProcessInfo;
extern PROCESS_INFORMATION audioProcessInfo;
extern Broadcast* switchBroadcastListener;
extern Connection* switchHandshakeConnection;
extern Connection* switchCommandListener;

MONITORINFOEX DefaultMonitorInfo();

void PrintMonitorInfo(MONITORINFOEX const& monitor, Log& logger);

int ChangeResolution(int width, int height);
int ChangeResolution(std::wstring device, int width, int height);
bool ResolutionChangeSuccessful(int const result);
tstring ResolutionChangeResultString(int const result);

void StopStreamProcesses();

BOOL WINAPI ConsoleWindowEventHandler(DWORD eventType);

bool SocketsReady();
bool SocketsReady(Log& logger);

bool VirtualControllerDriverAvailable();
bool VirtualControllerDriverAvailable(Log& logger);

bool StartupTouchContext();
bool StartupTouchContext(Log& logger);

std::string GetLastErrorAsString();

std::string PointerFlagToStr(POINTER_FLAGS pointerFlags);

std::string CreateDiagnosticOutput(std::vector<POINTER_TOUCH_INFO> const& contacts);

void minimiseToTray(HWND hWnd);
