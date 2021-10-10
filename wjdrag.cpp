#include "wjdrag.h"

string WJPARAMRECT::getHexColour()
{
	string hexColour;
	int iColour = wcSelf.red();
	int quotient = iColour / 16;
	if (quotient < 10) { hexColour += to_string(quotient); }
	else { hexColour += (55 + quotient); }
	int remainder = iColour % 16;
	if (remainder < 10) { hexColour += to_string(remainder); }
	else { hexColour += (55 + remainder); }

	iColour = wcSelf.green();
	quotient = iColour / 16;
	if (quotient < 10) { hexColour += to_string(quotient); }
	else { hexColour += (55 + quotient); }
	remainder = iColour % 16;
	if (remainder < 10) { hexColour += to_string(remainder); }
	else { hexColour += (55 + remainder); }

	iColour = wcSelf.blue();
	quotient = iColour / 16;
	if (quotient < 10) { hexColour += to_string(quotient); }
	else { hexColour += (55 + quotient); }
	remainder = iColour % 16;
	if (remainder < 10) { hexColour += to_string(remainder); }
	else { hexColour += (55 + remainder); }

	iColour = wcSelf.alpha();
	quotient = iColour / 16;
	if (quotient < 10) { hexColour += to_string(quotient); }
	else { hexColour += (55 + quotient); }
	remainder = iColour % 16;
	if (remainder < 10) { hexColour += to_string(remainder); }
	else { hexColour += (55 + remainder); }

	return hexColour;
}
void WJPARAMRECT::init()
{
	wlMinWidth = Wt::WLength(40.0);
	wlMinHeight = Wt::WLength(25.0);
	resize(wlMinWidth, wlMinHeight);

	wPen = Wt::WPen(Wt::PenStyle::SolidLine);
	wPen.setWidth(4.0);
	wBrush = Wt::WBrush(Wt::BrushStyle::Solid);
	wBrush.setColor(wcSelf);

	wRect = Wt::WRectF(0.0, 0.0, 40.0, 25.0);
	auto area = make_unique<Wt::WRectArea>(wRect);
	addArea(move(area));

	setDraggable("paramRect");

	update();
}
void WJPARAMRECT::paintEvent(Wt::WPaintDevice* paintDevice)
{
	Wt::WPainter painter(paintDevice);
	painter.setPen(wPen);
	painter.setBrush(wBrush);
	painter.drawRect(wRect);
}

void WJTRASH::dropEvent(Wt::WDropEvent wde)
{
	WJPARAMRECT* wjpr = (WJPARAMRECT*)wde.source();
	string sID = wjpr->id();
	deleteSignal_.emit(sID);
}
void WJTRASH::init()
{
	Wt::WString wsStyle = "jicon";
	acceptDrops("paramRect", wsStyle);
}
