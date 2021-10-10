#pragma once
#include <Wt/WPaintedWidget.h>
#include <Wt/WPainter.h>
#include <Wt/Chart/WDataSeries.h>
#include <Wt/WRectArea.h>

using namespace std;

class WJPARAMRECT : public Wt::WPaintedWidget
{
	Wt::WBrush wBrush;
	Wt::WColor wcSelf;
	Wt::WLength wlMinWidth, wlMinHeight;
	Wt::WPen wPen;
	Wt::WRectF wRect;

public:
	WJPARAMRECT(Wt::WColor self) : Wt::WPaintedWidget(), wcSelf(self) { init(); }
	~WJPARAMRECT() {}
	void init();
	string getHexColour();

protected:
	virtual void paintEvent(Wt::WPaintDevice* paintDevice) override;
};

class WJTRASH : public Wt::WImage
{
	Wt::Signal<string> deleteSignal_;

public:
	WJTRASH(Wt::WLink linkTrash) : Wt::WImage(linkTrash) { init(); }
	~WJTRASH() {}
	Wt::Signal<string>& deleteSignal() { return deleteSignal_; }
	void init();

protected:
	void dropEvent(Wt::WDropEvent wde) override;
};

