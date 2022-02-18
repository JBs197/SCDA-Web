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
#include "jscalebar.h"

class WTPAINT : public Wt::WPaintedWidget
{
	std::vector<Wt::WPolygonArea*> area;
	std::vector<std::vector<int>> areaColour, keyColour;  // Wt prefers ints for some reason...
	JNUMBER jnumber;
	JSTRING jstr;
	std::vector<std::vector<double>> keyColourDouble;
	std::vector<std::string> areaName, dataName;
	const double barNumberHeight = 14.0;  // Unit of pixels, for a horizontal bar.
	const double barNumberWidth = 100.0;  // Unit of pixels, for a vertical bar.
	const double barThickness = 14.0;  // Unit of pixels.
	std::vector<std::vector<double>> barTLBR, imgTLBR;  // Unit of pixels.
	std::vector<std::vector<Wt::WPointF>> border;  // Form [region index][border point index].
	const double colourDimPercent = 0.7;
	const std::string defaultLength = "200.0";
	double dHeight = -1.0, dWidth = -1.0;  // Unit of pixels.
	std::vector<int> extraColour, unknownColour;
	std::vector<double> extraColourDouble, unknownColourDouble;
	const double legendIdleThreshold = 0.4;  // Minimum percentage of bar unused, to trigger action.
	double legendMin = -1.0, legendMax = -1.0;
	std::unordered_map<std::string, int> mapArea;  // Form "Region Name"->indexArea.
	std::unordered_map<std::string, Wt::WString> mapTooltip;
	const int numColourBands = 5;
	double parentAspectRatio = -1.0;  // Width divided by height.
	int selIndex = -1;
	double widgetPPKM = -1.0;

	void err(std::string message);

public:
	WTPAINT() {
		setLayoutSizeAware(true);
		setDimensions(700, 700);
	}
	~WTPAINT() override {}

	JSCALEBAR jsb;
	int legendBarDouble = -1;  // 0 = single bar, 1 = double bars, 2 = single bar with Canada.
	int legendTickLines = -1;  // How many lines of text are needed at each tick mark on the bar.

	int adjustScale(std::vector<std::vector<int>>& scaleValues, std::string& sUnit);
	std::string areaClicked(int index);
	void clearAreas();
	std::vector<std::string> delinearizeTitle(std::string& linearTitle);
	std::vector<Wt::WPointF> displaceChildToParent(std::vector<std::vector<double>>& vvdBorder, std::vector<double>& childTL, std::vector<double> dispTL);
	void displaceParentToWidget(std::vector<std::vector<std::vector<double>>>& vvvdBorder, std::vector<std::vector<double>>& parentFrameKM);
	std::vector<Wt::WPolygonArea*> drawMap(std::vector<std::string>& vsRegion, std::vector<std::vector<std::vector<double>>>& vvvdFrame, std::vector<std::vector<std::vector<double>>>& vvvdBorder);
	void initColour();
	std::vector<double> getChildTL(std::vector<std::vector<double>>& vpfBorder, std::vector<std::vector<double>>& childFrameKM, std::vector<std::vector<double>>& parentFrameKM);
	std::vector<double> getDimensions();
	std::vector<std::vector<int>> getFrame(std::vector<Wt::WPointF>& path);
	std::vector<std::vector<std::string>> getGraphData();
	int getLegendBarDouble(std::vector<std::string>& vsRegion, std::string sUnit, int displayDataSize);
	int getLegendTickLines(std::string sUnit);
	void makeAreas();
	void paintLegendBar(Wt::WPainter& painter);
	void paintRegionAll(Wt::WPainter& painter);
	void scaleChildToWidget(std::vector<std::vector<double>>& vpfBorder, std::vector<std::vector<double>>& vpfFrame);
	void scaleImgBar();
	void scaleImgBar(std::vector<std::vector<double>>& parentFrameKM);
	std::vector<Wt::WPointF> scaleParentToWidget(std::vector<std::vector<std::vector<double>>>& vvvdBorder, std::vector<std::vector<double>>& parentFrameKM);
	void setDimensions(int iHeight, int iWidth);
	void setWColour(Wt::WColor& wColour, std::vector<int> rgb, double percent);
	void updateAreaColour();
	void updateDisplay(int index);

protected:
	virtual void paintEvent(Wt::WPaintDevice* paintDevice) override;
};


