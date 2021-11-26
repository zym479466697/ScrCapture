#include "DrawGraph.h"
const double PI = 3.1415926;

void GraphRectangle::ResetGraphPos(POINT ptStart, POINT ptEnd)
{
	RectX rcGraph(ptStart, ptEnd);
	rcGraph.ResetStartEnd();
	if (rcGraph.OutOfBoundary(m_canvasRc)) {
		rcGraph.ResetBoundary(m_canvasRc);
	}
	m_itemRc = rcGraph;
}

void GraphRectangle::DrawGraph(HDC hDC)
{
	Graphics graphics(hDC);
	Pen pen(m_penColor);
	pen.SetWidth(m_penWidth);
	graphics.DrawRectangle(&pen, m_itemRc.leftX, m_itemRc.topX, m_itemRc.rightX - m_itemRc.leftX, m_itemRc.bottomX - m_itemRc.topX);
	//CRenderEngine::DrawRect(hDC, m_itemRc.ToRECT(), m_penWidth, m_penColor, PS_SOLID);
}

bool GraphRectangle::PtInRect(const POINT& pt)
{
	const RECT rc = m_itemRc.ToRECT();
	if (::PtInRect(&rc, pt)) {
		RECT rcInset = m_itemRc.DeflateRect(-m_penWidth, -m_penWidth).ToRECT();
		if (!::PtInRect(&rcInset, pt)) {
			return true;
		}
	}
	return false;
}

//==========GraphEllipse============
void GraphEllipse::ResetGraphPos(POINT ptStart, POINT ptEnd)
{
	RectX rcGraph(ptStart, ptEnd);
	rcGraph.ResetStartEnd();
	if (rcGraph.OutOfBoundary(m_canvasRc)) {
		rcGraph.ResetBoundary(m_canvasRc);
	}
	m_itemRc = rcGraph;
}

void GraphEllipse::DrawGraph(HDC hDC)
{
	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	Pen pen(m_penColor, m_penWidth);
	graphics.DrawEllipse(&pen, m_itemRc.leftX, m_itemRc.topX, m_itemRc.rightX - m_itemRc.leftX, m_itemRc.bottomX - m_itemRc.topX);
	//CRenderEngine::DrawEllipse(hDC, m_itemRc.ToRECT(), m_penWidth, m_penColor, PS_SOLID);
}

bool GraphEllipse::PtInRect(const POINT& pt)
{

	return false;
}


//==========GraphLine============
void GraphLine::ResetGraphPos(POINT ptStart, POINT ptEnd)
{
	RectX rcGraph(ptStart, ptEnd);
	//rcGraph.ResetStartEnd();
	if (rcGraph.OutOfBoundary(m_canvasRc)) {
		rcGraph.ResetBoundary(m_canvasRc);
	}
	m_itemRc = rcGraph;
}

void GraphLine::DrawGraph(HDC hDC)
{
	Graphics graphics(hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	Pen pen(m_penColor, m_penWidth);
	graphics.DrawLine(&pen, m_itemRc.leftX, m_itemRc.topX, m_itemRc.rightX, m_itemRc.bottomX);
	//CRenderEngine::DrawLine(hDC, m_itemRc.ToRECT(), m_penWidth, m_penColor, PS_SOLID);
}

bool GraphLine::PtInRect(const POINT& pt)
{

	return false;
}


//==========GraphArrow============
void GraphArrow::ResetGraphPos(POINT ptStart, POINT ptEnd)
{
	RectX rcGraph(ptStart, ptEnd);
	//rcGraph.ResetStartEnd();
	if (rcGraph.OutOfBoundary(m_canvasRc)) {
		rcGraph.ResetBoundary(m_canvasRc);
	}
	m_itemRc = rcGraph;
}

void GraphArrow::DrawGraph(HDC hDC)
{
	POINT ptStart, ptEnd;
	ptStart.x = m_itemRc.leftX;
	ptStart.y = m_itemRc.topX;
	ptEnd.x = m_itemRc.rightX;
	ptEnd.y = m_itemRc.bottomX;
	CRectEx rcRgn = m_canvasRc;

	ptEnd.x = ptEnd.x < rcRgn.leftX ? rcRgn.leftX : ptEnd.x;
	ptEnd.x = ptEnd.x > rcRgn.rightX ? rcRgn.rightX : ptEnd.x;
	ptEnd.y = ptEnd.y < rcRgn.topX ? rcRgn.topX : ptEnd.y;
	ptEnd.y = ptEnd.y > rcRgn.bottomX ? rcRgn.bottomX : ptEnd.y;

	m_itemRc.leftX = ptStart.x;
	m_itemRc.topX = ptStart.y;
	m_itemRc.rightX = ptEnd.x;
	m_itemRc.bottomX = ptEnd.y;

	//简单箭头
	//Graphics graphics(hDC);
	//graphics.SetSmoothingMode(SmoothingModeHighQuality);
	//Pen pen(m_penColor, m_penWidth);
	//AdjustableArrowCap cap(4, 4, true);
	//pen.SetCustomEndCap(&cap);
	//graphics.DrawLine(&pen, m_itemRc.leftX, m_itemRc.topX, m_itemRc.rightX, m_itemRc.bottomX);

	LineArrow(hDC, ptStart, ptEnd, 30, 15, 40, RGB(m_penColor.GetR(), m_penColor.GetG(), m_penColor.GetB()), m_penWidth);
}

void GraphArrow::LineArrow(HDC pDC, POINT p1, POINT p2, double theta, double alpha, int length, COLORREF clr, int size)
{
	theta = 3.1415926*theta / 180;//转换为弧度
	alpha = 3.1415926*alpha / 180;
	double Px, Py, P1x, P1y, P2x, P2y;
	double Pax, Pay, Pbx, Pby;
	//以P2为原点得到向量P2P1（P）
	Px = p1.x - p2.x;
	Py = p1.y - p2.y;
	//向量P旋转theta角得到向量P1
	P1x = Px * cos(theta) - Py * sin(theta);
	P1y = Px * sin(theta) + Py * cos(theta);
	//向量P旋转alpha角得到向量Pa
	Pax = Px * cos(alpha) - Py * sin(alpha);
	Pay = Px * sin(alpha) + Py * cos(alpha);
	//向量P旋转-theta角得到向量P2
	P2x = Px * cos(-theta) - Py * sin(-theta);
	P2y = Px * sin(-theta) + Py * cos(-theta);
	//向量P旋转-alpha角得到向量Pb
	Pbx = Px * cos(-alpha) - Py * sin(-alpha);
	Pby = Px * sin(-alpha) + Py * cos(-alpha);
	//伸缩向量至制定长度
	int len;
	if (size == 1)
	{
		length = (int)(length / 2);
		len = (int)(length - 1.5);
	}
	else if (size == 2)
	{
		length = length * 2 / 3 + 2;
		len = length - 2;
	}
	else
	{
		len = length - 3;
	}

	double x1, x2;
	x1 = sqrt(P1x*P1x + P1y * P1y);
	P1x = P1x * length / x1;
	P1y = P1y * length / x1;
	x2 = sqrt(P2x*P2x + P2y * P2y);
	P2x = P2x * length / x2;
	P2y = P2y * length / x2;

	double xa, xb;
	xa = sqrt(Pax*Pax + Pay * Pay);
	Pax = Pax * (len) / x1;
	Pay = Pay * (len) / x1;
	xb = sqrt(Pbx*Pbx + Pby * Pby);
	Pbx = Pbx * (len) / x2;
	Pby = Pby * (len) / x2;
	//平移变量到直线的末端
	P1x = P1x + p2.x;
	P1y = P1y + p2.y;
	P2x = P2x + p2.x;
	P2y = P2y + p2.y;

	Pax = Pax + p2.x;
	Pay = Pay + p2.y;
	Pbx = Pbx + p2.x;
	Pby = Pby + p2.y;

	Gdiplus::Graphics graphics(pDC);
	SolidBrush GdiBrush(Gdiplus::Color(255, GetRValue(clr), GetGValue(clr), GetBValue(clr)));
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Point   pts[6];
	pts[0].X = (INT)p1.x;
	pts[0].Y = (INT)p1.y;
	pts[1].X = (INT)Pax;
	pts[1].Y = (INT)Pay;
	pts[2].X = (INT)P1x;
	pts[2].Y = (INT)P1y;
	pts[3].X = (INT)p2.x;
	pts[3].Y = (INT)p2.y;
	pts[4].X = (INT)P2x;
	pts[4].Y = (INT)P2y;
	pts[5].X = (INT)Pbx;
	pts[5].Y = (INT)Pby;
	graphics.FillPolygon(&GdiBrush, pts, 6);
}

bool GraphArrow::PtInRect(const POINT& pt)
{

	return false;
}


//==========GraphScrawl============
void GraphScrawl::ResetGraphPos(POINT ptStart, POINT ptEnd)
{
	//RectX rcGraph(ptStart, ptEnd);
	////rcGraph.ResetStartEnd();
	//if (rcGraph.OutOfBoundary(m_canvasRc)) {
	//	rcGraph.ResetBoundary(m_canvasRc);
	//}
	//m_itemRc = rcGraph;
	ptStart.x = ptStart.x < m_canvasRc.leftX ? m_canvasRc.leftX : ptStart.x;
	ptStart.x = ptStart.x > m_canvasRc.rightX ? m_canvasRc.rightX : ptStart.x;
	ptStart.y = ptStart.y < m_canvasRc.topX ? m_canvasRc.topX : ptStart.y;
	ptStart.y = ptStart.y > m_canvasRc.bottomX ? m_canvasRc.bottomX : ptStart.y;

	ptEnd.x = ptEnd.x < m_canvasRc.leftX ? m_canvasRc.leftX : ptEnd.x;
	ptEnd.x = ptEnd.x > m_canvasRc.rightX ? m_canvasRc.rightX : ptEnd.x;
	ptEnd.y = ptEnd.y < m_canvasRc.topX ? m_canvasRc.topX : ptEnd.y;
	ptEnd.y = ptEnd.y > m_canvasRc.bottomX ? m_canvasRc.bottomX : ptEnd.y;

	if (m_pointList.empty()) {
		m_pointList.push_back(ptStart);
		m_pointList.push_back(ptEnd);
	}
	else {
		POINT lastPoint = m_pointList[m_pointList.size() - 1];
		//如果和最后一个不一致
		if (lastPoint.x != ptEnd.x || lastPoint.y != ptEnd.y) {
			m_pointList.push_back(ptEnd);
		}
	}
}

void GraphScrawl::DrawGraph(HDC hDC)
{
	//Graphics graphics(hDC);
	//graphics.SetSmoothingMode(SmoothingModeNone);
	//Pen pen(m_penColor, m_penWidth);
	//for (int idx = 0; idx < m_pointList.size() - 1; idx++) {
	//	graphics.DrawLine(&pen, m_pointList[idx].x, m_pointList[idx].y, m_pointList[idx + 1].x, m_pointList[idx + 1].y);
	//}
	
	//不用gdi 效果会平滑点
	//for (int idx = 0; idx < m_pointList.size() - 1; idx++) {
	//	RECT rc = { m_pointList[idx].x, m_pointList[idx].y, m_pointList[idx + 1].x, m_pointList[idx + 1].y };
	//	CRenderEngine::DrawLine(hDC, rc, m_penWidth, RGB(m_penColor.GetB(), m_penColor.GetG(), m_penColor.GetR()), PS_SOLID);
	//}

	Graphics graphics(hDC);
	Pen pen(m_penColor, m_penWidth);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gdiplus::GraphicsPath scrawlPath;
	for (int idx = 0; idx < m_pointList.size() - 1; idx++) {
		scrawlPath.AddLine(m_pointList[idx].x, m_pointList[idx].y, m_pointList[idx + 1].x, m_pointList[idx + 1].y);
	}
	graphics.DrawPath(&pen, &scrawlPath);
}

bool GraphScrawl::PtInRect(const POINT& pt)
{
	const RECT rc = m_itemRc.ToRECT();
	if (::PtInRect(&rc, pt)) {
		RECT rcInset = m_itemRc.DeflateRect(-m_penWidth, -m_penWidth).ToRECT();
		if (!::PtInRect(&rcInset, pt)) {
			return true;
		}
	}
	return false;
}



//==========GraphHighlight============
void GraphHighlight::ResetGraphPos(POINT ptStart, POINT ptEnd)
{
	//RectX rcGraph(ptStart, ptEnd);
	////rcGraph.ResetStartEnd();
	//if (rcGraph.OutOfBoundary(m_canvasRc)) {
	//	rcGraph.ResetBoundary(m_canvasRc);
	//}
	//m_itemRc = rcGraph;
	ptStart.x = ptStart.x < m_canvasRc.leftX ? m_canvasRc.leftX : ptStart.x;
	ptStart.x = ptStart.x > m_canvasRc.rightX ? m_canvasRc.rightX : ptStart.x;
	ptStart.y = ptStart.y < m_canvasRc.topX ? m_canvasRc.topX : ptStart.y;
	ptStart.y = ptStart.y > m_canvasRc.bottomX ? m_canvasRc.bottomX : ptStart.y;

	ptEnd.x = ptEnd.x < m_canvasRc.leftX ? m_canvasRc.leftX : ptEnd.x;
	ptEnd.x = ptEnd.x > m_canvasRc.rightX ? m_canvasRc.rightX : ptEnd.x;
	ptEnd.y = ptEnd.y < m_canvasRc.topX ? m_canvasRc.topX : ptEnd.y;
	ptEnd.y = ptEnd.y > m_canvasRc.bottomX ? m_canvasRc.bottomX : ptEnd.y;

	if (m_pointList.empty()) {
		m_pointList.push_back(ptStart);
		m_pointList.push_back(ptEnd);
	}
	else {
		POINT lastPoint = m_pointList[m_pointList.size() - 1];
		//如果和最后一个不一致
		if (lastPoint.x != ptEnd.x || lastPoint.y != ptEnd.y) {
			m_pointList.push_back(ptEnd);
		}
	}
}

void GraphHighlight::DrawGraph(HDC hDC)
{
	////HPEN hPen = CreatePen(PS_SOLID, m_penWidth * 4, RGB(m_penColor.GetB(), m_penColor.GetG(), m_penColor.GetR()));
	//HPEN hPen = CreatePen(PS_SOLID, m_penWidth * 4, RGB(m_penColor.GetR(), m_penColor.GetG(), m_penColor.GetB()));
	//HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);

	//int hOldROP = SetROP2(hDC, R2_MASKPEN); //荧光笔
	//for (int idx = 0; idx < m_pointList.size() - 1; idx++) {
	//	//RECT rc = { m_pointList[idx].x, m_pointList[idx].y, m_pointList[idx + 1].x, m_pointList[idx + 1].y };
	//	//CRenderEngine::DrawLine(hDC, rc, m_penWidth, RGB(m_penColor.GetB(), m_penColor.GetG(), m_penColor.GetR()), PS_SOLID);
	//	MoveToEx(hDC, m_pointList[idx].x, m_pointList[idx].y, NULL);
	//	LineTo(hDC, m_pointList[idx + 1].x, m_pointList[idx + 1].y);
	//}
	//::SetROP2(hDC, hOldROP);
	//::SelectObject(hDC, hOldPen);
	//::DeleteObject(hPen);

	Graphics graphics(hDC);
	Color cr(125, m_penColor.GetR(), m_penColor.GetG(), m_penColor.GetB());
	Pen pen(cr, m_penWidth);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gdiplus::GraphicsPath highLinePath;
	for (int idx = 0; idx < m_pointList.size() - 1; idx++) {
		highLinePath.AddLine(m_pointList[idx].x, m_pointList[idx].y, m_pointList[idx + 1].x, m_pointList[idx + 1].y);
	}
	//graphics.set()
	graphics.DrawPath(&pen, &highLinePath);
}

bool GraphHighlight::PtInRect(const POINT& pt)
{
	return false;
}


//==========GraphMosaic============
Gdiplus::Bitmap* GraphMosaic::s_maskBitmap = NULL;
void GraphMosaic::ResetGraphPos(POINT ptStart, POINT ptEnd)
{
	//RectX rcGraph(ptStart, ptEnd);
	////rcGraph.ResetStartEnd();
	//if (rcGraph.OutOfBoundary(m_canvasRc)) {
	//	rcGraph.ResetBoundary(m_canvasRc);
	//}
	//m_itemRc = rcGraph;
	ptStart.x = ptStart.x < m_canvasRc.leftX ? m_canvasRc.leftX : ptStart.x;
	ptStart.x = ptStart.x > m_canvasRc.rightX ? m_canvasRc.rightX : ptStart.x;
	ptStart.y = ptStart.y < m_canvasRc.topX ? m_canvasRc.topX : ptStart.y;
	ptStart.y = ptStart.y > m_canvasRc.bottomX ? m_canvasRc.bottomX : ptStart.y;

	ptEnd.x = ptEnd.x < m_canvasRc.leftX ? m_canvasRc.leftX : ptEnd.x;
	ptEnd.x = ptEnd.x > m_canvasRc.rightX ? m_canvasRc.rightX : ptEnd.x;
	ptEnd.y = ptEnd.y < m_canvasRc.topX ? m_canvasRc.topX : ptEnd.y;
	ptEnd.y = ptEnd.y > m_canvasRc.bottomX ? m_canvasRc.bottomX : ptEnd.y;

	if (m_pointList.empty()) {
		m_pointList.push_back(ptStart);
		m_pointList.push_back(ptEnd);
	}
	else {
		POINT lastPoint = m_pointList[m_pointList.size() - 1];
		//如果和最后一个不一致
		if (lastPoint.x != ptEnd.x || lastPoint.y != ptEnd.y) {
			m_pointList.push_back(ptEnd);
		}
	}
}

void GraphMosaic::SetMaskBitMap(Gdiplus::Bitmap* bitMap)
{
	if (s_maskBitmap) {
		delete s_maskBitmap;
		s_maskBitmap = NULL;
	}
	s_maskBitmap = bitMap;
}

void GraphMosaic::DrawGraph(HDC hDC)
{
	Gdiplus::Graphics graphics(hDC);
	Gdiplus::TextureBrush tBrush((Gdiplus::Image*)s_maskBitmap);
	Pen pen(&tBrush, m_penWidth * 3);
	//graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	for (int idx = 0; idx < m_pointList.size() - 1; idx++) {
		graphics.DrawLine(&pen, m_pointList[idx].x, m_pointList[idx].y, m_pointList[idx + 1].x, m_pointList[idx + 1].y);
	}
}

bool GraphMosaic::PtInRect(const POINT& pt)
{
	const RECT rc = m_itemRc.ToRECT();
	if (::PtInRect(&rc, pt)) {
		RECT rcInset = m_itemRc.DeflateRect(-m_penWidth, -m_penWidth).ToRECT();
		if (!::PtInRect(&rcInset, pt)) {
			return true;
		}
	}
	return false;
}

//==========GraphFactory============
IGraph* GraphFactory::CreateGraph(ACTION_OP op)
{
	IGraph* pGraph = NULL;
	switch (op)
	{
	default:
		break;
	case ACTION_OP_RECT:
		pGraph = new GraphRectangle();
		break;
	case ACTION_OP_ELLIPSE:
		pGraph = new GraphEllipse();
		break;
	case ACTION_OP_LINE:
		pGraph = new GraphLine();
		break;
	case ACTION_OP_ARROW:
		pGraph = new GraphArrow();
		break;
	case ACTION_OP_SCRAWL:
		pGraph = new GraphScrawl();
		break;
	case ACTION_OP_HIGHLIGHT:
		pGraph = new GraphHighlight();
		break;
	case ACTION_OP_MOSAIC:
		pGraph = new GraphMosaic();
		break;
	}
	return pGraph;
}
