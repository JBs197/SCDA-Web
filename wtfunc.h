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
#include <Wt/WPolygonArea.h>
#include "jfunc.h"

using namespace std;

class WTFUNC : public Wt::WPaintedWidget
{
	JFUNC jf;
	
	const string defaultLength = "200.0";
	int indexAreaSel = -1;
	string proj_dir;
	const string sWidth, sHeight;
	Wt::WLength wlWidth, wlHeight;
	vector<vector<Wt::WPointF>> areas;

public:
	WTFUNC() : Wt::WPaintedWidget()
	{
		wlWidth = Wt::WLength(defaultLength);
		wlHeight = Wt::WLength(defaultLength);
		resize(wlWidth, wlHeight);
	}
	~WTFUNC() {}
	void areaClicked(int index);
	void err(string);
	void init_proj_dir(string);
	void initSize(double w, double h);
	void drawMap(vector<vector<Wt::WPointF>>& areas);
	vector<Wt::WPointF> makeWPPath(vector<vector<int>>& frameTLBR, vector<vector<int>>& border, vector<double>& windowDimPosition);

protected:
	virtual void paintEvent(Wt::WPaintDevice* paintDevice) override;
};

