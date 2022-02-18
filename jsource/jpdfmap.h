#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "jmath.h"
#include "jscalebar.h"

class JPDFMAP
{
	double barNumberHeight = 14.0;  // Unit of pixels, for a horizontal legend bar.
	double barNumberWidth = 104.0;  // Unit of pixels, for a vertical legend bar.
	double barThickness = 14.0;  // Unit of pixels.
	std::vector<std::vector<unsigned char>> colourList;  // Form [colour index][r, g, b, a].
	std::vector<std::vector<double>> colourListDouble;  // Form [colour index][r, g, b, a]. All values are inside the interval [0.0, 1.0].
	std::vector<int> displayData;
	std::vector<double> extraColour, unknownColour;
	HPDF_Font font;
	double fontSize = 14.0;  // Default.
	std::vector<std::vector<double>> imgBLTR, scalebarBLTR;
	JMATH jmath;
	JSCALEBAR jsb;
	std::vector<std::vector<double>> keyColour;  // Form [colour index][r, g, b].
	int legendBarDouble, legendTickLines;
	double legendMax, legendMin;
	const std::vector<std::vector<double>> mapBLTR;  // Maximum area accorded to the bar graph.
	std::vector<std::vector<std::vector<double>>> mapBorder;
	std::vector<std::vector<double>> mapData;
	std::vector<std::string> mapRegion;
	HPDF_Page& page;
	double PPKM = -1.0;
	int selIndex = -1;
	std::string sUnit;

	void err(std::string message);

public:
	JPDFMAP(HPDF_Page& pageRef, HPDF_Font& barFont, std::vector<std::vector<double>> bltr)
		: page(pageRef), font(barFont), mapBLTR(bltr) {
		initColour();
	}
	~JPDFMAP() {}

	void displaceChildToParent(std::vector<std::vector<double>>& vvdBorder, std::vector<std::vector<double>>& childFrame, std::vector<double> dispTL);
	void displaceParentToFrame(std::vector<std::vector<double>>& parentBorderKM, std::vector<std::vector<double>>& parentFrameKM);
	void drawCircle(std::vector<double> coordCenter, double radius, std::vector<double> colour, double thickness);
	void drawCircle(std::vector<double> coordCenter, double radius, std::vector<std::vector<double>> vColour, double thickness);
	void drawGradientBar(std::vector<std::vector<double>> rectBLTR, double frameThickness);
	void drawGradientBar(std::vector<std::vector<double>> rectBLTR, std::vector<std::vector<double>> gradientBLTR, double frameThickness);
	void drawGradientH(std::vector<float> heightStartStop, std::vector<std::vector<double>>& vColour, std::vector<int>& vColourPos);
	void drawGradientV(std::vector<float> widthStartStop, std::vector<std::vector<double>>& vColour, std::vector<int>& vColourPos);
	std::vector<std::vector<double>> drawLegend(int indexDataset);
	void drawLegendTicks(std::vector<std::vector<double>> rectBLTR, std::vector<double>& tickValues, double tickThickness);
	void drawLegendTicks(std::vector<std::vector<double>> rectBLTR, std::vector<std::vector<double>>& tickValues, double tickThickness);
	void drawLegendTicks(std::vector<std::vector<double>> rectBLTR, std::vector<double>& tickValues, double parentValue, double dotRadius, double tickThickness);
	void drawLine(std::vector<std::vector<double>> startStop, std::vector<double> colour, double thickness);
	void drawMap();
	void drawRect(std::vector<std::vector<double>> rectBLTR, std::vector<double> colour, double thickness);
	void drawRect(std::vector<std::vector<double>> rectBLTR, std::vector<std::vector<double>> vColour, double thickness);
	void drawRegion(std::vector<std::vector<double>>& vvdPath, std::vector<double> colour, bool dotted = 0);
	std::vector<double> getChildTL(std::vector<std::vector<double>>& vpfBorder, std::vector<std::vector<double>>& childFrameKM, std::vector<std::vector<double>>& parentFrameKM);
	std::unordered_map<std::string, double> getMapRegionData(int& decimalPlaces);
	void kmToPixel(std::vector<std::vector<std::vector<double>>> frame, std::vector<std::vector<std::vector<double>>>& border);
	void initColour();
	void initImgBar(std::vector<std::vector<double>>& parentFrameKM);
	void initScaleBar(std::string unit, std::vector<int> viIndex, int barDouble, int tickLines);
	void scaleChildToPage(std::vector<std::vector<double>>& vpfBorder, std::vector<std::vector<double>>& vpfFrame);
	void scaleParentToPage(std::vector<std::vector<double>>& parentBorder, std::vector<std::vector<double>>& parentFrameKM);
	void setValues(std::vector<std::string>& region, std::vector<std::vector<double>>& data);
	void textBox(std::vector<std::vector<double>> boxBLTR, std::string sText, std::string alignment);
	void textBox(std::vector<std::vector<double>> boxBLTR, std::string sText, std::string alignment, int fontsize);
};

