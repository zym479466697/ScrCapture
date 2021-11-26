#pragma once
#include "Define.h"

HBITMAP CreateDesktopBitmap(HWND hWnd);
HBITMAP CreateDesktopMaskBitmap(HWND hWnd);
HWND SmallestWindowFromCursor(RECT& rcWindow);
void GetDestImageRgb(int x, int y, int& r, int& g, int& b);
void FreeImageRgb();