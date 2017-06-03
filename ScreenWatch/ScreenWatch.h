#pragma once

#include "resource.h"
#include <winuser.h>
#include <Windows.h>
#include <windowsx.h>
#include <atlimage.h> 
#include <Gdiplusimaging.h> 
UINT TimerID = 0;
void ScreenWatch(HWND hWnd);
HWND smallcap, hwnd_sec;
int sec = 2;