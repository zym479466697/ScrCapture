#include "stdafx.h"
#include "UICircleProgress.h"


namespace DuiLib
{
	CCircleProgressUI::CCircleProgressUI() : m_nMin(0), m_nMax(100), m_nValue(0)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		SetFixedHeight(12);
		m_nInnerRadius = 0;
		m_nOuterRadius = this->GetWidth();
		m_nProColor = 0;
		m_nProBgkColor = 0;
	}

	LPCTSTR CCircleProgressUI::GetClass() const
	{
		return DUI_CTR_CIRCLEPROGRESS;
	}

	LPVOID CCircleProgressUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_CIRCLEPROGRESS) == 0 ) return static_cast<CCircleProgressUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	int CCircleProgressUI::GetMinValue() const
	{
		return m_nMin;
	}

	void CCircleProgressUI::SetMinValue(int nMin)
	{
		m_nMin = nMin;
		Invalidate();
	}

	int CCircleProgressUI::GetMaxValue() const
	{
		return m_nMax;
	}

	void CCircleProgressUI::SetMaxValue(int nMax)
	{
		m_nMax = nMax;
		Invalidate();
	}

	int CCircleProgressUI::GetValue() const
	{
		return m_nValue;
	}

	int CCircleProgressUI::GetInnerRadius() const
	{
		return m_nInnerRadius;
	}

	void CCircleProgressUI::SetInnerRadius(int nInnerRadius)
	{
		m_nInnerRadius = nInnerRadius;
		Invalidate();
	}

	int CCircleProgressUI::GetOuterRadius() const
	{
		return m_nOuterRadius;
	}

	void CCircleProgressUI::SetOuterRadius(int nOutRadius)
	{
		m_nOuterRadius = nOutRadius;
		Invalidate();
	}

	int CCircleProgressUI::GetProgressColor() const
	{
		return m_nProColor;
	}

	void CCircleProgressUI::SetProgressColor(int nProColor)
	{
		m_nProColor = nProColor;
		Invalidate();
	}

	int CCircleProgressUI::GetProgressBgkColor() const
	{
		return m_nProBgkColor;
	}

	void CCircleProgressUI::SetProgressBgkColor(int nProBgkColor)
	{
		m_nProBgkColor = nProBgkColor;
		Invalidate();
	}

	void CCircleProgressUI::SetValue(int nValue)
	{
		m_nValue = nValue;
		if (m_nValue > m_nMax) m_nValue = m_nMax;
		if (m_nValue < m_nMin) m_nValue = m_nMin;
		Invalidate();
	}
	
	void CCircleProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("min")) == 0 ) SetMinValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("max")) == 0 ) SetMaxValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("max")) == 0 ) SetMaxValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("value")) == 0 ) SetValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("outerRadius")) == 0 ) SetOuterRadius(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("innerRadius")) == 0 ) SetInnerRadius(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("progressColor")) == 0 ) 
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetProgressColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("progressBgkColor")) == 0 ) 
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetProgressBgkColor(clrColor);
		}
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}


	void CCircleProgressUI::PaintBkColor(HDC hDC)
	{
		__super::PaintBkColor(hDC);
#ifdef _USE_GDIPLUS	
		Gdiplus::REAL nAngle = (m_nValue - m_nMin) * 360.0 / (m_nMax - m_nMin)*1.0;

		Graphics nGraphics(hDC);
		nGraphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);//¿¹¾â³Ý   
	
		Gdiplus::SolidBrush brushBgk(Gdiplus::Color(255, GetRValue(m_nProBgkColor), GetGValue(m_nProBgkColor), GetBValue(m_nProBgkColor)));
		Gdiplus::GraphicsPath pathBgk;
		Gdiplus::Rect outerRect(m_rcItem.left + (m_rcItem.right - m_rcItem.left)/2 - m_nOuterRadius, 
			m_rcItem.top + (m_rcItem.bottom - m_rcItem.top)/2 - m_nOuterRadius,
			2 * m_nOuterRadius, 
			2 * m_nOuterRadius);
		pathBgk.AddEllipse(outerRect);


		Gdiplus::Rect innerRect(m_rcItem.left + (m_rcItem.right - m_rcItem.left)/2 - m_nInnerRadius, 
			m_rcItem.top + (m_rcItem.bottom - m_rcItem.top)/2 - m_nInnerRadius,
			2 * m_nInnerRadius, 
			2 * m_nInnerRadius);

		if(m_nInnerRadius > 0)
		{
			pathBgk.AddEllipse(innerRect);
			nGraphics.FillPath(&brushBgk, &pathBgk);
		}

		if(nAngle > 0)
		{
			Gdiplus::SolidBrush brushPro(Gdiplus::Color(255, GetRValue(m_nProColor), GetGValue(m_nProColor), GetBValue(m_nProColor)));
			Gdiplus::GraphicsPath pathPro;
			pathPro.AddPie(outerRect, -90, nAngle);
			if(m_nInnerRadius > 0) pathPro.AddPie(innerRect, -90, nAngle);

			pathPro.SetFillMode(Gdiplus::GpFillMode::FillModeAlternate);
			nGraphics.FillPath(&brushPro, &pathPro);
		}
#endif
	}

}
