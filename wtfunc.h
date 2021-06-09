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
extern mutex m_map;

class WTFUNC : public Wt::WPaintedWidget
{
	JFUNC jf;
	
	vector<string> areaNames;
	vector<string>& commWidget;
	const string defaultLength = "200.0";
	int indexAreaSel, indexAreaPrevious;
	unordered_map<string, int> mapAreas;  // Form "Region Name"->indexAreas.
	string nameAreaSel;
	string proj_dir;
	const string sWidth, sHeight;
	Wt::WLength wlWidth, wlHeight;
	vector<vector<Wt::WPointF>> areas;

public:
	WTFUNC(vector<string>& cW) : Wt::WPaintedWidget(), commWidget(cW)
	{
		wlWidth = Wt::WLength(defaultLength);
		wlHeight = Wt::WLength(defaultLength);
		resize(wlWidth, wlHeight);
	}
	~WTFUNC() {}
	void areaClicked(int index);
	void clearAreas();
	void err(string);
	void initAreas(vector<int>& indexOrder);
	void init_proj_dir(string);
	void initSize(double w, double h);
	void drawMap(vector<vector<Wt::WPointF>>& areas, vector<string>& areaNames);
	vector<vector<int>> getFrame(vector<Wt::WPointF>& path);
	vector<Wt::WPointF> makeWPPath(vector<vector<int>>& frameTLBR, vector<vector<int>>& border, vector<double>& windowDimPosition);
	void paintRegion(Wt::WPainter& painter, int index);
	void paintRegionAll(Wt::WPainter& painter, vector<int>& indexOrder);

protected:
	virtual void paintEvent(Wt::WPaintDevice* paintDevice) override;
};

