#pragma once
#include "Define.h"
#include "DrawGraph.h"
#include <vector>
#include "BitmapEx.h"

class CCanvasContainerUI: public CContainerUI
{
public:
	CCanvasContainerUI();
	virtual ~CCanvasContainerUI();
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	UINT GetControlFlags() const;

	// 0    1    2
	// 3    4    5
	// 6    7    8
	ACTION_OP GetActionOp();
	void SetActionOp(ACTION_OP op);
	RECT GetSizerRect(int iIndex);
	LPTSTR GetSizerCursor(POINT& pt, int& iCursor);
	void PaintBorder(HDC hDC);
	void PaintStatusImage(HDC hDC);
	void DoEvent(TEventUI& event);

	//设置绘制相关信息
	void SetPenWidth(int width);
	void SetPenColor(COLORREF color);
	void SetFontSize(int size);
	void SetMosaicValue(int value);
	void MakeMosaicBitMap();
	void Undo();

	BOOL SaveCapBmpToFile(const std::wstring& strSaveFile);
	void SaveCapBmpToClipboard();
private:
	BOOL GetImageCLSID(const WCHAR* format, CLSID* pCLSID);
protected:
	int m_iCursor;
	UINT m_uButtonState;
	POINT m_ptClipBasePoint; // 调整基准点
	POINT m_ptLastMouse; // 
	RECT m_WndDesktopRect;
	ACTION_OP m_actionOp;
	std::vector<IGraph*> m_graphItemList;

	//绘图相关
	int m_penWidth;
	COLORREF m_penColor;
	int m_fontSize;
	int m_mosaicValue;
	IGraph *m_pGraph;
};
