#include "stdafx.h"
#include "DynamicLayoutUI.h"

namespace DuiLib
{
	CDynamicLayoutUI::CDynamicLayoutUI() :m_iSpace(20), m_iFont(-1), 
		m_lineHeight(24), m_iCount(0), m_iLineCnt(0),
		m_bInit(false), m_txtColor(0xff999999), m_txtSelColor(0xff5c7ce9)
	{
		m_pCurAddLayout = NULL;
	}


	CDynamicLayoutUI::~CDynamicLayoutUI()
	{

	}

	LPCTSTR CDynamicLayoutUI::GetClass() const
	{
		return DUI_CTR_DYNAMICLAYOUT;
	}

	LPVOID CDynamicLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_DYNAMICLAYOUT) == 0) return static_cast<CDynamicLayoutUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

	void CDynamicLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("space")) == 0)
		{
			LPTSTR pstr = NULL;
			m_iSpace = _tcstoul(pstrValue, &pstr, 10);
		}
		else if (_tcscmp(pstrName, _T("font")) == 0)
		{
			LPTSTR pstr = NULL;
			m_iFont = _tcstoul(pstrValue, &pstr, 10);
		}
		else if (_tcscmp(pstrName, _T("lineheight")) == 0)
		{
			LPTSTR pstr = NULL;
			m_lineHeight = _tcstoul(pstrValue, &pstr, 10);
		}
		else if (_tcscmp(pstrName, _T("txtcolor")) == 0)
		{
			while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			m_txtColor = _tcstoul(pstrValue, &pstr, 16);
		}
		else if (_tcscmp(pstrName, _T("txtselcolor")) == 0)
		{
			while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			m_txtSelColor = _tcstoul(pstrValue, &pstr, 16);
		}
		else
		{
			CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
		}
	}


	void CDynamicLayoutUI::DoInit()
	{
		if (!m_bInit)
		{
			m_bInit = true;
			AddLine();
		}
	}

	void CDynamicLayoutUI::SetSelectIndex(int iIndex, bool bTriggerEvent/* = true*/)
	{
		CDuiString strName;
		strName.Format(_T("%s_item_%d"), this->GetName().GetData(), iIndex);
		COptionUI *pBtn = (COptionUI *)this->FindSubControl(strName);
		if (pBtn)
		{
			pBtn->Selected(true, false);
		}
	}

	void CDynamicLayoutUI::AddLine()
	{
		m_pCurAddLayout = new CHorizontalLayoutUI;
		CDuiString strName;
		strName.Format(_T("%s_line_%d"), this->GetName().GetData(), m_iLineCnt);
		m_pCurAddLayout->SetName(strName);
		m_pCurAddLayout->SetFloat(false);
		m_pCurAddLayout->SetMaxHeight(m_lineHeight);
		//m_pCurAddLayout->SetBkColor(0xffff0000);
		m_pCurAddLayout->SetMouseEnabled(false);
		this->Add(m_pCurAddLayout);
		m_iLineCnt++;
		this->SetFixedHeight(m_iLineCnt * m_lineHeight);
	}


	bool CDynamicLayoutUI::AddTxt(LPCTSTR strTxt)
	{
		if (!m_pCurAddLayout) return false;
		COptionUI *pBtn = new COptionUI;
		CDuiString strName;
		strName.Format(_T("%s_item_%d"), this->GetName().GetData(), m_iCount);
		pBtn->SetFloat(true);
		pBtn->SetName(strName);
		pBtn->SetText(strTxt);
		pBtn->SetBkColor(0x0000ff00);
		pBtn->SetSelectedBkColor(0x00ff0000);
		pBtn->SetTextColor(m_txtColor);
		pBtn->SetSelectedTextColor(m_txtSelColor);

		CDuiString strGroupName;
		strGroupName.Format(_T("%s_group"), this->GetName().GetData());
		pBtn->SetGroup(strGroupName.GetData());

		SIZE size = CRenderEngine::GetTextSize(m_pManager->GetPaintDC(), m_pManager, strTxt, m_iFont, DT_VCENTER | DT_LEFT);
		DWORD maxWidth = GetFixedWidth();
		DWORD distance = GetCurLineEndDistance();
		if (distance == 0)
		{

		}
		else if (distance + m_iSpace + size.cx > maxWidth)
		{
			AddLine();
			distance = GetCurLineEndDistance();
		}
		else
		{

		}
		if (distance != 0) distance += m_iSpace;

		SIZE leftTop = { distance, 0 };
		pBtn->SetFixedXY(leftTop);
		pBtn->SetFixedWidth(size.cx);
		pBtn->SetFixedHeight(m_lineHeight);
		m_iCount++;
		return m_pCurAddLayout->Add(pBtn);
	}


	DWORD CDynamicLayoutUI::GetCurLineEndDistance()
	{
		if (m_pCurAddLayout == NULL) return 0;
		int cnt = m_pCurAddLayout->GetCount();
		if (cnt == 0) return 0;

		CControlUI* pControl = m_pCurAddLayout->GetItemAt(cnt - 1);
		SIZE sz = pControl->GetFixedXY();
		int width = sz.cx + pControl->GetFixedWidth();
		return width;
	}

}