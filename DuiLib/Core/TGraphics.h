// TGraphics.h: interface for the CTGraphics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TGRAPHICS_H)
#define TGRAPHICS_H



class CTGraphics  
{
public:
	CTGraphics();
	virtual ~CTGraphics();

public:
	// CTGraphics class interface
	static void DrawLine(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color);
	static void DrawPolyline(HDC hDC, POINT* points, int numberPoints, COLORREF color);
	static void DrawPolygon(HDC hDC, POINT* points, int numberPoints, COLORREF color);
	static void DrawEllipse(HDC hDC, int x1, int y1, int x2, int y2, COLORREF color);
	static void DrawArc(HDC hDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
	static void DrawPie(HDC hDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
	static void DrawChord(HDC hDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
	static void DrawRoundRect(HDC hDC, int x1, int y1, int x2, int y2, int width, int height, COLORREF color);
	static void DrawBezier(HDC hDC, POINT* points, int numPoints, COLORREF color);

};

#endif // !defined(TGRAPHICS_H)
