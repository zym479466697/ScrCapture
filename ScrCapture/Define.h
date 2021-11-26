#ifndef	DEFINE_H_
#define DEFINE_H_
#include <string>
#include <stdint.h>


enum ACTION_OP
{
	ACTION_OP_NONE,
	ACTION_OP_RECT,
	ACTION_OP_ELLIPSE,
	ACTION_OP_LINE,
	ACTION_OP_ARROW,
	ACTION_OP_SCRAWL,
	ACTION_OP_HIGHLIGHT,
	ACTION_OP_MOSAIC,
	ACTION_OP_TXT,
};


//¼¤»îÏûÏ¢
#define WM_MSG_COPY_ACTIVE			WM_USER + 1000

#include "../DuiLib/UIlib.h"
using namespace DuiLib;
#define  DUI_CTR_CANVAS                          (_T("Canvas"))
#define  DUI_CTR_CANVASCONTAINER                 (_T("CanvasContainer"))
#define  DUI_CTR_MAGNIFIER                 (_T("Magnifier"))
#define  DUI_CTR_RECTANGLE                 (_T("Rectangle"))
#endif