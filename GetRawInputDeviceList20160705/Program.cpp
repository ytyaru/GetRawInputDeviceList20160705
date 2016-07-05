#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <wchar.h>
#include <string>
#include <vector>
using std::wstring;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szClassName[] = _T("GetRawInputDeviceList20160705");

HWND hWnd;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR lpszCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS myProg;
	if (!hPreInst) {
		myProg.style			= CS_HREDRAW | CS_VREDRAW;
		myProg.lpfnWndProc		= WndProc;
		myProg.cbClsExtra		= 0;
		myProg.cbWndExtra		= 0;
		myProg.hInstance		= hInstance;
		myProg.hIcon			= NULL;
		myProg.hCursor			= LoadCursor(NULL, IDC_ARROW);
		myProg.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		myProg.lpszMenuName		= NULL;
		myProg.lpszClassName	= szClassName;
		if (!RegisterClass(&myProg)) { return FALSE; }
	}
	hWnd = CreateWindow(szClassName,	// class名
		_T("デバイス(HID)の一覧を表示する。VC++2010Expressのデバッグ出力に表示する。"),		// タイトル
		WS_OVERLAPPEDWINDOW,		// Style
		CW_USEDEFAULT,				// X
		CW_USEDEFAULT,				// Y
		CW_USEDEFAULT,				// Width
		CW_USEDEFAULT,				// Height
		NULL,						// 親ウィンドウまたはオーナーウィンドウのハンドル
		NULL,						// メニューハンドルまたは子ウィンドウ ID
		hInstance,					// アプリケーションインスタンスのハンドル
		NULL						// ウィンドウ作成データ
	);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (msg.wParam);
}

void GetDevices()
{
	UINT numDevices;
	GetRawInputDeviceList(NULL, &numDevices, sizeof(RAWINPUTDEVICELIST));
	if(numDevices == 0) return;

	std::vector<RAWINPUTDEVICELIST> deviceList(numDevices);
	GetRawInputDeviceList(&deviceList[0], &numDevices, sizeof(RAWINPUTDEVICELIST));
	
	std::vector<TCHAR> deviceNameDataT;
	std::vector<wchar_t> deviceNameData;
	wstring deviceName;
	TCHAR dataStr[1024];
	for(UINT i = 0; i < numDevices; ++i)
	{
		const RAWINPUTDEVICELIST& device = deviceList[i];
		
		memset(dataStr, NULL, 1024);
		if(device.dwType == RIM_TYPEMOUSE) { _stprintf_s(dataStr, 1024, _T("type=Mouse:\n")); }
		else if(device.dwType == RIM_TYPEKEYBOARD) { _stprintf_s(dataStr, 1024, _T("type=Keyboard:\n")); }
		else if(device.dwType == RIM_TYPEHID) { _stprintf_s(dataStr, 1024, _T("type=HID:\n")); }
		else { _stprintf_s(dataStr, 1024, _T("type=?:\n")); }
		OutputDebugString(dataStr);
		
		memset(dataStr, NULL, 1024);
		_stprintf_s(dataStr, 1024, _T("	Handle=0x%08X\n"), device.hDevice);
		OutputDebugString(dataStr);

		UINT dataSize;
		GetRawInputDeviceInfo(device.hDevice, RIDI_DEVICENAME, nullptr, &dataSize);
		if(dataSize)
		{
			deviceNameData.resize(dataSize);
			UINT result = GetRawInputDeviceInfo(device.hDevice, RIDI_DEVICENAME, &deviceNameData[0], &dataSize);
			if(result != UINT_MAX)
			{
				deviceName.assign(deviceNameData.begin(), deviceNameData.end());
				memset(dataStr, NULL, 1024);
				_stprintf_s(dataStr, 1024, _T("	Name=%s\n"), deviceName.c_str());
				OutputDebugString(dataStr);
			}
		}

		RID_DEVICE_INFO deviceInfo;
		deviceInfo.cbSize = sizeof deviceInfo;
		dataSize = sizeof deviceInfo;
		UINT result = GetRawInputDeviceInfo(device.hDevice, RIDI_DEVICEINFO, &deviceInfo, &dataSize);
		if(result != UINT_MAX)
		{
			memset(dataStr, NULL, 1024);
			if(device.dwType == RIM_TYPEMOUSE)
			{
				_stprintf_s(dataStr, 1024, _T("	Id=%u, Buttons=%u, SampleRate=%u, HorizontalWheel=%s\n"), 
					deviceInfo.mouse.dwId,
					deviceInfo.mouse.dwNumberOfButtons,
					deviceInfo.mouse.dwSampleRate,
					deviceInfo.mouse.fHasHorizontalWheel ? L"1" : L"0");
			}
			else if(device.dwType == RIM_TYPEKEYBOARD)
			{
				_stprintf_s(dataStr, 1024, _T("	Type=%u, SubType=%u, KeyboardMode=%u, NumberOfFunctionKeys=%u, NumberOfIndicators=%u, NumberOfKeysTotal=%u\n"), 
					deviceInfo.keyboard.dwType,
					deviceInfo.keyboard.dwSubType,
					deviceInfo.keyboard.dwKeyboardMode,
					deviceInfo.keyboard.dwNumberOfFunctionKeys,
					deviceInfo.keyboard.dwNumberOfIndicators,
					deviceInfo.keyboard.dwNumberOfKeysTotal);
			}
			else if(device.dwType == RIM_TYPEHID)
			{
				_stprintf_s(dataStr, 1024, _T("	VendorId=%u, ProductId=%u, VersionNumber=%u, UsagePage=0x%X, Usage=0x%X, NumberOfKeysTotal=%u\n"), 
					deviceInfo.hid.dwVendorId,
					deviceInfo.hid.dwProductId,
					deviceInfo.hid.dwVersionNumber,
					deviceInfo.hid.usUsagePage,
					deviceInfo.hid.usUsage);
			}
			else {}
			OutputDebugString(dataStr);
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_CREATE:
			GetDevices();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return (0L);
}
