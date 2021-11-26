#ifndef SCRCAPTUREWND_H
#define SCRCAPTUREWND_H
#include "Define.h"
#include "WinBaseEx.h"
#include "CanvasContainer.h"
#include <memory>
#include "DrawGraph.h"

class CMagnifierUI;
class CScrCaptureWnd : public CWindowWnd, public INotifyUI, public IMessageFilterUI
{
private:
	CScrCaptureWnd();
	~CScrCaptureWnd();

public:
	static CScrCaptureWnd* Instance();

	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void OnFinalMessage(HWND /*hWnd*/);

	CDuiString GetSkinFolder()
	{
		return _T("ScrCaptureSkin\\");
	}

	virtual LPCTSTR GetResourceID() const
	{
#ifdef _DEBUG
		return NULL;
#else
		return MAKEINTRESOURCE(IDR_ZIPRES_SRCCAPTURE);
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

	void Init();
	void OnPrepare();
	void Notify(TNotifyUI& msg);

	RECT GetWindowRect() const;
	RECT GetClipPadding() const;
	void SetClipPadding(RECT rc);
	RECT GetClipRect() const;
	RECT GetCanvasContainerRect() const;
	void RefreshClipWinSize();
	//toolbar 相关
	void ShowMainBar(bool bShow = true);
	void ShowSubBar();
	void OnSelectColor(int idx);
	void OnMainBarSelectActionOp(ACTION_OP op);

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	static CScrCaptureWnd* m_pInstance;
	static LPBYTE m_lpResourceZIPBuffer;

	ULONG_PTR m_pGdiplusToken;
	CPaintManagerUI m_pm;
	HMENU m_hEditMenu;

	CControlUI* m_pDesktopImage;
	CControlUI* m_pDesktopMask;
	CControlUI* m_pDesktopMaskBorder;
	CMagnifierUI* m_pMagnifierCtrl;
	CLabelUI* m_pTxtCanvasSize;
	CCanvasContainerUI* m_pDesktopCanvasContainer;
	CHorizontalLayoutUI* m_pMainBar;
	CHorizontalLayoutUI* m_pSubBar;
	CHorizontalLayoutUI* m_pPenLayout;
	CHorizontalLayoutUI* m_pFontLayout;
	CHorizontalLayoutUI* m_pColorSplitLayout;
	CHorizontalLayoutUI* m_pColorSelectLayout;
	CHorizontalLayoutUI* m_pColorMosaicLayout;
	CLabelUI* m_pLabelSelColor;
	COptionUI* m_pOptionSmallPen;
	COptionUI* m_pOptionMidPen;
	COptionUI* m_pOptionBigPen;
	CComboUI* m_pComboxFont;
	CSliderUI* m_pSliderMosaic;
	
	RECT m_rcWindow;
	RECT m_rcClipPadding;
	POINT m_ptClipBasePoint; // 调整基准点
	bool m_bClipChoiced; // 是否已选择截图区域
	bool m_isSubOnBottom;//sub在main下边缘
	TCHAR m_sClipDrawStringBuf[MAX_PATH];
};



#endif/*SCRCAPTUREWND_H*/