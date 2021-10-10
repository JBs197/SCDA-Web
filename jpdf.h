#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "mathfunc.h"

using namespace std;
namespace
{
    void  __stdcall error_handler(HPDF_STATUS   error_no, HPDF_STATUS   detail_no, void* user_data)
    {
        printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
    }
}

class JPDFBARGRAPH
{
    const vector<vector<double>> bargraphBLTR;  // Maximum area accorded to the bar graph.
    vector<double> black = { 0.0, 0.0, 0.0 };
    HPDF_Font font;
    JFUNC jf;
    HPDF_Page& page;
    MATHFUNC mf;
    string sUnit;
    vector<vector<double>> vRegionData;  // Form [indexRegion][series0, series1, ...]
    vector<string> xValues;

public:
    JPDFBARGRAPH(HPDF_Page& pageRef, HPDF_Font& barFont, vector<vector<double>>& bltr) 
        : page(pageRef), font(barFont), bargraphBLTR(bltr) {}
    ~JPDFBARGRAPH() {}

    vector<vector<double>> dataBLTR, xAxisBLTR, yAxisBLTR;
    double fontSize = 12.0;
    double lineThickness = 2.0;
    double padding = 2.0;
    double tickLength = 7.0;

    void addRegionData(int indexRegion, vector<double>& regionData);
    void addValuesX(vector<string>& vsValue, double rotationDeg);
    void drawAxis(vector<vector<double>> startStop, vector<double> vTick, vector<double> colour, double thickness);
    void drawAxisX(vector<string>& vsValue, double rotationDeg);
    void drawAxisY(vector<double>& minMax, string unit);
    void drawData(vector<vector<double>>& seriesColour);
    void drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness);
    void textBox(vector<vector<double>> boxBLTR, string sText, string alignment, int fontsize);
};

struct JPDFCELL
{
    string alignment = "left";
    vector<double> backgroundColour;
    const vector<vector<double>> cellBLTR;  // For the entire cell.
    HPDF_Font font, fontItalic;
    double padding = 2.0;
    int rowIndex, colIndex;
    vector<double> splitPos;            // List of xCoords where a column split exists.
    vector<vector<double>> vBarColour;  // Form [bar index][r, g, b, a].
    vector<vector<vector<double>>> vBLTR;  // Form [value index][BL, TR][xCoord, yCoord]
    vector<string> vsText;

    JPDFCELL(vector<vector<double>> cBLTR) : cellBLTR(cBLTR) {}
    ~JPDFCELL() {}

    void drawCellBar(HPDF_Page& page, double barWidth, JFUNC& jf);
    void drawCellText(HPDF_Page& page, string text, JFUNC& jf);
    void drawCellTextItalic(HPDF_Page& page, int indexText, JFUNC& jf);
    void drawCellTextItalic(HPDF_Page& page, string text, JFUNC& jf);
    void drawCellTextPlain(HPDF_Page& page, int indexText, JFUNC& jf);
    void drawCellTextPlain(HPDF_Page& page, string text, JFUNC& jf);
    double getMaxFontHeight();
};

class JPDFTABLE
{
    vector<vector<double>> boxBLTR, titleBLTR;
    int fontSizeTitle;
    JFUNC jf;
    int numCol = 0;
    int numRow = 0;
    HPDF_Page& page;
    const vector<vector<double>> tableBLTR;  // Maximum area accorded to the table.
    string title;
    vector<vector<JPDFCELL>> vvCell;  // Form [row index][column value]

    void initTitle(string sTitle, HPDF_Font& font, int fontSize);

public:
    JPDFTABLE(HPDF_Page& pageRef, vector<vector<double>> bltr, string sTitle, HPDF_Font& font, int fontSize)
        : page(pageRef), tableBLTR(bltr) 
    { 
        initTitle(sTitle, font, fontSize); 
    }
    JPDFTABLE(HPDF_Page& pageRef, vector<vector<double>> bltr) : page(pageRef), tableBLTR(bltr)
    {
        boxBLTR = tableBLTR;
    }
    ~JPDFTABLE() {}

    double borderThickness = 3.0;  // Default, in units of pixels.
    vector<vector<double>> colourListDouble;  // Form [colour index][r, g, b, a].
    HPDF_Font fontItalic, fontTitle;

    void addColourBars(vector<vector<double>>& vColour, int iRow, int iCol);
    void addText(vector<string>& vsText);
    void addText(string& text, int iRow, int iCol);
    void addTextList(vector<string>& vsText, int iRow, int iCol);
    void drawBackgroundColour();
    void drawColourBars(int barWidth);
    void drawColSplit();
    void drawFrames();
    void drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness);
    void drawRect(vector<vector<double>> rectBLTR, vector<double> colour, double thickness);
    vector<vector<double>> drawTable();
    void drawText(int index);
    void drawTextListItalic(int italicFreq);
    void drawTitle();
    void setColourBackground(vector<vector<int>> vvColourIndex);
    void setRowCol(vector<int>& vNumLine, vector<double>& vRowHeight, int numCol);
};

class JPDF
{
    vector<vector<unsigned char>> colourList;  // Form [colour index][r, g, b, a].
    vector<vector<double>> colourListDouble;  // Form [colour index][r, g, b, a]. All values are inside the interval [0.0, 1.0].
    HPDF_Font fontAdded, fontAddedItalic, fontDefault;
    int fontSize = 18;  // Default.
    float lineHeight;  // For text.
    JFUNC jf;
    HPDF_Page page;
    HPDF_Doc pdf;

public:
    JPDF() { init(); }
    ~JPDF() {}

    vector<double> cursor;  // Form [xCoord, yCoord]. 
    double margin = 50.0;
    vector<JPDFBARGRAPH> vBarGraph;
    vector<JPDFTABLE> vTable;

    int addBarGraph(vector<vector<double>>& bargraphBLTR);
    int addTable(int numCol, vector<int> vNumLine, vector<double> vRowHeight, string title, double fontSizeTitle);
    float breakListFitWidth(vector<string>& vsList, float textWidth, vector<vector<string>>& vvsList);
    void drawCircle(vector<double> coordCenter, double radius, vector<double> colour, double thickness);
    void drawCircle(vector<double> coordCenter, double radius, vector<vector<double>> vColour, double thickness);
    void drawGradientH(vector<float> heightStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos);
    void drawGradientV(vector<float> widthStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos);
    void drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness);
    void drawRect(vector<vector<double>> rectBLTR, vector<double> colour, double thickness);
    void drawRect(vector<vector<double>> rectBLTR, vector<vector<double>> vColour, double thickness);
    void drawRegion(vector<vector<double>>& vvdPath, vector<double> colour);
    bool hasFont();
    vector<double> getPageDimensions();
    HPDF_UINT32 getPDF(string& sPDF);
    int getNumLines(vector<string>& vsText, double width, int iFontHeight);
    void init();
    void initColour();
    void parameterSectionBottom(vector<string>& vsParameter, vector<int>& vColour);
    void sectionListBoxed(string sTitle, vector<string>& vsList);
    void sectionListBoxed(string sTitle, vector<string>& vsList, vector<int>& vColour);
    void sectionListBoxed(string sTitle, vector<string>& vsList, vector<int>& vColour, int numCol);
    void setFont(string filePath);
    void setFont(string filePath, string filePathItalic);
    void setFontSize(int size);
    void textBox(vector<vector<double>> boxBLTR, string sText, string alignment);
    void textBox(vector<vector<double>> boxBLTR, string sText, string alignment, int fontsize);
};

