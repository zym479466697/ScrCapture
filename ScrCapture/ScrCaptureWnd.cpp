#include <windows.h>
#include <io.h>
#include <direct.h>
#include <shellapi.h>
#include <shlobj.h>
#include <time.h>
#include <algorithm>
#include "resource.h"
#include "CommFunc.h"
#include "ScrCaptureWnd.h"
#include "Util.h"
#include "CanvasContainer.h"
#include "MagnifierUI.h"
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define SPE_MAINBAR_CANVAS			2		//mainbar 和 画布间隔 
#define SPE_MAINBAR_SUNBAR			2		//mainbar 和 subbar间隔 
#define MAINBAR_WIDTH						360	//mainbar宽度
#define MAINBAR_HEIGHT						30		//mainbar高度
#define SUBBAR_HEIGHT						40		//subbar高度
#define SUBBAR_PEN_WIDTH					100
#define SUBBAR_PEN_SEP_WIDTH			5
#define SUBBAR_TXT_WIDTH					90
#define SUBBAR_MOSAIC_WIDTH			170
#define SUBBAR_COLOR_WIDTH			170
#define SUBBAR_SMALL_PEN				2
#define SUBBAR_MID_PEN					4
#define SUBBAR_BIG_PEN						8

CScrCaptureWnd* CScrCaptureWnd::m_pInstance = NULL;

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		if (_tcscmp(pstrClass, DUI_CTR_CANVASCONTAINER) == 0) {
			return new CCanvasContainerUI;
		}
		else if (_tcscmp(pstrClass, DUI_CTR_MAGNIFIER) == 0) {
			return new CMagnifierUI;
		}
		return NULL;
	}
};

LPBYTE CScrCaptureWnd::m_lpResourceZIPBuffer = NULL;
CScrCaptureWnd::CScrCaptureWnd() : m_hEditMenu(NULL), m_bClipChoiced(false)
{
	//gdi+
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_pGdiplusToken, &gdiplusStartupInput, NULL);

	m_ptClipBasePoint.x = m_ptClipBasePoint.y = 0;
	m_isSubOnBottom = true;
	::ZeroMemory(&m_rcWindow, sizeof(m_rcWindow));
	::ZeroMemory(&m_rcClipPadding, sizeof(m_rcClipPadding));
	::ZeroMemory(m_sClipDrawStringBuf, sizeof(m_sClipDrawStringBuf));
};

CScrCaptureWnd::~CScrCaptureWnd()
{
	//gdi+
	GdiplusShutdown(m_pGdiplusToken);
	::DestroyMenu(m_hEditMenu);
}

CScrCaptureWnd* CScrCaptureWnd::Instance()
{
	if (m_pInstance == NULL) m_pInstance = new CScrCaptureWnd;
	return m_pInstance;
}

LPCTSTR CScrCaptureWnd::GetWindowClassName() const
{
	return _T("UIScrCaptureFrame");
}

UINT CScrCaptureWnd::GetClassStyle() const
{
	return UI_CLASSSTYLE_DIALOG | CS_DBLCLKS;
}

void CScrCaptureWnd::OnFinalMessage(HWND /*hWnd*/)
{
	delete this;
};

void CScrCaptureWnd::Init()
{
	m_hEditMenu = ::CreatePopupMenu();

	m_pDesktopImage = m_pm.FindControl(_T("desktopimage"));
	m_pDesktopMask = m_pm.FindControl(_T("desktopmask"));
	m_pDesktopMaskBorder = m_pm.FindControl(_T("desktopmaskborder"));
	m_pMagnifierCtrl = static_cast<CMagnifierUI*>(m_pm.FindControl(_T("magnifier")));
	m_pTxtCanvasSize = static_cast<CLabelUI*>(m_pm.FindControl(_T("txt_canvas_size")));
	m_pDesktopCanvasContainer = static_cast<CCanvasContainerUI*>(m_pm.FindControl(_T("desktopcanvascontainer")));

	if (m_pDesktopImage == NULL || m_pDesktopMask == NULL ||
		m_pDesktopMaskBorder == NULL || m_pDesktopCanvasContainer == NULL
		|| m_pMagnifierCtrl == NULL
		|| m_pTxtCanvasSize == NULL
		) {
		::PostQuitMessage(0L);
	}

	::GetWindowRect(::GetDesktopWindow(), &m_rcWindow);
	::SetWindowPos(GetHWND(), NULL, m_rcWindow.left, m_rcWindow.top, m_rcWindow.right - m_rcWindow.left, m_rcWindow.bottom - m_rcWindow.top, SWP_NOZORDER);

	HBITMAP hDesktopBmp = CreateDesktopBitmap(GetHWND());
	m_pm.AddImage(_T("desktopimage"), hDesktopBmp, m_rcWindow.right - m_rcWindow.left, m_rcWindow.bottom - m_rcWindow.top, false);
	HBITMAP hDesktopBmpMask = CreateDesktopMaskBitmap(GetHWND());
	m_pm.AddImage(_T("desktopimagemask"), hDesktopBmpMask, m_rcWindow.right - m_rcWindow.left, m_rcWindow.bottom - m_rcWindow.top, true);
	HBITMAP hMagnifierBmp = CreateDesktopBitmap(GetHWND());
	m_pm.AddImage(_T("magnifierimage"), hMagnifierBmp, m_rcWindow.right - m_rcWindow.left, m_rcWindow.bottom - m_rcWindow.top, false);

	m_pDesktopImage->SetBkImage(_T("desktopimage"));
	m_pDesktopMask->SetBkImage(_T("desktopimagemask"));
	m_pMagnifierCtrl->SetBkImage(_T("magnifierimage"));

	m_pMainBar = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("main_bar")));
	m_pSubBar = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("sub_bar")));
	m_pPenLayout = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("pen_layout")));
	m_pFontLayout = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("font_layout")));
	m_pColorSplitLayout = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("color_split_layout")));
	m_pColorSelectLayout = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("color_select_layout")));
	m_pColorMosaicLayout = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("mosaic_layout")));
	m_pLabelSelColor = static_cast<CLabelUI*>(m_pm.FindControl(_T("color_select")));
	m_pOptionSmallPen = static_cast<COptionUI*>(m_pm.FindControl(_T("op_small_pen")));
	m_pOptionMidPen = static_cast<COptionUI*>(m_pm.FindControl(_T("op_mid_pen")));
	m_pOptionBigPen = static_cast<COptionUI*>(m_pm.FindControl(_T("op_big_pen")));
	m_pComboxFont = static_cast<CComboUI*>(m_pm.FindControl(_T("font_comb")));
	m_pSliderMosaic = static_cast<CSliderUI*>(m_pm.FindControl(_T("mosaic_slider")));
	m_pInputWordEdit = static_cast<CRichEditUI*>(m_pm.FindControl(_T("edit_word")));

	m_pInputWordEdit->SetVisible(FALSE);
	ShowMainBar(false);
}

void CScrCaptureWnd::OnPrepare()
{
}

void CScrCaptureWnd::Notify(TNotifyUI& msg)
{
	std::wstring strName = msg.pSender->GetName().GetData();
	if (msg.sType == _T("windowinit")) {
		OnPrepare();
	}
	else if (msg.sType == DUI_MSGTYPE_VALUECHANGED) {
		if (strName == L"mosaic_slider") {
			m_pDesktopCanvasContainer->SetMosaicValue(m_pSliderMosaic->GetValue());
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMSELECT) {
		if (strName == L"font_comb") {
			int m_iCurSel = msg.wParam;
			int iOldSel = msg.lParam;
			int font = _wtoi(m_pComboxFont->GetText().GetData());
			m_pDesktopCanvasContainer->SetFontSize(font);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED) {
		if (strName == L"op_rectangle") {
			OnMainBarSelectActionOp(ACTION_OP_RECT);
		}
		else if (strName == L"op_ellipse") {
			OnMainBarSelectActionOp(ACTION_OP_ELLIPSE);
		}
		else if (strName == L"op_line") {
			OnMainBarSelectActionOp(ACTION_OP_LINE);
		}
		else if (strName == L"op_arrow") {
			OnMainBarSelectActionOp(ACTION_OP_ARROW);
		}
		else if (strName == L"op_scrawl") {
			OnMainBarSelectActionOp(ACTION_OP_SCRAWL);
		}
		else if (strName == L"op_highlight") {
			OnMainBarSelectActionOp(ACTION_OP_HIGHLIGHT);
		}
		else if (strName == L"op_mosaic") {
			OnMainBarSelectActionOp(ACTION_OP_MOSAIC);
		}
		else if (strName == L"op_text") {
			OnMainBarSelectActionOp(ACTION_OP_TXT);
		}
		else if (strName == L"op_small_pen") {
			m_pDesktopCanvasContainer->SetPenWidth(SUBBAR_SMALL_PEN);
		}
		else if (strName == L"op_mid_pen") {
			m_pDesktopCanvasContainer->SetPenWidth(SUBBAR_MID_PEN);
		}
		else if (strName == L"op_big_pen") {
			m_pDesktopCanvasContainer->SetPenWidth(SUBBAR_BIG_PEN);
		}
		else if (startWith(strName, L"color_")) {
			std::vector<std::wstring> strList;
			StringSplit(strList, strName, L"_");
			if (strList.size() == 2) {
				int idx = _wtoi(strList[1].c_str());
				OnSelectColor(idx);
			}
		}
	}
	else if (msg.sType == DUI_MSGTYPE_CLICK) {
		if (strName == L"btn_undo") {
			m_pDesktopCanvasContainer->Undo();
		}
		else if (strName == L"btn_save") {
			CDuiString strDefaultName;
			SYSTEMTIME sys;
			GetLocalTime(&sys);
			strDefaultName.Format(_T("截图%4d%02d%02d%02d%02d%02d%03d"), sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
			wchar_t szPathName[MAX_PATH] = { 0 };
			OPENFILENAME ofn = { OPENFILENAME_SIZE_VERSION_400 };//or  {sizeof (OPENFILENAME)}  
			ofn.hwndOwner = GetForegroundWindow();			// 拥有者句柄	
			wcscpy_s(szPathName, strDefaultName.GetData());		// 定义预先的存储文件名
			ofn.lpstrFilter = TEXT("PNG(*.png)\0*.png\0\0");
			ofn.lpstrFile = szPathName;
			ofn.nMaxFile = sizeof(szPathName);
			ofn.lpstrTitle = TEXT("保存文件");
			ofn.Flags = OFN_OVERWRITEPROMPT;		// 覆盖提示
			BOOL bOk = GetSaveFileName(&ofn);
			if (bOk) {
				std::wstring strLastName = std::wstring(szPathName) + L".png";
				if (m_pDesktopCanvasContainer->SaveCapBmpToFile(strLastName)) {
					::PostQuitMessage(0L);
				}
			}
		}
		else if (strName == L"btn_cancel") {
			::PostQuitMessage(0L);
		}
		else if (strName == L"btn_finish") {
			m_pDesktopCanvasContainer->SaveCapBmpToClipboard();
			::PostQuitMessage(0L);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_KILLFOCUS) {
		if (strName == L"edit_word") {
			std::wstring strWord = m_pInputWordEdit->GetText().GetData();
			m_pInputWordEdit->SetVisible(false);
			m_pDesktopCanvasContainer->OnEditWordKillFocus(strWord);
		}
	}
}

void CScrCaptureWnd::OnMainBarSelectActionOp(ACTION_OP op)
{
	m_pDesktopCanvasContainer->SetActionOp(op);
	ShowMainBar();
	//初始化画笔
	m_pOptionSmallPen->Selected(true);
	m_pDesktopCanvasContainer->SetPenWidth(SUBBAR_SMALL_PEN);
	//默认颜色
	COptionUI* ptrColor = static_cast<COptionUI*>(m_pm.FindControl(_T("color_11")));
	ptrColor->Selected(true);
	m_pDesktopCanvasContainer->SetPenColor(0xFB3838);
	//默认12号字体
	m_pComboxFont->SelectItem(2);
	m_pDesktopCanvasContainer->SetFontSize(12);
	//默认模糊度80
	m_pSliderMosaic->SetValue(20);
}

void CScrCaptureWnd::OnSelectColor(int idx)
{
	COLORREF color = 0xff0000;
	switch (idx)
	{
	case 1: color = 0x000000; break;
	case 2: color = 0x808080; break;
	case 3: color = 0x800000; break;
	case 4: color = 0xF7883A; break;
	case 5: color = 0x308430; break;
	case 6: color = 0x385AD3; break;
	case 7: color = 0x800080; break;
	case 8: color = 0x009999; break;
	case 9: color = 0xFFFFFF; break;
	case 10: color = 0xC0C0C0; break;
	case 11: color = 0xFB3838; break;
	case 12: color = 0xFFFF00; break;
	case 13: color = 0x99CC00; break;
	case 14: color = 0x3894E4; break;
	case 15: color = 0xF31BF3; break;
	case 16: color = 0x16DCDC; break;
	default:
		break;
	}
	wchar_t szImage[128] = { 0 };
	wsprintf(szImage, L"file='color%d.png' corner='2,2,2,2'", idx);
	m_pLabelSelColor->SetAttribute(L"bkimage", szImage);
	m_pDesktopCanvasContainer->SetPenColor(color);
}

RECT CScrCaptureWnd::GetWindowRect() const
{
	return m_rcWindow;
}

RECT CScrCaptureWnd::GetClipPadding() const
{
	return m_rcClipPadding;
}

void CScrCaptureWnd::SetClipPadding(RECT rc) {
	m_rcClipPadding = rc;
	_stprintf_s(m_sClipDrawStringBuf, MAX_PATH - 1, _T("file='desktopimagemask' scale9='%d,%d,%d,%d' hole='true'"),
		m_rcClipPadding.left, m_rcClipPadding.top, m_rcClipPadding.right, m_rcClipPadding.bottom);
	m_pDesktopMask->SetBkImage(m_sClipDrawStringBuf);
	m_pDesktopMaskBorder->SetPos(GetClipRect());
	m_pDesktopCanvasContainer->SetPos(GetCanvasContainerRect());
	//显示截屏窗口大小
	RefreshClipWinSize();
	if (m_bClipChoiced)
		ShowMainBar();
}

//显示截屏窗口大小
void CScrCaptureWnd::RefreshClipWinSize()
{
	RECT canvasRc = GetCanvasContainerRect();
	RECT rcInset = m_pDesktopCanvasContainer->GetInset();
	canvasRc.left = canvasRc.left + rcInset.left;
	canvasRc.right = canvasRc.right - rcInset.right;
	canvasRc.top = canvasRc.top + rcInset.top;
	canvasRc.bottom = canvasRc.bottom - rcInset.bottom;

	wchar_t szSize[64] = { 0 };
	int itemWidth = canvasRc.right - canvasRc.left;
	int itemHeight = canvasRc.bottom - canvasRc.top;
	wsprintf(szSize, L"%dx%d", itemWidth, itemHeight);
	int txtWidth = 80;
	int txtHeight = 20;
	RECT txtRc = { canvasRc.left, canvasRc.top - txtHeight, canvasRc.left + txtWidth, canvasRc.top };
	//上边缘 超出
	if (txtRc.top - m_rcWindow.top < txtHeight) {
		txtRc.top = canvasRc.top;
		txtRc.bottom = txtRc.top + txtHeight;
	}
	m_pTxtCanvasSize->SetText(szSize);
	m_pTxtCanvasSize->SetPos(txtRc);
}

RECT CScrCaptureWnd::GetClipRect() const
{
	RECT rc = m_rcClipPadding;
	rc.left = rc.left + m_rcWindow.left;
	rc.top = rc.top + m_rcWindow.top;
	rc.right = m_rcWindow.right - rc.right;
	rc.bottom = m_rcWindow.bottom - rc.bottom;
	return rc;
}

RECT CScrCaptureWnd::GetCanvasContainerRect() const
{
	RECT rcInset = m_pDesktopCanvasContainer->GetInset();
	RECT rc = m_rcClipPadding;
	rc.left = rc.left + m_rcWindow.left - rcInset.left;
	rc.top = rc.top + m_rcWindow.top - rcInset.left;
	rc.right = m_rcWindow.right - rc.right + rcInset.left;
	rc.bottom = m_rcWindow.bottom - rc.bottom + rcInset.left;
	return rc;
}

LRESULT CScrCaptureWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN) {
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
			return true;
		}

	}
	return false;
}

LRESULT CScrCaptureWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE) {
		m_pm.Init(m_hWnd);
		m_pm.AddPreMessageFilter(this);

		CDialogBuilder builder;

		CDuiString strResourcePath = m_pm.GetResourcePath();
		if (strResourcePath.IsEmpty())
		{
			strResourcePath = m_pm.GetInstancePath();
			strResourcePath += GetSkinFolder().GetData();
		}
		m_pm.SetResourcePath(strResourcePath.GetData());

		switch (GetResourceType())
		{
		case UILIB_ZIP:
			m_pm.SetResourceZip(GetZIPFileName().GetData(), true);
			break;
		case UILIB_ZIPRESOURCE:
		{
			HRSRC hResource = ::FindResource(m_pm.GetResourceDll(), GetResourceID(), _T("ZIPRES"));
			if (hResource == NULL)
				return 0L;
			DWORD dwSize = 0;
			HGLOBAL hGlobal = ::LoadResource(m_pm.GetResourceDll(), hResource);
			if (hGlobal == NULL)
			{
#if defined(WIN32) && !defined(UNDER_CE)
				::FreeResource(hResource);
#endif
				return 0L;
			}
			dwSize = ::SizeofResource(m_pm.GetResourceDll(), hResource);
			if (dwSize == 0)
				return 0L;
			m_lpResourceZIPBuffer = new BYTE[dwSize];
			if (m_lpResourceZIPBuffer != NULL)
			{
				::CopyMemory(m_lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);
			}
#if defined(WIN32) && !defined(UNDER_CE)
			::FreeResource(hResource);
#endif
			m_pm.SetResourceZip(m_lpResourceZIPBuffer, dwSize);
		}
		break;
		}

		CDialogBuilderCallbackEx cb;
		CControlUI* pRoot = builder.Create(_T("ScrCapture.xml"), (UINT)0, &cb, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);
		Init();
		return 0;
	}
	else if (uMsg == WM_DESTROY) {
		::PostQuitMessage(0L);
	}
	else if (uMsg == WM_LBUTTONDOWN) {
		if (!m_bClipChoiced) {
			m_pDesktopCanvasContainer->SetEnabled(false);
			m_ptClipBasePoint.x = GET_X_LPARAM(lParam);
			m_ptClipBasePoint.y = GET_Y_LPARAM(lParam);
			return 0;
		}
	}
	else if (uMsg == WM_LBUTTONUP) {
		if (!m_bClipChoiced) {
			m_bClipChoiced = true;
			m_pMagnifierCtrl->SetVisible(false);
			m_pDesktopMaskBorder->SetVisible(false);
			m_pDesktopCanvasContainer->SetVisible(true);
			m_pDesktopCanvasContainer->SetEnabled(true);
			SetClipPadding(m_rcClipPadding);
			//显示工具栏
			ShowMainBar();
			return 0;
		}
		else {

		}
	}
	else if (uMsg == WM_LBUTTONDBLCLK) {
		// todo:完成截图，保存到剪贴板
		m_bClipChoiced = true;
		::PostQuitMessage(0L);
		return 0;
	}
	else if (uMsg == WM_RBUTTONUP) {
		if (m_bClipChoiced) {
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			RECT rcClipRect = GetClipRect();
			if (::PtInRect(&rcClipRect, pt)) {
				::ClientToScreen(m_hWnd, &pt);
				::TrackPopupMenu(m_hEditMenu, TPM_TOPALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
				::PostMessage(m_hWnd, WM_NULL, 0, 0);
			}
			else {
				m_bClipChoiced = false;
				m_pDesktopCanvasContainer->SetEnabled(false);
				::SendMessage(m_hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
			}
		}
		else {
			// 退出截图
			::PostQuitMessage(0L);
		}
		return 0;
	}
	else if (uMsg == WM_MOUSEMOVE) {
		if (!m_bClipChoiced) {
			if (::GetKeyState(VK_LBUTTON) < 0) { // 左键按下状态
				if (m_ptClipBasePoint.x != GET_X_LPARAM(lParam) || m_ptClipBasePoint.y != GET_Y_LPARAM(lParam)) {
					m_rcClipPadding.left = min(GET_X_LPARAM(lParam), m_ptClipBasePoint.x) - m_rcWindow.left; if (m_rcClipPadding.left < 0) m_rcClipPadding.left = 0;
					m_rcClipPadding.top = min(GET_Y_LPARAM(lParam), m_ptClipBasePoint.y) - m_rcWindow.top; if (m_rcClipPadding.top < 0) m_rcClipPadding.top = 0;
					m_rcClipPadding.right = m_rcWindow.right - max(GET_X_LPARAM(lParam), m_ptClipBasePoint.x); if (m_rcClipPadding.right < 0) m_rcClipPadding.right = 0;
					m_rcClipPadding.bottom = m_rcWindow.bottom - max(GET_Y_LPARAM(lParam), m_ptClipBasePoint.y); if (m_rcClipPadding.bottom < 0) m_rcClipPadding.bottom = 0;
					m_pDesktopMaskBorder->SetVisible(false);
					m_pDesktopCanvasContainer->SetVisible(true);
					SetClipPadding(m_rcClipPadding);
				}
			}
			else {
				RECT rc;
				HWND hWnd;
				::EnableWindow(m_hWnd, FALSE);
				hWnd = SmallestWindowFromCursor(rc);
				::EnableWindow(m_hWnd, TRUE);
				::SetFocus(m_hWnd);
				rc.left = rc.left - m_rcWindow.left; if (rc.left < 0) rc.left = 0;
				rc.top = rc.top - m_rcWindow.top; if (rc.top < 0) rc.top = 0;
				rc.right = m_rcWindow.right - rc.right; if (rc.right < 0) rc.right = 0;
				rc.bottom = m_rcWindow.bottom - rc.bottom; if (rc.bottom < 0) rc.bottom = 0;
				if (rc.left != m_rcClipPadding.left || rc.right != m_rcClipPadding.right || rc.top != m_rcClipPadding.top || rc.bottom != m_rcClipPadding.bottom) {
					SetClipPadding(rc);
					m_pDesktopMaskBorder->SetVisible(true);
					m_pDesktopCanvasContainer->SetVisible(false);
				}
				//放大镜
				if (m_pMagnifierCtrl) {
					if (!m_pMagnifierCtrl->IsVisible()) {
						m_pMagnifierCtrl->SetVisible(true);
					}
					else {
						POINT pt;
						::GetCursorPos(&pt);
						m_pMagnifierCtrl->SetSrcImagePos(pt.x, pt.y);
						m_pDesktopMask->Invalidate();
					}
				}
				RefreshClipWinSize();
			}
		}
		else {

		}
	}
	else if (uMsg == WM_COMMAND) {

	}
	LRESULT lRes = 0;
	if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}


void CScrCaptureWnd::ShowMainBar(bool bShow/* = true*/)
{
	if (!bShow) {
		m_pMainBar->SetVisible(false);
		m_pSubBar->SetVisible(false);
		return;
	}
	if (!m_pMainBar->IsVisible()) {
		m_pMainBar->SetVisible(true);
	}
	m_isSubOnBottom = true;
	RECT canvasRc = GetCanvasContainerRect();
	RECT rcInset = m_pDesktopCanvasContainer->GetInset();
	canvasRc.left = canvasRc.left + rcInset.left;
	canvasRc.right = canvasRc.right - rcInset.right;
	canvasRc.top = canvasRc.top + rcInset.top;
	canvasRc.bottom = canvasRc.bottom - rcInset.bottom;

	RECT mainBarRc;
	//默认右下角
	mainBarRc.left = canvasRc.right - MAINBAR_WIDTH;
	mainBarRc.top = canvasRc.bottom + SPE_MAINBAR_CANVAS;

	//超出左边框
	if (mainBarRc.left < 0) {
		mainBarRc.left = 0;
	}
	int totalHeight = MAINBAR_HEIGHT;
	//如果显示sub工具栏
	if (m_pDesktopCanvasContainer->GetActionOp() != ACTION_OP_NONE) {
		totalHeight = totalHeight + SUBBAR_HEIGHT + SPE_MAINBAR_SUNBAR;//子工具栏 + 间隔2
	}
	//超出下边栏 则显示到上边栏去
	if (mainBarRc.top + totalHeight > m_rcWindow.bottom) {
		mainBarRc.top = canvasRc.top - totalHeight - SPE_MAINBAR_CANVAS;
		//如果上边还是溢出 则显示内部
		if (mainBarRc.top < 0) {
			mainBarRc.top = canvasRc.top + SPE_MAINBAR_CANVAS;
			m_isSubOnBottom = true;
		}
		else {
			//显示在 mainbar 上边栏
			mainBarRc.top = canvasRc.top - MAINBAR_HEIGHT - SPE_MAINBAR_SUNBAR;
			m_isSubOnBottom = false;
		}
	}
	mainBarRc.right = mainBarRc.left + MAINBAR_WIDTH;
	mainBarRc.bottom = mainBarRc.top + totalHeight;
	SIZE size = { mainBarRc.left, mainBarRc.top };
	m_pMainBar->SetFixedXY(size);
	//m_pMainBar->Move(size);
	if (m_pDesktopCanvasContainer->GetActionOp() != ACTION_OP_NONE) {
		ShowSubBar();
	}
}

void CScrCaptureWnd::ShowSubBar()
{
	bool bShowPen = true;
	bool bShowColor = true;
	switch (m_pDesktopCanvasContainer->GetActionOp())
	{
	case ACTION_OP_RECT:
	{
		bShowPen = true;
		bShowColor = true;
	}break;
	case ACTION_OP_ELLIPSE:
	{
		bShowPen = true;
		bShowColor = true;
	}break;
	case ACTION_OP_LINE:
	{
		bShowPen = true;
		bShowColor = true;
	}break;
	case ACTION_OP_ARROW:
	{
		bShowPen = true;
		bShowColor = true;
	}break;
	case ACTION_OP_SCRAWL:
	{
		bShowPen = true;
		bShowColor = true;
	}break;
	case ACTION_OP_HIGHLIGHT:
	{
		bShowPen = true;
		bShowColor = true;
	}break;
	case ACTION_OP_MOSAIC:
	{
		bShowPen = true;
		bShowColor = false;
	}break;
	default:
	case ACTION_OP_TXT:
	{
		bShowPen = false;
		bShowColor = true;
	}break;
	}

	m_pPenLayout->SetVisible(bShowPen);
	m_pFontLayout->SetVisible(!bShowPen);
	m_pColorSplitLayout->SetVisible(bShowPen);
	m_pColorSelectLayout->SetVisible(bShowColor);
	m_pColorMosaicLayout->SetVisible(!bShowColor);

	int width = SUBBAR_PEN_WIDTH + SUBBAR_PEN_SEP_WIDTH + SUBBAR_COLOR_WIDTH;
	//pen + sep + color
	if (bShowPen && bShowColor) {
		width = SUBBAR_PEN_WIDTH + SUBBAR_PEN_SEP_WIDTH + SUBBAR_COLOR_WIDTH;
	}
	//pen + sep + mosaic
	else if (bShowPen && !bShowColor) {
		width = SUBBAR_PEN_WIDTH + SUBBAR_PEN_SEP_WIDTH + SUBBAR_MOSAIC_WIDTH;
	}
	//t + color
	else if (!bShowPen && bShowColor) {
		width = SUBBAR_TXT_WIDTH + SUBBAR_COLOR_WIDTH;
	}

	SIZE mainBarPos = m_pMainBar->GetFixedXY();
	SIZE subBarPos = mainBarPos;
	//显示在mainbar下方
	if (m_isSubOnBottom) {
		subBarPos.cy = mainBarPos.cy + MAINBAR_HEIGHT + 2;
	}
	else {
		//显示在mainbar上方
		subBarPos.cy = mainBarPos.cy - SUBBAR_HEIGHT - 2;
	}
	m_pSubBar->SetFixedXY(subBarPos);
	m_pSubBar->SetFixedWidth(width);
	if (!m_pSubBar->IsVisible()) {
		m_pSubBar->SetVisible(true);
	}
}

void CScrCaptureWnd::ResetInputWordEditPos(const RECT& rc, const std::wstring& strWord)
{
	SIZE pos = { rc.left, rc.top };
	m_pInputWordEdit->SetFixedXY(pos);
	m_pInputWordEdit->SetFixedWidth(rc.right - rc.left);
	m_pInputWordEdit->SetFixedHeight(rc.bottom - rc.top);
	if (!m_pInputWordEdit->IsVisible()) {
		m_pInputWordEdit->SetVisible(true);
	}
	m_pInputWordEdit->SetText(strWord.c_str());
	m_pInputWordEdit->SetFocus();
}