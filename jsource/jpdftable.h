#pragma once
#define HPDF_DLL
#include "jpdfcell.h"

using namespace std;

class JPDFTABLE
{
    vector<vector<double>> boxBLTR, titleBLTR;
    vector<vector<unsigned char>> colourList;  // Form [colour index][r, g, b, a].
    vector<vector<double>> colourListDouble;  // Form [colour index][r, g, b, a]. All values are inside the interval [0.0, 1.0].
    double fontSizeTitle;
    JFUNC jf;
    int numCol = 0;
    int numRow = 0;
    HPDF_Page& page;
    const vector<vector<double>> tableBLTR;  // Maximum area accorded to the table.
    string title;
    vector<vector<JPDFCELL>> vvCell;  // Form [row index][column value]

    void err(string message);
    void initTitle(string sTitle, HPDF_Font& font, double fontSize);

public:
    JPDFTABLE(HPDF_Page& pageRef, vector<vector<double>> bltr, string sTitle, HPDF_Font& font, double fontSize)
        : page(pageRef), tableBLTR(bltr)
    {
        initTitle(sTitle, font, fontSize);
        initColour();
    }
    JPDFTABLE(HPDF_Page& pageRef, vector<vector<double>> bltr) : page(pageRef), tableBLTR(bltr)
    {
        boxBLTR = tableBLTR;
        initColour();
    }
    ~JPDFTABLE() {}

    double borderThickness = 3.0;  // Default, in units of pixels.
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
    void drawRect(vector<vector<double>> rectBLTR, vector<vector<double>> vColour, double thickness);
    void drawRectRounded(vector<vector<double>> rectBLTR, vector<double> colour, double thickness, double radius);
    void drawRectRounded(vector<vector<double>> rectBLTR, vector<vector<double>> vColour, double thickness, double radius);
    vector<vector<double>> drawTable();
    void drawText(int index);
    void drawTextList(int italicFreq, string separator);
    void drawTitle();
    void initColour();
    void setColourBackground(vector<int> vColourIndex);
    void setColourBackground(vector<vector<int>> vvColourIndex);
    void setRowCol(vector<int>& vNumLine, vector<double>& vRowHeight, int numCol);
};

