#include "Define.h"
#include "ScrCaptureWnd.h"
#include <io.h>
#include <iostream>
#include <windows.h>
#include <memory>
#include "CommFunc.h"
#include "Log.h"
#include "Util.h"

bool IsAlreadyRunning()
{
	bool bRet = FALSE;
	HANDLE hMutex = ::CreateMutex(NULL, TRUE, L"{AA3FFA7F-BDB4-010D-2638-B0A936274932}");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		bRet = TRUE;
		HWND hWnd = FindWindow(NULL, L"截屏工具");
		if (hWnd)
		{
			COPYDATASTRUCT pCopyDataStruct = { 0 };
			SendMessage(hWnd, WM_COPYDATA, WM_MSG_COPY_ACTIVE, (LPARAM)&pCopyDataStruct);
		}
	}
	if (hMutex)
	{
		::ReleaseMutex(hMutex);
	}
	return bRet;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, int nCmdShow)
{
	//只启动一次
	if (IsAlreadyRunning()) return 0;
	//设置工作路径
	SetCurrentDirectory(GetExeDir().c_str());
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return 0;

	//设置日志输出方式
	CLog::getInstance()->SetOutputType(CLog::TYPE_OUTPUTDEBUG);
	CLog::getInstance()->start();

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("ScrCaptureSkin"));
	CWndShadow::Initialize(hInstance); 
    
	CScrCaptureWnd* pFrame = CScrCaptureWnd::Instance();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T(""), WS_VISIBLE | WS_POPUP, /*WS_EX_TOOLWINDOW|WS_EX_TOPMOST*/0);
	pFrame->ShowWindow(true);
	CPaintManagerUI::MessageLoop();
	//释放颜色存储
	FreeImageRgb();
	::CoUninitialize();
	return 0;
}