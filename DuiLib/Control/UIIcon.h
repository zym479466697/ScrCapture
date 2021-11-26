#ifndef __UIICON_H__
#define __UIICON_H__

#pragma once

namespace DuiLib
{
	class DUILIB_API CIconUI : public CControlUI
	{
	public:
		CIconUI(void);
		~CIconUI(void);
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintBkImage(HDC hDC);

		void SetIcon(HICON hIcon);
		void SetInset(RECT rc);
	private:
		HICON m_hIcon;
		RECT  m_rcInset;  
	};
}

#endif