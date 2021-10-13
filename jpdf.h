#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "jpdfsection.h"
#include "mathfunc.h"

using namespace std;
namespace
{
    void  __stdcall error_handler(HPDF_STATUS   error_no, HPDF_STATUS   detail_no, void* user_data)
    {
        printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
    }
}

class JPDF
{
    HPDF_Font fontAdded, fontAddedItalic, fontDefault;
    int fontSize = 12;  // Default.
    float lineHeight;  // For text.
    JFUNC jf;
    HPDF_Page page;
    HPDF_Doc pdf;

public:
    JPDF() { init(); }
    ~JPDF() {}

    vector<double> cursor;  // Form [xCoord, yCoord]. 
    double margin = 50.0;
    vector<unique_ptr<JPDFSECTION>> vSection;

    int addBargraph(vector<vector<double>>& bargraphBLTR);
    int addMap(vector<vector<double>>& mapBLTR);
    int addTable(int numCol, vector<int> vNumLine, vector<double> vRowHeight, string title, double fontSizeTitle);
    float breakListFitWidth(vector<string>& vsList, float textWidth, vector<vector<string>>& vvsList);
    bool hasFont();
    vector<double> getPageDimensions();
    HPDF_UINT32 getPDF(string& sPDF);
    int getNumLines(vector<string>& vsText, double width, int iFontHeight);
    void init();
    void setFont(string filePath);
    void setFont(string filePath, string filePathItalic);
    void setFontSize(int size);
    void textBox(vector<vector<double>> boxBLTR, string sText, string alignment);
    void textBox(vector<vector<double>> boxBLTR, string sText, string alignment, int fontsize);
};

