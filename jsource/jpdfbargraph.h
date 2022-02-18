#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "jscalebar.h"

class JPDFBARGRAPH
{
    const std::vector<std::vector<double>> bargraphBLTR;  // Maximum area accorded to the bar graph.
    std::vector<double> black = { 0.0, 0.0, 0.0 };
    HPDF_Font font;
    JNUMBER jnumber;
    JSCALEBAR jsb;
    JSTRING jstr;
    HPDF_Page& page;
    std::string sUnit;
    std::vector<std::vector<double>> vRegionData;  // Form [indexRegion][series0, series1, ...]
    std::vector<std::string> xValues;

    void err(std::string message);

public:
    JPDFBARGRAPH(HPDF_Page& pageRef, HPDF_Font& barFont, std::vector<std::vector<double>> bltr)
        : page(pageRef), font(barFont), bargraphBLTR(bltr) {}
    ~JPDFBARGRAPH() {}

    std::vector<std::vector<double>> dataBLTR, xAxisBLTR, yAxisBLTR;
    double fontSize = 12.0;
    double lineThickness = 2.0;
    double padding = 2.0;
    double tickLength = 7.0;
    double xMaxHeightPercentage = 1.0 / 3.0;  // Maximum ratio of vertical space accorded to the angled x-axis names.

    void addRegionData(int indexRegion, std::vector<double>& regionData);
    std::vector<double> angledTRtoBL(double xTR, double yTR, double textLength, double textHeight, double thetaRad);
    void drawAxis(std::vector<std::vector<double>> startStop, std::vector<double> vTick, std::vector<double> colour, double thickness);
    void drawAxisX(std::vector<std::string> vsValue, double rotationDeg);
    void drawAxisY(std::vector<double>& minMax, std::string unit);
    void drawData(std::vector<std::vector<double>>& seriesColour);
    void drawLine(std::vector<std::vector<double>> startStop, std::vector<double> colour, double thickness);
    void initAxisX(std::vector<std::string> vsValue, double rotationDeg);
    void initAxisY(std::vector<double>& minMax, std::string unit);
    std::vector<std::string> splitAtSpaceBox(std::string tooLong, std::vector<std::vector<double>>& boxBLTR, double& fontHeight, double thetaRad, double textPadding, double topXMinTR);
    std::vector<std::string> splitAtSpaceEven(std::string tooLong, int pieces);
    void textBox(std::vector<std::vector<double>> boxBLTR, std::string sText, std::string alignment, double fontsize);
    void textBoxAngled(std::string sText, std::vector<double> corner, std::string sCorner, double fontsize, double thetaRad);
};

