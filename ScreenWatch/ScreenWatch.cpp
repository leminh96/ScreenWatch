#include "stdafx.h"
#include "ScreenWatch.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SCREENWATCH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCREENWATCH));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCREENWATCH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_BTNFACE + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SCREENWATCH);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   250, 100, 630, 500, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HWND hWnd;
HHOOK hMyHook;
HINSTANCE hInstance;
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
INT InitKeyHook(HWND hwndYourWindow)
{
	hWnd = hwndYourWindow;
	hMyHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, hInstance, 0);
	if (hMyHook != NULL)
	{
		return 1;
	}
	return 0;
}

bool flag = false;
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	int buff_size = GetWindowTextLength(hwnd_sec);
	WCHAR * buff;
	buff = new WCHAR[5];
	GetWindowText(hwnd_sec, buff, buff_size + 1);
	if (wcscmp(buff, L"") == 0)
		sec = 2;
	else
		sec = _wtoi(buff);
	if (sec < 0)
	{
		MessageBox(NULL, L"Please insert 0 sec timer or bigger", L"Error", MB_ICONERROR | MB_OK | MB_DEFBUTTON2);
	}
	else if (nCode < 0)
		return CallNextHookEx(hMyHook, nCode, wParam, lParam);
	else
	{
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState('S') & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (flag == false))
		{
			flag = true;
			TimerID = SetTimer(hWnd, ID_SECONDS, sec * 1000, NULL);
			MessageBox(NULL, L"Activated", L"Screen Watch", NULL);
		}
		else if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState('S') & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000) && (flag == true))
		{
			flag = false;
			MessageBox(NULL, L"Deactivated", L"Screen Watch", NULL);
			KillTimer(hWnd, TimerID);
		}	
	}
	return CallNextHookEx(hMyHook, nCode, wParam, lParam);
}

void ScreenWatch(HWND hWnd)
{
	if (!IsClipboardFormatAvailable(CF_DIB))
	{
		return;
	}
	if (!OpenClipboard(hWnd))
	{
		return;
	}
	int left_screen = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int right_screen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int top_screen = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int bottom_screen = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	int width = right_screen - left_screen;
	int height = bottom_screen - top_screen;

	HDC myScreen = GetDC(NULL);
	HDC memDC = CreateCompatibleDC(myScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(myScreen, width, height);
	HGDIOBJ myobj = SelectObject(memDC, hBitmap);
	BitBlt(memDC, 0, 0, width, height, myScreen, left_screen, top_screen, SRCCOPY);
	BITMAP bitmap;
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);

	HDC hdc = GetDC(smallcap);
	SetStretchBltMode(hdc, HALFTONE);
	StretchBlt(hdc, 0, 0, 500, 300, memDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

	TCHAR szName[MAX_PATH] = { 0 };
	SYSTEMTIME st = { 0 };
	::GetLocalTime(&st);
	wsprintf(szName, _T("Screenshot//Screen %d-%d-%d %d-%d-%d.bmp"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	CImage image;
	image.Attach(hBitmap);
	image.Save(szName, Gdiplus::ImageFormatBMP);

	SelectObject(memDC, myobj);
	DeleteDC(memDC);
	ReleaseDC(NULL, myScreen);
	ReleaseDC(hWnd, hdc);
	DeleteObject(hBitmap);
	CloseClipboard();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		HFONT hFont = CreateFont(-25, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
		HFONT kFont = CreateFont(-15, 0, 0, 0, FW_LIGHT, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
		HFONT mFont = CreateFont(-15, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
		HFONT nFont = CreateFont(-13, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
		smallcap = CreateWindowEx(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_BITMAP, 50, 50, 200, 200, hWnd, NULL, hInst, NULL);
		HWND hwnd = CreateWindowEx(0, L"STATIC", L"SCREEN WATCH", WS_CHILD | WS_VISIBLE | SS_LEFT, 200, 0, 300, 100, hWnd, NULL, hInst, NULL);
		SendMessage(hwnd, WM_SETFONT, WPARAM(hFont), TRUE);
		hwnd = CreateWindowEx(0, L"STATIC", L"Your current screenshot", WS_CHILD | WS_VISIBLE | SS_LEFT, 220, 360, 300, 100, hWnd, NULL, hInst, NULL);
		SendMessage(hwnd, WM_SETFONT, WPARAM(kFont), TRUE);
		hwnd = CreateWindowEx(0, L"STATIC", L"Set timer: ", WS_CHILD | WS_VISIBLE | SS_LEFT, 50, 400, 100, 30, hWnd, NULL, hInst, NULL);
		SendMessage(hwnd, WM_SETFONT, WPARAM(mFont), TRUE);
		hwnd_sec = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 150, 390, 120, 30, hWnd, NULL, hInst, NULL);
		SendMessage(hwnd_sec, WM_SETFONT, WPARAM(mFont), TRUE);
		hwnd = CreateWindowEx(0, L"STATIC", L"Default value of timer will be 2 seconds", WS_CHILD | WS_VISIBLE | SS_LEFT, 290, 410, 300, 30, hWnd, NULL, hInst, NULL);
		SendMessage(hwnd, WM_SETFONT, WPARAM(nFont), TRUE);
		hwnd = CreateWindowEx(0, L"STATIC", L"Press Ctrl + Alt + Shift + S to activate", WS_CHILD | WS_VISIBLE | SS_LEFT, 300, 50, 300, 30, hWnd, NULL, hInst, NULL);
		SendMessage(hwnd, WM_SETFONT, WPARAM(nFont), TRUE);
		InitKeyHook(hWnd);
	}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_TIMER:
		keybd_event(VK_SNAPSHOT, 0, 0, 0);
		Sleep(50);
		keybd_event(VK_SNAPSHOT, 0, KEYEVENTF_KEYUP, 0);
		ScreenWatch(hWnd);
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
