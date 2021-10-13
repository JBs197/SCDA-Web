#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "jfunc.h"

using namespace std;

struct JPDFCELL
{
    string alignment = "left";
    vector<double> backgroundColour;
    const vector<vector<double>> cellBLTR;  // For the entire cell.
    HPDF_Font font, fontItalic;
    JFUNC jf;
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

