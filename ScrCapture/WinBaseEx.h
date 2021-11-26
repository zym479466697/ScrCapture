#ifndef WINBASEEX_H
#define WINBASEEX_H
#include <windows.h>
#include "resource.h"
#include "../DuiLib/UIlib.h"
using namespace DuiLib;

class CWinBaseEx : public DuiLib::WindowImplBase
{
public:
	
	CWinBaseEx(void){}
	virtual ~CWinBaseEx(void)
	{
		if(m_pWndShadow)
		{
			delete m_pWndShadow;
		}
	}

	CDuiString GetSkinFolder()
	{
		return _T("ScrCaptureSkin\\");
	}

	virtual LPCTSTR GetResourceID() const
	{
#ifdef _DEBUG
		return NULL;
#else
		return MAKEINTRESOURCE(IDR_ZIPRES_UNINST);
#endif

	}

	UILIB_RESOURCETYPE GetResourceType() const
	{
#ifdef _DEBUG
		return UILIB_FILE;
#else
		return UILIB_ZIPRESOURCE;
#endif
	}

	CDuiString GetZIPFileName() const
	{
#ifdef _DEBUG
		return _T("");
#else
		return _T("ScrCaptureSkin.zip");
#endif
	}


	//初始化阴影效果
	virtual void InitShadow()
	{
		//设置任务栏图标
		SetIcon(IDI_ICON);

		m_pWndShadow = new CWndShadow;
		m_pWndShadow->Create(this->GetHWND());
		RECT rcCorner = { 16,16,16,16};
		RECT rcHoleOffset = {0,0,0,0};
		m_pWndShadow->SetImage(_T("shadow.png"), rcCorner, rcHoleOffset);
	}


	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
	{
		if (uMsg == WM_KEYDOWN)
		{
			switch (wParam)
			{
			case VK_RETURN:
			case VK_ESCAPE:
				return 0;
			default:
				break;
			}
		}
		return WindowImplBase::MessageHandler(uMsg, wParam, lParam, bHandled);
	}

protected:
	CWndShadow *m_pWndShadow;
};


#endif