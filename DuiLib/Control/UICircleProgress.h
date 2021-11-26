#ifndef __UICIRCLEPROGRESS_H__
#define __UICIRCLEPROGRESS_H__

#pragma once

namespace DuiLib
{
	class DUILIB_API CCircleProgressUI : public CLabelUI
	{
	public:
		CCircleProgressUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		int GetMinValue() const;
		void SetMinValue(int nMin);
		int GetMaxValue() const;
		void SetMaxValue(int nMax);
		int GetValue() const;
		void SetValue(int nValue);

		int GetInnerRadius() const;
		void SetInnerRadius(int nInnerRadius);
		int GetOuterRadius() const;
		void SetOuterRadius(int nOutRadius);
		int GetProgressColor() const;
		void SetProgressColor(int nProColor);
		int GetProgressBgkColor() const;
		void SetProgressBgkColor(int nProBgkColor);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintBkColor(HDC hDC);

	protected:
		int m_nMax;
		int m_nMin;
		int m_nValue;
		int m_nInnerRadius;
		int m_nOuterRadius;
		int m_nProColor;
		int m_nProBgkColor;

		TDrawInfo m_diFore;
	};

} // namespace DuiLib

#endif // __UIPROGRESS_H__
