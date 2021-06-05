#pragma once
#include <string>
#include <vector>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPanel.h>
#include <Wt/WLink.h>
#include <Wt/WText.h>
#include <Wt/WPainter.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPaintDevice.h>
#include "jfunc.h"

using namespace std;

class WTFUNC : public Wt::WPaintedWidget
{
	JFUNC jf;
	
	const string defaultLength = "200.0";
	string proj_dir;
	const string sWidth, sHeight;
	Wt::WLength wlWidth, wlHeight;
	Wt::WPainterPath wpPath;

public:
	explicit WTFUNC(string width, string height) : Wt::WPaintedWidget(), sWidth(width), sHeight(height)
	{
		wlWidth = Wt::WLength(sWidth);
		wlHeight = Wt::WLength(sHeight);
		resize(wlWidth, wlHeight);
	}
	explicit WTFUNC() : Wt::WPaintedWidget()
	{
		wlWidth = Wt::WLength(defaultLength);
		wlHeight = Wt::WLength(defaultLength);
		resize(wlWidth, wlHeight);
	}
	~WTFUNC() {}
	void err(string);
	void init_proj_dir(string);
	void initSize(double w, double h);
	void drawMap(Wt::WPainterPath& wpp);
	void makeWPPath(vector<vector<int>>& frameTLBR, vector<vector<int>>& border, vector<double>& windowDim, Wt::WPainterPath& wpPath);

protected:
	void paintEvent(Wt::WPaintDevice* paintDevice);
};

