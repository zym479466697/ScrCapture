#include "MagnifierUI.h"
#include "Util.h"

CMagnifierUI::CMagnifierUI()
{
	//border size
	SetAttribute(L"bordersize", L"1");
	SetAttribute(L"bordercolor", L"#ff00A2E9");
	SetAttribute(L"bkcolor", L"#c8000000");
	SetFixedHeight(180);
	SetFixedWidth(120);
	SetFixedXY({ 0, 0 });
	m_imgSrcX = 0;
	m_imgSrcY = 0;
	m_imgSrcR = 0;
	m_imgSrcG = 0;
	m_imgSrcB = 0;
	m_WndDesktopRect = { 0, 0, 1920, 1080 };
	HWND hWndDesktop = ::GetDesktopWindow();
	GetWindowRect(hWndDesktop, &m_WndDesktopRect);
}

CMagnifierUI::~CMagnifierUI()
{

}

LPCTSTR CMagnifierUI::GetClass() const
{
	return DUI_CTR_MAGNIFIER;
}

LPVOID CMagnifierUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, DUI_CTR_MAGNIFIER) == 0 )
		return static_cast<CMagnifierUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CMagnifierUI::DoEvent(TEventUI& event)
{
	// if m_rcItem.left < 20 左边可以修改
	CControlUI::DoEvent(event);
}

void CMagnifierUI::SetSrcImagePos(int x, int y)
{
	m_imgSrcX = x;
	m_imgSrcY = y;
	if (m_diBk.pImageInfo != NULL) {
		GetDestImageRgb(x, y, m_imgSrcR, m_imgSrcG, m_imgSrcB);
	}
	m_cXY.cx = x + 20;
	m_cXY.cy = y + 20;

	if (m_cXY.cx + 120 > m_WndDesktopRect.right) {
		m_cXY.cx = x - 20 - 120;
	}
	if (m_cXY.cy + 180 > m_WndDesktopRect.bottom) {
		m_cXY.cy = y - 20 - 180;
	}
	m_rcItem.left = m_cXY.cx;
	m_rcItem.right = m_cXY.cx + 120;
	m_rcItem.top = m_cXY.cy;
	m_rcItem.bottom = m_cXY.cy + 180;
	
	Invalidate();
}


void CMagnifierUI::PaintBkColor(HDC hDC)
{
	CControlUI::PaintBkColor(hDC);
	//先绘制背景黑色
	//RECT rcDest = { 0, 0, 120, 120 };
	RECT rcDest = m_rcItem;
	rcDest.bottom = rcDest.top + 120;

	CRenderEngine::DrawColor(hDC, rcDest, 0xff000000);
}

void CMagnifierUI::PaintBkImage(HDC hDC)
{
	RECT rcDest = m_rcItem;
	rcDest.bottom = rcDest.top + 120;

	if (m_diBk.pImageInfo == NULL) {
		CRenderEngine::DrawImage(hDC, m_pManager, rcDest, m_rcPaint, m_diBk);
	}
	//DrawImage(hDC, m_diBk);
	
	//放大1倍
	RECT rcBmpPart = { m_imgSrcX, m_imgSrcY, m_imgSrcX + 60, m_imgSrcY + 60 };
	CRenderEngine::DrawImage(hDC, m_diBk.pImageInfo->hBitmap, rcDest, m_rcPaint, rcBmpPart, m_diBk.rcScale9,
		m_diBk.pImageInfo->bAlpha, m_diBk.uFade, m_diBk.bHole, m_diBk.bTiledX, m_diBk.bTiledY);

	//DrawImage(hDC, drawInfo.pImageInfo->hBitmap, rcDest, rcPaint, drawInfo.rcBmpPart, drawInfo.rcScale9,
	//	drawInfo.pImageInfo->bAlpha, drawInfo.uFade, drawInfo.bHole, drawInfo.bTiledX, drawInfo.bTiledY);
	//CRenderEngine::DrawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint,
	//	const RECT& rcBmpPart, const RECT& rcScale9, bool bAlpha,
	//	BYTE uFade, bool bHole, bool bTiledX, bool bTiledY)

}

void CMagnifierUI::PaintText(HDC hDC)
{
	wchar_t szPos[64] = { 0 };
	wchar_t szColor[64] = { 0 };
	wchar_t szTips[64] = L"双击可完成复制";
	wsprintf(szPos, L"坐标：(%d, %d)", m_imgSrcX, m_imgSrcY);
	wsprintf(szColor, L"RGB：(%d,%d,%d)", m_imgSrcR, m_imgSrcG, m_imgSrcB);

	RECT rcPos, rcColor, rcTips;
	rcPos = rcColor = rcTips = m_rcItem;

	rcPos.top = rcPos.top + 120;
	rcPos.bottom = rcPos.top + 20;
	rcPos.left = rcPos.left + 3;
	rcPos.right = rcPos.right - 3;

	rcColor.top = rcColor.top + 140;
	rcColor.bottom = rcColor.top + 20;
	rcColor.left = rcColor.left + 3;
	rcColor.right = rcColor.right - 3;

	rcTips.top = rcTips.top + 160;
	rcTips.bottom = rcTips.top + 20;

	CRenderEngine::DrawText(hDC, m_pManager, rcPos, szPos, 0xffcccccc, -1, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	CRenderEngine::DrawText(hDC, m_pManager, rcColor, szColor, 0xffcccccc, -1, DT_SINGLELINE |DT_LEFT | DT_VCENTER);
	CRenderEngine::DrawText(hDC, m_pManager, rcTips, szTips, 0xffcccccc, -1, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

void CMagnifierUI::PaintBorder(HDC hDC)
{
	RECT rcBorder = m_rcItem;
	rcBorder.bottom = rcBorder.top + 120;
	CRenderEngine::DrawRect(hDC, rcBorder, m_rcBorderSize.left, GetAdjustColor(m_dwBorderColor), m_nBorderStyle);

	rcBorder = { m_rcItem.left + 60, m_rcItem.top, m_rcItem.left + 60, m_rcItem.bottom - 60};
	CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.left, GetAdjustColor(m_dwBorderColor), m_nBorderStyle);

	rcBorder = { m_rcItem.left, m_rcItem.top + 60 , m_rcItem.right, m_rcItem.top + 60 };
	CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.left, GetAdjustColor(m_dwBorderColor), m_nBorderStyle);
}