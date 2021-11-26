#ifndef __DYNAMICLAYOUTUI_H__
#define __DYNAMICLAYOUTUI_H__
#pragma once

namespace DuiLib
{

	class DUILIB_API CDynamicLayoutUI : public CVerticalLayoutUI
	{
	public:
		CDynamicLayoutUI();
		~CDynamicLayoutUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void SetSelectIndex(int iIndex, bool bTriggerEvent = true);
		virtual void DoInit();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		bool AddTxt(LPCTSTR strTxt);
		DWORD GetLineCnt() { return m_iLineCnt; }
	protected:
		void AddLine();
		DWORD GetCurLineEndDistance();
	protected:
		DWORD m_txtColor;
		DWORD m_txtSelColor;
		DWORD m_iSpace;
		DWORD m_iFont;
		DWORD m_lineHeight;
		CHorizontalLayoutUI* m_pCurAddLayout;
		DWORD m_iCount;
		DWORD m_iLineCnt;
		bool m_bInit;
	};
}

#endif /*__DYNAMICLAYOUTUI_H__*/