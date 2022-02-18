#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "jpdfsection.h"

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
    HPDF_Page page;
    HPDF_Doc pdf;
    std::mutex m_pdf;

    void err(std::string message);

public:
    JPDF() { init(); }
    ~JPDF() {}

    std::vector<double> cursor;  // Form [xCoord, yCoord]. 
    double margin = 50.0;
    std::vector<std::shared_ptr<JPDFSECTION>> vSection;

    int addBargraph(std::vector<std::vector<double>>& bargraphBLTR);
    int addMap(std::vector<std::vector<double>>& mapBLTR);
    int addTable(int numCol, std::vector<int> vNumLine, std::vector<double> vRowHeight, std::string title, double fontSizeTitle);
    float breakListFitWidth(std::vector<std::string>& vsList, float textWidth, std::vector<std::vector<std::string>>& vvsList);
    bool hasFont();
    std::vector<double> getPageDimensions();
    HPDF_UINT32 getPDF(std::string& sPDF);
    HPDF_UINT32 getPDF(std::vector<unsigned char>& binPDF);
    int getNumLines(std::vector<std::string>& vsText, double width, int iFontHeight);
    void init();
    void setFont(std::string filePath);
    void setFont(std::string filePath, std::string filePathItalic);
    void setFontSize(int size);
    void textBox(std::vector<std::vector<double>> boxBLTR, std::string sText, std::string alignment);
    void textBox(std::vector<std::vector<double>> boxBLTR, std::string sText, std::string alignment, int fontsize);
};

