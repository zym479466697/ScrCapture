#ifndef MAGNIFIERUI_H_
#define MAGNIFIERUI_H_
#include "Define.h"

class CMagnifierUI : public CControlUI
{
public:
	CMagnifierUI();
	~CMagnifierUI();
	void SetSrcImagePos(int x, int y);
protected:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void DoEvent(TEventUI& event) ;
	virtual void PaintBkColor(HDC hDC);
	virtual void PaintBkImage(HDC hDC);
	virtual void PaintText(HDC hDC);
	virtual void PaintBorder(HDC hDC);
private:
	int m_imgSrcX;
	int m_imgSrcY;
	int m_imgSrcR;
	int m_imgSrcG;
	int m_imgSrcB;
	RECT m_WndDesktopRect;
};


#endif // !MAGNIFIERUI_H_