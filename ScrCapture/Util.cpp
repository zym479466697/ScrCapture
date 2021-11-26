#include "Util.h"


bool g_init = false;
int g_screen_x = 0;
int g_screen_y = 0;
int g_screen_buffer_size = 0;
BYTE* g_screen_data = 0;

void GetDestImageRgb(int x, int y, int& r, int& g, int& b)
{
	if (g_screen_data) {
		b = g_screen_data[4 * ((y*g_screen_x) + x)];
		g = g_screen_data[4 * ((y*g_screen_x) + x) + 1];
		r = g_screen_data[4 * ((y*g_screen_x) + x) + 2];
	}
	else {
		r = g = b = 0;
	}
}

void FreeImageRgb()
{
	if (g_screen_data) {
		free(g_screen_data);
		g_screen_data = nullptr;
	}
}

HBITMAP CreateDesktopBitmap(HWND hWnd)
{
	HWND hWndDesktop = ::GetDesktopWindow();
	RECT rect;
	GetWindowRect(hWndDesktop, &rect);
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	rect.left = rect.top = 0;

	HDC hDcDesktop = GetDC(hWndDesktop);
	HDC hDcMem = CreateCompatibleDC(hDcDesktop);
	HBITMAP hBmp = CreateCompatibleBitmap(hDcDesktop, rect.right, rect.bottom);
	HGDIOBJ hOld = SelectObject(hDcMem, hBmp);
	::BitBlt(hDcMem, 0, 0, rect.right, rect.bottom, hDcDesktop, 0, 0, SRCCOPY|CAPTUREBLT);

	if (!g_init) {
		g_init = true;
		HDC hScreen = GetDC(NULL);
		g_screen_x = GetDeviceCaps(hScreen, HORZRES);
		g_screen_y = GetDeviceCaps(hScreen, VERTRES);

		BITMAPINFOHEADER bmi = { 0 };
		bmi.biSize = sizeof(BITMAPINFOHEADER);
		bmi.biPlanes = 1;
		bmi.biBitCount = 32;
		bmi.biWidth = g_screen_x;
		bmi.biHeight = -g_screen_y;
		bmi.biCompression = BI_RGB;
		bmi.biSizeImage = 0;// 3 * ScreenX * ScreenY;
		if (g_screen_data)
			free(g_screen_data);
		g_screen_data = (BYTE*)malloc(4 * g_screen_x * g_screen_y);
		GetDIBits(hDcMem, hBmp, 0, g_screen_y, g_screen_data, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
	}

	::ReleaseDC(hWndDesktop, hDcDesktop);
	::DeleteDC(hDcMem);
	return hBmp;
}

HBITMAP CreateDesktopMaskBitmap(HWND hWnd)
{
	HDC hDCMem = CreateCompatibleDC(NULL);
	RECT rect;
	GetWindowRect(::GetDesktopWindow(), &rect);
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	rect.left = rect.top = 0;

	HBITMAP hBmp = NULL;

	HDC hDC = GetDC(hWnd);
	hBmp = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
	ReleaseDC(hWnd, hDC);

	HGDIOBJ hOld = SelectObject(hDCMem, hBmp);
	CRenderEngine::DrawColor(hDCMem, rect, 0x4F000000);
	SelectObject(hDCMem, hOld);

	DeleteObject(hDCMem);
	return hBmp;
}

HWND SmallestWindowFromCursor(RECT& rcWindow)
{	
	HWND hWnd, hTemp;
	POINT pt;
	::GetCursorPos(&pt);
	hWnd = ::ChildWindowFromPointEx(::GetDesktopWindow(), pt, CWP_SKIPDISABLED|CWP_SKIPINVISIBLE);
	if( hWnd != NULL )
	{
		hTemp = hWnd;  
		while (true) {
			::GetCursorPos(&pt);
			::ScreenToClient(hTemp, &pt);  
			hTemp = ::ChildWindowFromPointEx(hTemp, pt, CWP_SKIPINVISIBLE);  
			if (hTemp == NULL || hTemp == hWnd)  
				break;
			hWnd = hTemp;
		}  
		::GetWindowRect(hWnd, &rcWindow);
	}
	return hWnd;
}
