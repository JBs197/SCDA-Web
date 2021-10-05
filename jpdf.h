#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "jfunc.h"

using namespace std;
namespace
{
    void  __stdcall error_handler(HPDF_STATUS   error_no, HPDF_STATUS   detail_no, void* user_data)
    {
        printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
    }
}

class JPDFTABLE
{
    vector<vector<double>> boxBLTR, titleBLTR;
    int fontSizeCell, fontSizeTitle;
    HPDF_Font fontTitle;
    int numCol = 0;
    int numRow = 0;
    const vector<vector<double>> tableBLTR;
    string title;
    double width, height;

public:
    JPDFTABLE(vector<vector<double>> bltr) : tableBLTR(bltr) 
    {
        width = tableBLTR[1][0] - tableBLTR[0][0];
        height = tableBLTR[1][1] - tableBLTR[0][1];
    }
    ~JPDFTABLE() {}

    void setTitle(string& sTitle, HPDF_Font& font, int fontSize);

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

