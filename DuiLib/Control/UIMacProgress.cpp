#include "stdafx.h"
#include "UIMacProgress.h"

namespace DuiLib
{
	const float EPSINON = 0.000001f;
	bool IsFloatZero(float x)
	{
		if ((x >= - EPSINON) && (x <= EPSINON))
		{
			return true;
		}
		return false;
	}

	CMacProgressUI::CMacProgressUI() :m_nMin(0), m_nMax(100), m_nValue(0)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		SetFixedHeight(12);
		m_nInnerRadius = 0;
		m_nOuterRadius = this->GetWidth();
		m_nProColor = 0;
		m_nProBgkColor = 0;
	}

	LPCTSTR CMacProgressUI::GetClass() const
	{
		return DUI_CTR_MACPROGRESS;
	}

	LPVOID CMacProgressUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_MACPROGRESS) == 0 ) return static_cast<CMacProgressUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	int CMacProgressUI::GetMinValue() const
	{
		return m_nMin;
	}

	void CMacProgressUI::SetMinValue(int nMin)
	{
		m_nMin = nMin;
		Invalidate();
	}

	int CMacProgressUI::GetMaxValue() const
	{
		return m_nMax;
	}

	void CMacProgressUI::SetMaxValue(int nMax)
	{
		m_nMax = nMax;
		Invalidate();
	}

	int CMacProgressUI::GetValue() const
	{
		return m_nValue;
	}

	int CMacProgressUI::GetInnerRadius() const
	{
		return m_nInnerRadius;
	}

	void CMacProgressUI::SetInnerRadius(int nInnerRadius)
	{
		m_nInnerRadius = nInnerRadius;
		Invalidate();
	}

	int CMacProgressUI::GetOuterRadius() const
	{
		return m_nOuterRadius;
	}

	void CMacProgressUI::SetOuterRadius(int nOutRadius)
	{
		m_nOuterRadius = nOutRadius;
		Invalidate();
	}

	int CMacProgressUI::GetProgressColor() const
	{
		return m_nProColor;
	}

	void CMacProgressUI::SetProgressColor(int nProColor)
	{
		m_nProColor = nProColor;
		Invalidate();
	}

	int CMacProgressUI::GetProgressBgkColor() const
	{
		return m_nProBgkColor;
	}

	void CMacProgressUI::SetProgressBgkColor(int nProBgkColor)
	{
		m_nProBgkColor = nProBgkColor;
		Invalidate();
	}

	void CMacProgressUI::SetValue(int nValue)
	{
		m_nValue = nValue;
		if (m_nValue > m_nMax) m_nValue = m_nMax;
		if (m_nValue < m_nMin) m_nValue = m_nMin;
		Invalidate();
	}

	void CMacProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
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
		else CButtonUI::SetAttribute(pstrName, pstrValue);
	}


	void CMacProgressUI::PaintBkColor(HDC hDC)
	{
		__super::PaintBkColor(hDC);

#ifdef _USE_GDIPLUS
		Gdiplus::REAL nAngle = (m_nValue - m_nMin) * 360.0f / (m_nMax - m_nMin)*1.0f;
	
		Graphics nGraphics(hDC);
		nGraphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);//¿¹¾â³Ý   

		RECT rc = m_rcItem;
		Gdiplus::Rect outerRect((rc.left + rc.right)/2 - m_nOuterRadius, 
			(rc.top + rc.bottom)/2 - m_nOuterRadius,
			2 * m_nOuterRadius - 1, 
			2 * m_nOuterRadius - 1);

		Gdiplus::Pen penOuter(Gdiplus::Color(255, GetRValue(m_nProBgkColor), GetGValue(m_nProBgkColor), GetBValue(m_nProBgkColor)));
		nGraphics.DrawEllipse(&penOuter, outerRect);

		//if((m_uButtonState & UISTATE_HOT) == 0)
		{
			//Gdiplus::SolidBrush brushBgk(Gdiplus::Color(255, GetRValue(m_nProBgkColor), GetGValue(m_nProBgkColor), GetBValue(m_nProBgkColor)));
			//Gdiplus::GraphicsPath pathBgk;
			//pathBgk.AddEllipse(outerRect);
			Gdiplus::Rect innerRect((rc.left + rc.right)/2 - m_nInnerRadius, 
				(rc.top + rc.bottom)/2 - m_nInnerRadius,
				2 * m_nInnerRadius - 1, 
				2 * m_nInnerRadius - 1);

			//if(m_nInnerRadius > 0)
			//{
			//	pathBgk.AddEllipse(innerRect);
			//	nGraphics.FillPath(&brushBgk, &pathBgk);
			//}

			if(!IsFloatZero(nAngle))
			{
				Gdiplus::SolidBrush brushPro(Gdiplus::Color(255, GetRValue(m_nProColor), GetGValue(m_nProColor), GetBValue(m_nProColor)));
				Gdiplus::GraphicsPath pathPro;
				pathPro.AddPie(outerRect, -90, nAngle);
				if(m_nInnerRadius > 0) pathPro.AddPie(innerRect, -90, nAngle);

				pathPro.SetFillMode(Gdiplus::GpFillMode::FillModeAlternate);
				nGraphics.FillPath(&brushPro, &pathPro);
			}
		}
#endif
	}

}
