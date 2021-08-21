#pragma once
#include <Wt/WEnvironment.h>
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

class WTPAINT : public Wt::WPaintedWidget
{
	vector<Wt::WPolygonArea*> area;
	vector<vector<int>> areaColour, keyColour;  // Wt prefers ints for some reason...
	vector<double> areaData, areaDataChildren, legendBoxDim = { 250.0, 150.0 };
	vector<string> areaName, dataName;
	vector<vector<Wt::WPointF>> border;  // Form [region index][border point index].
	const double colourDimPercent = 0.7, barThickness = 20.0;
	const string defaultLength = "200.0";
	const double defaultParentExclusionThreshold = 0.5;  // If no child's data passes this percentage of the parent's data, exclude parent from the map's legend/colour.
	double dHeight = -1.0, dWidth = -1.0;  // Unit of pixels.
	vector<int> extraColour;
	int indexAreaSel, indexAreaPrevious;
	JFUNC jf;
	unordered_map<string, int> mapArea;  // Form "Region Name"->indexArea.
	string nameAreaSel;
	const int numColourBands = 5;
	double widgetPPKM = -1.0;

public:
	WTPAINT() {
		setLayoutSizeAware(true);
		resize(700, 700);
		setDimensions(700, 700);
	}
	~WTPAINT() override {}

	void areaClicked(int index);
	void clearAreas();
	vector<string> delinearizeTitle(string& linearTitle);
	vector<Wt::WPointF> displaceChildToParent(vector<vector<double>>& vvdBorder, vector<double>& childTL, vector<double> dispTL);
	void displaceParentToWidget(vector<vector<vector<double>>>& vvvdBorder, vector<vector<double>>& parentFrameKM);
	void drawMap(vector<vector<vector<double>>>& vvvdBorder, vector<string>& vsRegion, vector<double>& vdData);
	void initColour();
	vector<double> getChildTL(vector<vector<double>>& vpfBorder, vector<vector<double>>& childFrameKM, vector<vector<double>>& parentFrameKM);
	vector<vector<int>> getFrame(vector<Wt::WPointF>& path);
	vector<int> getScaleValues(int numTicks);
	void makeAreas();
	void paintLegendBarV(Wt::WPainter& painter);
	void paintLegendBox(Wt::WPainter& painter, int index);
	void paintRegion(Wt::WPainter& painter, int index, double percent);
	void paintRegionAll(Wt::WPainter& painter);
	void scaleChildToWidget(vector<vector<double>>& vpfBorder, vector<vector<double>>& vpfFrame);
	vector<Wt::WPointF> scaleParentToWidget(vector<vector<vector<double>>>& vvvdBorder, vector<vector<double>>& vvdFrame);
	void setDimensions(int iHeight, int iWidth);
	void setWColour(Wt::WColor& wColour, vector<int> rgb, double percent);
	bool testExcludeParent();
	void updateAreaColour();

protected:
	virtual void paintEvent(Wt::WPaintDevice* paintDevice) override;
};

