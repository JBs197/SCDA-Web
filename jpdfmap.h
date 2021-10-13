#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "jscalebar.h"

using namespace std;

class JPDFMAP
{
	double barNumberHeight = 14.0;  // Unit of pixels, for a horizontal legend bar.
	double barNumberWidth = 100.0;  // Unit of pixels, for a vertical legend bar.
	double barThickness = 14.0;  // Unit of pixels.
	vector<vector<unsigned char>> colourList;  // Form [colour index][r, g, b, a].
	vector<vector<double>> colourListDouble;  // Form [colour index][r, g, b, a]. All values are inside the interval [0.0, 1.0].
	vector<int> displayData;
	vector<double> extraColour, unknownColour;
	HPDF_Font font;
	double fontSize = 14.0;  // Default.
	vector<vector<double>> imgBLTR, scalebarBLTR;
	JFUNC jf;
	JSCALEBAR jsb;
	vector<vector<double>> keyColour;  // Form [colour index][r, g, b].
	int legendBarDouble, legendTickLines;
	double legendMax, legendMin;
	const vector<vector<double>> mapBLTR;  // Maximum area accorded to the bar graph.
	vector<vector<vector<double>>> mapBorder;
	vector<vector<double>> mapData;
	vector<string> mapRegion;
	MATHFUNC mf;
	HPDF_Page& page;
	double PPKM = -1.0;
	int selIndex = -1;
	string sUnit;

public:
	JPDFMAP(HPDF_Page& pageRef, HPDF_Font& barFont, vector<vector<double>> bltr)
		: page(pageRef), font(barFont), mapBLTR(bltr) {
		initColour();
	}
	~JPDFMAP() {}

	void displaceChildToParent(vector<vector<double>>& vvdBorder, vector<vector<double>>& childFrame, vector<double> dispTL);
	void displaceParentToFrame(vector<vector<double>>& parentBorderKM, vector<vector<double>>& parentFrameKM);
	void drawCircle(vector<double> coordCenter, double radius, vector<double> colour, double thickness);
	void drawCircle(vector<double> coordCenter, double radius, vector<vector<double>> vColour, double thickness);
	void drawGradientBar(vector<vector<double>> rectBLTR, double frameThickness);
	void drawGradientBar(vector<vector<double>> rectBLTR, vector<vector<double>> gradientBLTR, double frameThickness);
	void drawGradientH(vector<float> heightStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos);
	void drawGradientV(vector<float> widthStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos);
	vector<vector<double>> drawLegend(int indexDataset);
	void drawLegendTicks(vector<vector<double>> rectBLTR, vector<double>& tickValues, double tickThickness);
	void drawLegendTicks(vector<vector<double>> rectBLTR, vector<vector<double>>& tickValues, double tickThickness);
	void drawLegendTicks(vector<vector<double>> rectBLTR, vector<double>& tickValues, double parentValue, double dotRadius, double tickThickness);
	void drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness);
	void drawMap();
	void drawRect(vector<vector<double>> rectBLTR, vector<double> colour, double thickness);
	void drawRect(vector<vector<double>> rectBLTR, vector<vector<double>> vColour, double thickness);
	void drawRegion(vector<vector<double>>& vvdPath, vector<double> colour);
	vector<double> getChildTL(vector<vector<double>>& vpfBorder, vector<vector<double>>& childFrameKM, vector<vector<double>>& parentFrameKM);
	unordered_map<string, double> getMapRegionData();
	void kmToPixel(vector<vector<vector<double>>>& frame, vector<vector<vector<double>>>& border);
	void initColour();
	void initImgBar(vector<vector<double>>& parentFrameKM);
	void initScaleBar(string unit, vector<int> viIndex, int barDouble, int tickLines);
	void scaleChildToPage(vector<vector<double>>& vpfBorder, vector<vector<double>>& vpfFrame);
	void scaleParentToPage(vector<vector<double>>& parentBorder, vector<vector<double>>& parentFrameKM);
	void setValues(vector<string>& region, vector<vector<double>>& data);
	void textBox(vector<vector<double>> boxBLTR, string sText, string alignment);
	void textBox(vector<vector<double>> boxBLTR, string sText, string alignment, int fontsize);
};

