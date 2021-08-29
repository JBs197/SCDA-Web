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
	vector<double>* activeData = nullptr;
	vector<double>* activeDataChildren = nullptr;
	vector<Wt::WPolygonArea*> area;
	vector<vector<int>> areaColour, keyColour;  // Wt prefers ints for some reason...
	vector<vector<double>> areaData, areaDataChildren;
	vector<double> areaDataProcessed, areaDataProcessedChildren;
	vector<string> areaName, dataName;
	const double barNumberHeight = 14.0;  // Unit of pixels, for a horizontal bar.
	const double barNumberWidth = 100.0;  // Unit of pixels, for a vertical bar.
	const double barThickness = 14.0;  // Unit of pixels.
	vector<vector<double>> barTLBR, imgTLBR;  // Unit of pixels.
	vector<vector<Wt::WPointF>> border;  // Form [region index][border point index].
	const double colourDimPercent = 0.7;
	const string defaultLength = "200.0";
	double dHeight = -1.0, dWidth = -1.0;  // Unit of pixels.
	vector<int> displayData;
	vector<int> extraColour;
	JFUNC jf;
	int legendBarDouble = -1;  // 0 = single bar, 1 = double bars, 2 = single bar with Canada.
	int legendTickLines = -1;  // How many lines of text are needed at each tick mark on the bar.
	unordered_map<string, int> mapArea;  // Form "Region Name"->indexArea.
	unordered_map<string, Wt::WString> mapTooltip;
	const int numColourBands = 5;
	double parentAspectRatio = -1.0;  // Width divided by height.
	int selIndex = -1;
	string sUnit;
	double widgetPPKM = -1.0;

public:
	WTPAINT() {
		setLayoutSizeAware(true);
		setDimensions(700, 700);
	}
	~WTPAINT() override {}

	int adjustScale(vector<vector<int>>& scaleValues, string& sUnit);
	string areaClicked(int index);
	void clearAreas();
	vector<string> delinearizeTitle(string& linearTitle);
	vector<Wt::WPointF> displaceChildToParent(vector<vector<double>>& vvdBorder, vector<double>& childTL, vector<double> dispTL);
	void displaceParentToWidget(vector<vector<vector<double>>>& vvvdBorder, vector<vector<double>>& parentFrameKM);
	vector<Wt::WPolygonArea*> drawMap(vector<vector<vector<double>>>& vvvdBorder, vector<string>& vsRegion, vector<vector<double>>& vvdData);
	void initColour();
	vector<double> getChildTL(vector<vector<double>>& vpfBorder, vector<vector<double>>& childFrameKM, vector<vector<double>>& parentFrameKM);
	vector<vector<int>> getFrame(vector<Wt::WPointF>& path);
	vector<vector<double>> getParentFrameKM(vector<vector<vector<double>>>& vvvdBorder);
	vector<vector<int>> getScaleValues(int numTicks);
	void makeAreas();
	void paintLegendBar(Wt::WPainter& painter);
	void paintRegionAll(Wt::WPainter& painter);
	vector<double> processPercent(vector<int> viIndex);
	void scaleChildToWidget(vector<vector<double>>& vpfBorder, vector<vector<double>>& vpfFrame);
	void scaleImgBar();
	void scaleImgBar(vector<vector<double>>& parentFrameKM);
	vector<Wt::WPointF> scaleParentToWidget(vector<vector<vector<double>>>& vvvdBorder, vector<vector<double>>& parentFrameKM);
	void setDimensions(int iHeight, int iWidth);
	void setUnit(string unit, vector<int> viIndex);
	void setWColour(Wt::WColor& wColour, vector<int> rgb, double percent);
	void updateAreaColour();
	void updateAreaColour(vector<int> viIndex);
	void updateDisplay(vector<int> viIndex);

protected:
	virtual void paintEvent(Wt::WPaintDevice* paintDevice) override;
};


