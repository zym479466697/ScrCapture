#include "ScrCaptureWnd.h"
#include "CanvasContainer.h"
#include "CommFunc.h"
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

CCanvasContainerUI::CCanvasContainerUI() : m_iCursor(0), m_uButtonState(0)
{
	m_penWidth = 2;
	m_penColor = 0xff0000;
	m_fontSize = 12;
	m_mosaicValue = 4;
	m_pGraph = nullptr;

	m_actionOp = ACTION_OP_NONE;
	m_ptClipBasePoint.x = m_ptClipBasePoint.y = 0;
	m_ptLastMouse.x = m_ptLastMouse.y = 0;
	m_WndDesktopRect = { 0, 0, 1920, 1080 };
	HWND hWndDesktop = ::GetDesktopWindow();
	GetWindowRect(hWndDesktop, &m_WndDesktopRect);
}

CCanvasContainerUI::~CCanvasContainerUI() 
{
	for (size_t i = 0; i < m_graphItemList.size(); i++)
	{
		IGraph* ptr = m_graphItemList[i];
		delete ptr;
		ptr = nullptr;
	}
	m_graphItemList.clear();
}

LPCTSTR CCanvasContainerUI::GetClass() const
{
	return DUI_CTR_CANVASCONTAINER;
}

LPVOID CCanvasContainerUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_CANVASCONTAINER) == 0 ) return static_cast<CCanvasContainerUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

UINT CCanvasContainerUI::GetControlFlags() const
{
	return UIFLAG_SETCURSOR;
}

RECT CCanvasContainerUI::GetSizerRect(int iIndex)
{
	LONG lMiddleX = (m_rcItem.left + m_rcItem.right) / 2;
	LONG lMiddleY = (m_rcItem.top + m_rcItem.bottom) / 2;
	LONG SIZER_WIDTH = m_rcInset.left*2;
	switch( iIndex ) {
	case 0:
		return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left + SIZER_WIDTH, m_rcItem.top + SIZER_WIDTH);
	case 1:
		return CDuiRect(lMiddleX - SIZER_WIDTH/2, m_rcItem.top, lMiddleX + SIZER_WIDTH/2, m_rcItem.top + SIZER_WIDTH);
	case 2:
		return CDuiRect(m_rcItem.right - SIZER_WIDTH, m_rcItem.top, m_rcItem.right, m_rcItem.top + SIZER_WIDTH);
	case 3:
		return CDuiRect(m_rcItem.left, lMiddleY - SIZER_WIDTH/2, m_rcItem.left + SIZER_WIDTH, lMiddleY + SIZER_WIDTH/2);
	case 4:
		break;
	case 5:
		return CDuiRect(m_rcItem.right - SIZER_WIDTH, lMiddleY - SIZER_WIDTH/2, m_rcItem.right, lMiddleY + SIZER_WIDTH/2);
	case 6:
		return CDuiRect(m_rcItem.left, m_rcItem.bottom - SIZER_WIDTH, m_rcItem.left + SIZER_WIDTH, m_rcItem.bottom);
	case 7:
		return CDuiRect(lMiddleX - SIZER_WIDTH/2, m_rcItem.bottom - SIZER_WIDTH, lMiddleX + SIZER_WIDTH/2, m_rcItem.bottom);
	case 8:
		return CDuiRect(m_rcItem.right - SIZER_WIDTH, m_rcItem.bottom - SIZER_WIDTH, m_rcItem.right, m_rcItem.bottom);
	default:
		break;
	}
	return CDuiRect();
}

LPTSTR CCanvasContainerUI::GetSizerCursor(POINT& pt, int& iCursor)
{
	LONG SIZER_WIDTH = m_rcInset.left*2;
	LONG SIZER_TO_ROOT = 20;	
	RECT rcRoot = m_pManager->GetRoot()->GetPos();

	iCursor = -1;
	for( int i = 8; i >= 0; --i ) {
		RECT rc = GetSizerRect(i);
		if( ::PtInRect(&rc, pt) ) {
			iCursor = i;
			break;
		}
	}
	if (iCursor < 0 && rcRoot.bottom - m_rcItem.bottom < SIZER_TO_ROOT) {
		RECT rc = CDuiRect(m_rcItem.left, m_rcItem.bottom-SIZER_WIDTH, m_rcItem.right, m_rcItem.bottom);
		if( ::PtInRect(&rc, pt) ) {
			iCursor = 7;
		}
	}
	if (iCursor < 0 && rcRoot.right - m_rcItem.right < SIZER_TO_ROOT) {
		RECT rc = CDuiRect(m_rcItem.right-SIZER_WIDTH, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
		if( ::PtInRect(&rc, pt) ) {
			iCursor = 5;
		}
	}
	if (iCursor < 0 && m_rcItem.left - rcRoot.left < SIZER_TO_ROOT) {
		RECT rc = CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left+SIZER_WIDTH, m_rcItem.bottom);
		if( ::PtInRect(&rc, pt) ) {
			iCursor = 3;
		}
	}
	if (iCursor < 0 && m_rcItem.top - rcRoot.top < SIZER_TO_ROOT) {
		RECT rc = CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.right, m_rcItem.top+SIZER_WIDTH);
		if( ::PtInRect(&rc, pt) ) {
			iCursor = 1;
		}
	}

	//如果有状态 改成+星
	if (m_actionOp != ACTION_OP_NONE) {
		if (iCursor < 0) {
			return IDC_CROSS;
		}
		return IDC_ARROW;
	}
	if (iCursor < 0) {
		iCursor = 4;
	}
	switch( iCursor ) {
	case 0:
		return IDC_SIZENWSE;
	case 1:
		return IDC_SIZENS;
	case 2:
		return IDC_SIZENESW;
	case 3:
		return IDC_SIZEWE;
	case 4:
		return IDC_SIZEALL;
	case 5:
		return IDC_SIZEWE;
	case 6:
		return IDC_SIZENESW;
	case 7:
		return IDC_SIZENS;
	case 8:
		return IDC_SIZENWSE;
	default: {
			return IDC_SIZEALL;
		}
	}
}

void CCanvasContainerUI::PaintBorder(HDC hDC) {
	//如果是不绘制状态
	if (m_actionOp == ACTION_OP_NONE) {
		for (int i = 0; i < 9; ++i) {
			CRenderEngine::DrawColor(hDC, GetSizerRect(i), m_dwBorderColor);
		}
	}
	RECT rc = m_rcItem;
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.left;
	rc.right -= m_rcInset.left;
	rc.bottom -= m_rcInset.left;
	CRenderEngine::DrawRect(hDC, rc, m_rcBorderSize.left, m_dwBorderColor);
}


void CCanvasContainerUI::DoEvent(TEventUI& event) 
{
	if( event.Type == UIEVENT_SETCURSOR && IsEnabled())
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) return;
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(GetSizerCursor(event.ptMouse, m_iCursor))));
		return;
	}
	if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
	{
		m_uButtonState |= UISTATE_CAPTURED;
		m_ptLastMouse = event.ptMouse;
		//绘制状态
		if (m_actionOp != ACTION_OP_NONE) {
			switch (m_actionOp)
			{
			case ACTION_OP_RECT: 
			case ACTION_OP_ELLIPSE:
			case ACTION_OP_LINE:
			case ACTION_OP_ARROW:
			case ACTION_OP_SCRAWL:
			case ACTION_OP_HIGHLIGHT:
			case ACTION_OP_MOSAIC:
			{
				m_pGraph = GraphFactory::CreateGraph(m_actionOp);
				m_pGraph->SetPen(m_penWidth, m_penColor, RectX(m_rcItem).DeflateRect(-3, -3));
				POINT ptEnd = { m_ptLastMouse.x + 1, m_ptLastMouse.y + 1 };
				m_pGraph->ResetGraphPos(m_ptLastMouse, ptEnd);
			}break;
			case ACTION_OP_TXT:
				break;
			default:
				break;
			}
			return;
		}

		RECT rcWindow = CScrCaptureWnd::Instance()->GetWindowRect();
		RECT rcClipPadding = CScrCaptureWnd::Instance()->GetClipPadding();
		switch( m_iCursor ) {
		case 0:
		case 1:
		case 3:
			m_ptClipBasePoint.x = rcWindow.right-rcClipPadding.right;
			m_ptClipBasePoint.y = rcWindow.bottom-rcClipPadding.bottom;
			break;
		case 2:
		case 5:
			m_ptClipBasePoint.x = rcWindow.left+rcClipPadding.left;
			m_ptClipBasePoint.y = rcWindow.bottom-rcClipPadding.bottom;
			break;
		case 6:
		case 7:
			m_ptClipBasePoint.x = rcWindow.right-rcClipPadding.right;
			m_ptClipBasePoint.y = rcWindow.top+rcClipPadding.top;
			break;
		case 8:
		default:
			m_ptClipBasePoint.x = rcWindow.left+rcClipPadding.left;
			m_ptClipBasePoint.y = rcWindow.top+rcClipPadding.top;
			break;
		}
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			m_uButtonState &= ~UISTATE_CAPTURED;

			//绘制状态
			if (m_actionOp != ACTION_OP_NONE) {
				switch (m_actionOp)
				{
				case ACTION_OP_RECT: 
				case ACTION_OP_ELLIPSE:
				case ACTION_OP_LINE:
				case ACTION_OP_ARROW:
				case ACTION_OP_SCRAWL:
				case ACTION_OP_HIGHLIGHT:
				case ACTION_OP_MOSAIC:
					{
						m_pGraph->ResetGraphPos(m_ptLastMouse, event.ptMouse);
						m_graphItemList.push_back(m_pGraph);
						m_pGraph = nullptr;
					}break;
				case ACTION_OP_TXT:
					break;
				default:
					break;
				}
				return;
			}
		}
		return;
	}
	else if( event.Type == UIEVENT_MOUSEMOVE )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) == 0 ) return;

		//绘制状态
		if (m_actionOp != ACTION_OP_NONE) {
			switch (m_actionOp)
			{
			case ACTION_OP_RECT: 
			case ACTION_OP_ELLIPSE:
			case ACTION_OP_LINE:
			case ACTION_OP_ARROW:
			case ACTION_OP_SCRAWL:
			case ACTION_OP_HIGHLIGHT:
			case ACTION_OP_MOSAIC:
				{
					m_pGraph->ResetGraphPos(m_ptLastMouse, event.ptMouse);
					Invalidate();
				} break;
			case ACTION_OP_TXT:
				break;
			default:
				break;
			}
			CContainerUI::DoEvent(event);
			return;
		}

		RECT rcWindow = CScrCaptureWnd::Instance()->GetWindowRect();
		RECT rcClipPadding = CScrCaptureWnd::Instance()->GetClipPadding();
		switch( m_iCursor ) {
		case 0:
		case 2:
		case 6:
		case 8:
			rcClipPadding.left = min(event.ptMouse.x, m_ptClipBasePoint.x)-rcWindow.left; if (rcClipPadding.left < 0) rcClipPadding.left = 0;
			rcClipPadding.top = min(event.ptMouse.y, m_ptClipBasePoint.y)-rcWindow.top; if (rcClipPadding.top < 0) rcClipPadding.top = 0;
			rcClipPadding.right = rcWindow.right-max(event.ptMouse.x, m_ptClipBasePoint.x); if (rcClipPadding.right < 0) rcClipPadding.right = 0;
			rcClipPadding.bottom = rcWindow.bottom-max(event.ptMouse.y, m_ptClipBasePoint.y); if (rcClipPadding.bottom < 0) rcClipPadding.bottom = 0;
			break;
		case 1:
		case 7:
			rcClipPadding.top = min(event.ptMouse.y, m_ptClipBasePoint.y)-rcWindow.top; if (rcClipPadding.top < 0) rcClipPadding.top = 0;
			rcClipPadding.bottom = rcWindow.bottom-max(event.ptMouse.y, m_ptClipBasePoint.y); if (rcClipPadding.bottom < 0) rcClipPadding.bottom = 0;
			break;
		case 3:
		case 5:
			rcClipPadding.left = min(event.ptMouse.x, m_ptClipBasePoint.x)-rcWindow.left; if (rcClipPadding.left < 0) rcClipPadding.left = 0;
			rcClipPadding.right = rcWindow.right-max(event.ptMouse.x, m_ptClipBasePoint.x); if (rcClipPadding.right < 0) rcClipPadding.right = 0;
			break;
		default:
			{
				LONG cx = event.ptMouse.x - m_ptLastMouse.x;
				LONG cy = event.ptMouse.y - m_ptLastMouse.y;
				if (rcClipPadding.left + cx >= 0 && rcClipPadding.right - cx >=0) {
					rcClipPadding.left += cx;
					rcClipPadding.right -= cx;
				}
				if (rcClipPadding.top + cy >= 0 && rcClipPadding.bottom - cy >=0) {
					rcClipPadding.top += cy;
					rcClipPadding.bottom -= cy;
				}
			}
			break;
		}
		m_ptLastMouse = event.ptMouse;
		CScrCaptureWnd::Instance()->SetClipPadding(rcClipPadding);
	}
	CContainerUI::DoEvent(event);
}


void CCanvasContainerUI::SetActionOp(ACTION_OP op)
{
	m_actionOp = op;
	if (GraphMosaic::s_maskBitmap == NULL) {
		MakeMosaicBitMap();
	}
}

ACTION_OP CCanvasContainerUI::GetActionOp()
{
	return m_actionOp;
}


//设置绘制相关信息
void CCanvasContainerUI::SetPenWidth(int width)
{
	m_penWidth = width;
}

void CCanvasContainerUI::SetPenColor(COLORREF color)
{
	m_penColor = color;
}

void CCanvasContainerUI::SetFontSize(int size)
{
	m_fontSize = size;
}

void CCanvasContainerUI::SetMosaicValue(int value)
{
	m_mosaicValue = 4 + value / 10;
	MakeMosaicBitMap();
	Invalidate();
}

void CCanvasContainerUI::MakeMosaicBitMap()
{
	HBITMAP deskBitMap = m_pManager->GetImage(L"desktopimage")->hBitmap;
	CBitmapEx bitmapEx;
	bitmapEx.Load(deskBitMap);
	//RECT cutRc = m_rcItem;
	//bitmapEx.Crop(cutRc.left, cutRc.top, cutRc.right - cutRc.left, cutRc.bottom - cutRc.top);
	bitmapEx.Pixelize(m_mosaicValue);
	HBITMAP bitMap = NULL;
	bitmapEx.Save(bitMap);
	Gdiplus::Bitmap* mosaicBitMap = Gdiplus::Bitmap::FromHBITMAP(bitMap, NULL);
	GraphMosaic::SetMaskBitMap(mosaicBitMap);
}

void CCanvasContainerUI::PaintStatusImage(HDC hDC)
{
	for (int idx = 0; idx < m_graphItemList.size(); idx++) {
		m_graphItemList[idx]->DrawGraph(hDC);
	}
	if (m_pGraph) {
		m_pGraph->DrawGraph(hDC);
	}
}


BOOL CCanvasContainerUI::GetImageCLSID(const WCHAR* format, CLSID* pCLSID)
{
	UINT num = 0;
	UINT size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0) {
		return FALSE;
	}
	pImageCodecInfo = (ImageCodecInfo *)(malloc(size));
	if (pImageCodecInfo == NULL)
		return FALSE;
	GetImageEncoders(num, size, pImageCodecInfo);

	// Find for the support of format for image in the windows
	for (UINT i = 0; i < num; ++i)
	{
		//MimeType: Depiction for the program image  
		if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
		{
			*pCLSID = pImageCodecInfo[i].Clsid;
			free(pImageCodecInfo);
			return TRUE;
		}
	}
	free(pImageCodecInfo);
	return FALSE;
}

BOOL CCanvasContainerUI::SaveCapBmpToFile(const std::wstring& strSaveFile)
{
	if (strSaveFile.empty())
		return FALSE;

	CLSID   imgClsid;
	std::wstring strExt = PathFindExtensionW(strSaveFile.c_str());
	std::wstring strEncoder = L"image/jpeg";
	if (endWith(strSaveFile, L".png")) {
		strEncoder = L"image/png";
	}
	else if (endWith(strSaveFile, L".bmp")) {
		strEncoder = L"image/bmp";
	}
	else if (endWith(strSaveFile, L".gif")) {
		strEncoder = L"image/gif";
	}
	else if (endWith(strSaveFile, L".tiff")) {
		strEncoder = L"image/tiff";
	}

	GetImageCLSID(strEncoder.c_str(), &imgClsid);
	int nSaveWidth = m_rcItem.right - m_rcItem.left;
	int nSaveHeight = m_rcItem.bottom - m_rcItem.top;
	
	BOOL bRet = FALSE;
	//将目标区域贴图到内存BITMAP  
	HDC memDC = CreateCompatibleDC(m_pManager->GetPaintDC());
	HBITMAP hBmp = CreateCompatibleBitmap(m_pManager->GetPaintDC(), nSaveWidth, nSaveHeight);
	SelectObject(memDC, hBmp);
	BitBlt(memDC, 0, 0, nSaveWidth, nSaveHeight,
		m_pManager->GetPaintDC(), m_rcItem.left, m_rcItem.top, SRCCOPY);

	//保存成文件  
	{
		//L"image/bmp" L"image/jpeg"  L"image/gif" L"image/tiff" L"image/png"  
		Gdiplus::Bitmap *pbmSrc = Gdiplus::Bitmap::FromHBITMAP(hBmp, NULL);
		if (pbmSrc->Save(strSaveFile.c_str(), &imgClsid) == Ok){
			bRet = TRUE;
		}
		delete pbmSrc;
		pbmSrc = NULL;
	}

	//清理工作  
	SelectObject(memDC, (HBITMAP)NULL);
	DeleteDC(memDC);
	DeleteObject(hBmp);
	return bRet;
}


void CCanvasContainerUI::SaveCapBmpToClipboard()
{
	int nSaveWidth = m_rcItem.right - m_rcItem.left;
	int nSaveHeight = m_rcItem.bottom - m_rcItem.top;
	//将目标区域贴图到内存BITMAP  
	HDC memDC = CreateCompatibleDC(m_pManager->GetPaintDC());
	HBITMAP hBmp = CreateCompatibleBitmap(m_pManager->GetPaintDC(), nSaveWidth, nSaveHeight);
	SelectObject(memDC, hBmp);
	BitBlt(memDC, 0, 0, nSaveWidth, nSaveHeight,
		m_pManager->GetPaintDC(), m_rcItem.left, m_rcItem.top, SRCCOPY);
	//清理工作  
	SelectObject(memDC, (HBITMAP)NULL);
	DeleteDC(memDC);
	if (OpenClipboard(NULL))
	{
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, hBmp);
		CloseClipboard();
	}
	DeleteObject(hBmp);
}

void CCanvasContainerUI::Undo()
{
	if (m_graphItemList.empty()) {
		return;
	}
	m_graphItemList.erase(m_graphItemList.begin() + (m_graphItemList.size() - 1));
	Invalidate();
}