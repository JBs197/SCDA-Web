#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPanel.h>
#include <Wt/WLink.h>
#include <Wt/WText.h>
#include <Wt/WPainter.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPaintDevice.h>
#include <Wt/WPolygonArea.h>
#include <Wt/WRectArea.h>
#include <Wt/WFlags.h>
#include "jfunc.h"

using namespace std;
extern mutex m_map;

class WTPAINT : public Wt::WPaintedWidget
{
	vector<vector<int>> areaColour, keyColour;  // Wt prefers ints for some reason...
	vector<int> extraColour;
	vector<double> areaData, areaDataKids, legendBoxDim = { 250.0, 150.0 };
	vector<string> areaNames, dataName;
	vector<vector<Wt::WPointF>> areas;
	const double colourDimPercent = 0.7, barThickness = 20.0;
	vector<string>& commWidget;
	const string defaultLength = "200.0";
	const double defaultParentExclusionThreshold = 0.5;  // If no child's data passes this percentage of the parent's data, exclude parent from the map's legend/colour.
	int indexAreaSel, indexAreaPrevious;
	JFUNC jf;
	unordered_map<string, int> mapAreas;  // Form "Region Name"->indexAreas.
	string nameAreaSel;
	const int numColourBands = 5;
	string proj_dir;
	const string sWidth, sHeight;
	Wt::WLength wlWidth, wlHeight;	

public:
	WTPAINT(vector<string>& cW) : Wt::WPaintedWidget(), commWidget(cW)
	{
		wlWidth = Wt::WLength(defaultLength);
		wlHeight = Wt::WLength(defaultLength);
		resize(wlWidth, wlHeight);
	}
	~WTPAINT() {}
	void areaClicked(int index);
	void clearAreas();
	vector<string> delinearizeTitle(string& linearTitle);
	void err(string);
	void initAreas(vector<int>& indexOrder);
	void initColour();
	void init_proj_dir(string);
	void initSize(double w, double h);
	void drawMap(vector<vector<Wt::WPointF>>& areas, vector<string>& areaNames, vector<double>& regionData);
	vector<vector<int>> getFrame(vector<Wt::WPointF>& path);
	vector<int> getScaleValues(int numTicks);
	vector<Wt::WPointF> makeWPPath(vector<vector<int>>& frameTLBR, vector<vector<int>>& border, vector<double>& windowDimPosition);
	void paintLegendBarV(Wt::WPainter& painter);
	void paintLegendBox(Wt::WPainter& painter, int index);
	void paintRegion(Wt::WPainter& painter, int index, double percent);
	void paintRegionAll(Wt::WPainter& painter, vector<int> indexOrder, double percent);
	void setWColour(Wt::WColor& wColour, vector<int> rgb, double percent);
	bool testExcludeParent();
	void updateAreaColour();

protected:
	virtual void paintEvent(Wt::WPaintDevice* paintDevice) override;
};

