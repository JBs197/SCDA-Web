#pragma once
#define HPDF_DLL
#include "jpdfcell.h"

class JPDFTABLE
{
    std::vector<std::vector<double>> boxBLTR, titleBLTR;
    std::vector<std::vector<unsigned char>> colourList;  // Form [colour index][r, g, b, a].
    std::vector<std::vector<double>> colourListDouble;  // Form [colour index][r, g, b, a]. All values are inside the interval [0.0, 1.0].
    double fontSizeTitle;
    int numCol = 0;
    int numRow = 0;
    HPDF_Page& page;
    const std::vector<std::vector<double>> tableBLTR;  // Maximum area accorded to the table.
    std::string title;
    std::vector<std::vector<JPDFCELL>> vvCell;  // Form [row index][column value]

    void err(std::string message);
    void initTitle(std::string sTitle, HPDF_Font& font, double fontSize);

public:
    JPDFTABLE(HPDF_Page& pageRef, std::vector<std::vector<double>> bltr, std::string sTitle, HPDF_Font& font, double fontSize)
        : page(pageRef), tableBLTR(bltr)
    {
        initTitle(sTitle, font, fontSize);
        initColour();
    }
    JPDFTABLE(HPDF_Page& pageRef, std::vector<std::vector<double>> bltr) : page(pageRef), tableBLTR(bltr)
    {
        boxBLTR = tableBLTR;
        initColour();
    }
    ~JPDFTABLE() {}

    double borderThickness = 3.0;  // Default, in units of pixels.
    HPDF_Font fontItalic, fontTitle;

    void addColourBars(std::vector<std::vector<double>>& vColour, int iRow, int iCol);
    void addText(std::vector<std::string>& vsText);
    void addText(std::string& text, int iRow, int iCol);
    void addTextList(std::vector<std::string>& vsText, int iRow, int iCol);
    void drawBackgroundColour();
    void drawColourBars(int barWidth);
    void drawColSplit();
    void drawFrames();
    void drawLine(std::vector<std::vector<double>> startStop, std::vector<double> colour, double thickness);
    void drawRect(std::vector<std::vector<double>> rectBLTR, std::vector<double> colour, double thickness);
    void drawRect(std::vector<std::vector<double>> rectBLTR, std::vector<std::vector<double>> vColour, double thickness);
    void drawRectRounded(std::vector<std::vector<double>> rectBLTR, std::vector<double> colour, double thickness, double radius);
    void drawRectRounded(std::vector<std::vector<double>> rectBLTR, std::vector<std::vector<double>> vColour, double thickness, double radius);
    std::vector<std::vector<double>> drawTable();
    void drawText(int index);
    void drawTextList(int italicFreq, std::string separator);
    void drawTitle();
    void initColour();
    void setColourBackground(std::vector<int> vColourIndex);
    void setColourBackground(std::vector<std::vector<int>> vvColourIndex);
    void setRowCol(std::vector<int>& vNumLine, std::vector<double>& vRowHeight, int numCol);
};

