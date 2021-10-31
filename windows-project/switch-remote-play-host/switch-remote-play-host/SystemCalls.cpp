#include "SystemCalls.h"
#include "VirtualController.h"
#include <winsock2.h> // to get the windows definitions made so 'ChangeDisplaySettings' can be defined
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

MONITORINFOEX DefaultMonitorInfo()
{
	auto monitorPoint = POINT{ 0 };
	monitorPoint.x = 0; monitorPoint.y = 0;
	auto handle = MonitorFromPoint(monitorPoint, MONITOR_DEFAULTTOPRIMARY);

	auto monitorInfo = MONITORINFOEX{};
	monitorInfo.cbSize = sizeof(monitorInfo);

	GetMonitorInfo(handle, &monitorInfo);

	return monitorInfo;
}

void PrintMonitorInfo(MONITORINFOEX const& monitor)
{
	std::cout << "\n---- Monitor ----\n";
	std::cout << "Printing found monitor info...\n";
	std::wcout << "    " << monitor.szDevice << "\n";
	if (monitor.dwFlags == MONITORINFOF_PRIMARY)
		std::cout << "        Primary monitor\n";
	else
		std::cout << "        Secondary monitor\n";

	std::cout << "        Display Area: " << monitor.rcMonitor.left << "," << monitor.rcMonitor.top << " - " << monitor.rcMonitor.right << "," << monitor.rcMonitor.bottom << "\n";

	auto const x = monitor.rcMonitor.right - monitor.rcMonitor.left;
	auto const y = monitor.rcMonitor.bottom - monitor.rcMonitor.top;
	std::cout << "        Resolution:   " << x << " x " << y << "\n\n";
}

int ChangeResolution(int width, int height)
{
	auto deviceMode = DEVMODE{ 0 };

	deviceMode.dmSize = sizeof(deviceMode);
	deviceMode.dmBitsPerPel = 32;
	deviceMode.dmPelsWidth = width;
	deviceMode.dmPelsHeight = height;
	deviceMode.dmDisplayFrequency = 60;
	deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	return ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
}

int ChangeResolution(std::wstring device, int width, int height)
{
	auto deviceMode = DEVMODE{ 0 };

	deviceMode.dmSize = sizeof(deviceMode);
	deviceMode.dmBitsPerPel = 32;
	deviceMode.dmPelsWidth = width;
	deviceMode.dmPelsHeight = height;
	deviceMode.dmDisplayFrequency = 60;
	deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	return ChangeDisplaySettingsEx(device.c_str(), &deviceMode, NULL, CDS_FULLSCREEN, 0);
}

bool ResolutionChangeSuccessful(int const result)
{
	return result == DISP_CHANGE_SUCCESSFUL;
}

void PrintResolutionChangeResult(int const result)
{
	switch (result)
	{
	case DISP_CHANGE_SUCCESSFUL:
		std::cout << "Resolution changed successfully\n";
		break;

	case DISP_CHANGE_RESTART:
		std::cout << "Resolution change failed(DISP_CHANGE_RESTART): The computer must be restarted for the graphics mode to work.\n";
		break;

	case DISP_CHANGE_FAILED:
		std::cout << "Resolution change failed(DISP_CHANGE_FAILED): The display driver failed the specified graphics mode.\n";
		break;

	case DISP_CHANGE_BADMODE:
		std::cout << "Resolution change failed(DISP_CHANGE_BADMODE): The graphics mode is not supported.\n";
		break;

	case DISP_CHANGE_NOTUPDATED:
		std::cout << "Resolution change failed(DISP_CHANGE_NOTUPDATED): Unable to write settings to the registry.\n";
		break;

	case DISP_CHANGE_BADFLAGS:
		std::cout << "Resolution change failed(DISP_CHANGE_BADFLAGS): An invalid set of flags was passed in.\n";
		break;

	case DISP_CHANGE_BADPARAM:
		std::cout << "Resolution change failed(DISP_CHANGE_BADPARAM): An invalid parameter was passed in. This can include an invalid flag or combination of flags.\n";
		break;

	case DISP_CHANGE_BADDUALVIEW:
		std::cout << "Resolution change failed(DISP_CHANGE_BADDUALVIEW): The settings change was unsuccessful because the system is DualView capable.\n";
		break;
	}
}

void StopStreamProcesses()
{
	if (streamProcessInfo.hProcess != nullptr)
	{
		TerminateProcess(streamProcessInfo.hProcess, 1);
		CloseHandle(streamProcessInfo.hProcess);
		CloseHandle(streamProcessInfo.hThread);
		ZeroMemory(&streamProcessInfo, sizeof(streamProcessInfo));
	}
	if (audioProcessInfo.hProcess != nullptr)
	{
		TerminateProcess(audioProcessInfo.hProcess, 1);
		CloseHandle(audioProcessInfo.hProcess);
		CloseHandle(audioProcessInfo.hThread);
		ZeroMemory(&audioProcessInfo, sizeof(audioProcessInfo));
	}
}

BOOL WINAPI ConsoleWindowEventHandler(DWORD eventType)
{
	switch (eventType)
	{
		// Handle the CTRL-C signal.
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		std::cout << "Cleaning up in event handler...\n";
		std::cout << "Terminating video and audio process\n";
		StopStreamProcesses();
		if (switchHandshakeConnection != nullptr)
		{
			std::cout << "terminating handshake connection\n";
			switchHandshakeConnection->Shutdown();
			switchHandshakeConnection->Close();
		}
		if (switchBroadcastListener != nullptr)
		{
			std::cout << "terminating broadcast listener ";
			switchBroadcastListener->Close();
			switchBroadcastListener->Shutdown();
		}
		if (switchCommandListener != nullptr)
		{
			std::cout << "terminating command receiver connection\n";
			switchCommandListener->Close();
			switchCommandListener->Shutdown();
		}
		Sleep(2000);
		return TRUE;

	default:
		return FALSE;
	}
}

bool WinsockReady()
{
	WSADATA wsaStateData;
	auto socketStartup = WSAStartup(MAKEWORD(2, 2), &wsaStateData);

	switch (socketStartup)
	{
	case WSASYSNOTREADY:
		std::cout << "Winsock Error - WSASYSNOTREADY: The underlying network subsystem is not ready for network communication\n";
		return false;

	case WSAVERNOTSUPPORTED:
		std::cout << "Winsock Error - WSAVERNOTSUPPORTED: The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation\n";
		return false;

	case WSAEINPROGRESS:
		std::cout << "Winsock Error - WSAEINPROGRESS: A blocking Windows Sockets 1.1 operation is in progress\n";
		return false;

	case WSAEPROCLIM:
		std::cout << "Winsock Error - WSAEPROCLIM: A limit on the number of tasks supported by the Windows Sockets implementation has been reached\n";
		return false;

	case WSAEFAULT:
		std::cout << "Winsock Error - WSAEFAULT: The lpWSAData parameter is not a valid pointer\n";
		return false;
	}

	return true;
}

bool VirtualControllerDriverAvailable()
{
	using namespace std;
	cout << "\n---- Virtual Controller ----\n";
	cout << "Testing for correct Virtual Controller driver installation...\n\n";
	auto client = vigem_alloc();

	if (client == nullptr)
	{
		cout << "    Failed to connect to driver" << endl;
		return false;
	}
	cout << "    Client allocation successful.\n";
	const auto result = vigem_connect(client);
	if (!VIGEM_SUCCESS(result))
	{
		vigem_free(client);
		cout << "    Virtual Controller connection failed with error code: 0x" << std::hex << result << endl;
		return false;
	}

	auto pad = vigem_target_ds4_alloc();

	const auto pluginEvent = vigem_target_add(client, pad);

	if (!VIGEM_SUCCESS(pluginEvent))
	{
		cout << "    Virtual Controller plugin failed with error code: 0x" << std::hex << pluginEvent << endl;
		vigem_disconnect(client);
		vigem_free(client);
		vigem_target_free(pad);

		return false;
	}

	cout << "    Successfully created, connected, and plugged in a virtual PS4 controller. Cleaning up now...\n";
	DS4_REPORT state{};
	ZeroMemory(&state, sizeof(DS4_REPORT));
	DS4_REPORT_INIT(&state);

	this_thread::sleep_for(chrono::duration<int, milli>(1000));
	auto unplugEvent = vigem_target_remove(client, pad);
	if (!VIGEM_SUCCESS(unplugEvent))
	{
		vigem_target_free(pad);
		cout << "    Virtual Controller unplug failed with error code: 0x" << std::hex << unplugEvent << endl;
		vigem_disconnect(client);
		vigem_free(client);

		return false;
	}

	vigem_disconnect(client);
	vigem_free(client);
	return true;
}

bool StartupTouchContext()
{
	auto result = InitializeTouchInjection(5, TOUCH_FEEDBACK_INDIRECT);
	if (result == FALSE)
	{
		auto err = GetLastError();
		std::cout << "Failed to initialise touch injection context with error code: " << err << "\n\n";
	}

	return result == TRUE;
}

std::string GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}

std::string PointerFlagToStr(POINTER_FLAGS pointerFlags)
{
	auto ss = std::stringstream{};

	auto map = std::unordered_map<POINTER_FLAGS, std::string>{
		{ POINTER_FLAG_UPDATE, "pf_UPDATE" },
		{ POINTER_FLAG_DOWN, "pf_DOWN" },
		{ POINTER_FLAG_INRANGE, "pf_INRANGE" },
		{ POINTER_FLAG_INCONTACT, "pf_INCONTACT" },
		{ POINTER_FLAG_UP, "pf_UP" }
	};

	for (auto const& pair : map)
	{
		if (pair.first & pointerFlags)
			ss << pair.second << " ";
	}

	return ss.str();
}

std::string CreateDiagnosticOutput(std::vector<POINTER_TOUCH_INFO> const& contacts)
{
	auto ss = std::stringstream{};

	for (auto const& c : contacts)
	{
		ss << c.pointerInfo.pointerId << ":  ";
		ss << "[ " << PointerFlagToStr(c.pointerInfo.pointerFlags) << "] ";
		ss << "\n";
	}

	return ss.str();
}

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include <CommCtrl.h>
#include <shellapi.h>
#include <strsafe.h>


LRESULT CALLBACK TheProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	auto& msg = (MSG&)lParam;

	switch (msg.message)
	{
	case HCBT_SYSCOMMAND:
		if (wParam == SC_MINIMIZE)
		{
			minimiseToTray(GetConsoleWindow());
		}
		break;
	}

	return CallNextHookEx(0, nCode, wParam, lParam);
}

void minimiseToTray(HWND hWnd)
{
	NOTIFYICONDATA nid = {};
	nid.cbSize = sizeof(nid);
	nid.hWnd = hWnd;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_GUID;

	// Note: This is an example GUID only and should not be used.
	// Normally, you should use a GUID-generating tool to provide the value to
	// assign to guidItem.
	static const GUID myGUID =
	{ 0x23977b55, 0x10e0, 0x4041, {0xb8, 0x62, 0xb1, 0x95, 0x41, 0x96, 0x36, 0x69} };
	nid.guidItem = myGUID;

	// This text will be shown as the icon's tooltip.
	StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Test application");

	// Load the icon for high DPI.
	auto hInst2 = GetWindowLong(hWnd, GWLP_HINSTANCE);
	auto hInst = GetModuleHandle(NULL);
	LoadIconMetric(hInst, MAKEINTRESOURCE(101), LIM_SMALL, &(nid.hIcon));

	SetWindowsHook(WH_CBT, TheProc);

	// Show the notification.
	auto result = Shell_NotifyIcon(NIM_ADD, &nid) ? S_OK : E_FAIL;
	ShowWindow(hWnd, SW_HIDE);

	if (result == S_OK)
	{
		std::cout << "Created Tray Icon\n";
		auto constexpr second = std::chrono::duration<int, std::milli>(1000);
		std::this_thread::sleep_for(second * 5);
		ShowWindow(hWnd, SW_SHOW);
		Shell_NotifyIcon(NIM_DELETE, &nid);
		auto r2 = DestroyIcon(nid.hIcon);
		if (r2 == TRUE)
			std::cout << "Destroyed Icon\n";
	}
}
