#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "jstring.h"

struct JPDFCELL
{
    std::string alignment = "left";
    std::vector<double> backgroundColour;
    const std::vector<std::vector<double>> cellBLTR;  // For the entire cell.
    HPDF_Font font, fontItalic;
    JSTRING jstr;
    double padding = 2.0;
    int rowIndex, colIndex;
    std::vector<double> splitPos;            // List of xCoords where a column split exists.
    std::vector<std::vector<double>> vBarColour;  // Form [bar index][r, g, b, a].
    std::vector<std::vector<std::vector<double>>> vBLTR;  // Form [value index][BL, TR][xCoord, yCoord]
    std::vector<std::string> vsText;

    JPDFCELL(std::vector<std::vector<double>> cBLTR) : cellBLTR(cBLTR) {}
    ~JPDFCELL() {}

    void drawCellBar(HPDF_Page& page, double barWidth);
    void drawCellText(HPDF_Page& page, std::string text, HPDF_Font& Font);
    void drawCellTextItalic(HPDF_Page& page, std::string text);
    void drawCellTextList(HPDF_Page& page, int italicFreq, std::string separator);
    void drawCellTextPlain(HPDF_Page& page, std::string text);
    void drawText(HPDF_Page& page, std::string text);
    double getMaxFontHeight();

private:
    void err(std::string message);
};

