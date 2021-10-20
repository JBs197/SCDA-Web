#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "jscalebar.h"

using namespace std;

class JPDFBARGRAPH
{
    const vector<vector<double>> bargraphBLTR;  // Maximum area accorded to the bar graph.
    vector<double> black = { 0.0, 0.0, 0.0 };
    HPDF_Font font;
    JFUNC jf;
    JSCALEBAR jsb;
    HPDF_Page& page;
    MATHFUNC mf;
    string sUnit;
    vector<vector<double>> vRegionData;  // Form [indexRegion][series0, series1, ...]
    vector<string> xValues;

public:
    JPDFBARGRAPH(HPDF_Page& pageRef, HPDF_Font& barFont, vector<vector<double>> bltr)
        : page(pageRef), font(barFont), bargraphBLTR(bltr) {}
    ~JPDFBARGRAPH() {}

    vector<vector<double>> dataBLTR, xAxisBLTR, yAxisBLTR;
    double fontSize = 12.0;
    double lineThickness = 2.0;
    double padding = 2.0;
    double tickLength = 7.0;
    double xMaxHeightPercentage = 1.0 / 3.0;  // Maximum ratio of vertical space accorded to the angled x-axis names.

    void addRegionData(int indexRegion, vector<double>& regionData);
    vector<double> angledTRtoBL(double xTR, double yTR, double textLength, double textHeight, double thetaRad);
    void drawAxis(vector<vector<double>> startStop, vector<double> vTick, vector<double> colour, double thickness);
    void drawAxisX(vector<string> vsValue, double rotationDeg);
    void drawAxisY(vector<double>& minMax, string unit);
    void drawData(vector<vector<double>>& seriesColour);
    void drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness);
    void initAxisX(vector<string> vsValue, double rotationDeg);
    void initAxisY(vector<double>& minMax, string unit);
    vector<string> splitAtSpaceBox(string tooLong, vector<vector<double>>& boxBLTR, double& fontHeight, double thetaRad, double textPadding, double topXMinTR);
    vector<string> splitAtSpaceEven(string tooLong, int pieces);
    void textBox(vector<vector<double>> boxBLTR, string sText, string alignment, double fontsize);
    void textBoxAngled(string sText, vector<double> corner, string sCorner, double fontsize, double thetaRad);
};

