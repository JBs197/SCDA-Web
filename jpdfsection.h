#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include "jpdfbargraph.h"
#include "jpdfmap.h"
#include "jpdftable.h"

using namespace std;

struct JPDFSECTION
{
    HPDF_Font font, fontItalic;
    unique_ptr<JPDFBARGRAPH> jpBargraph = nullptr;
    unique_ptr<JPDFMAP> jpMap = nullptr;
    unique_ptr<JPDFTABLE> jpTable = nullptr;
    JSCALEBAR jsb;
    HPDF_Page& page;
    const vector<vector<double>> sectionBLTR;  // Maximum area accorded to the section.

    JPDFSECTION(HPDF_Page& pageRef, HPDF_Font& Font, vector<vector<double>> bltr) : page(pageRef), font(Font), sectionBLTR(bltr) {}
    ~JPDFSECTION() {}

    void makeBargraph();
    void makeMap();
    void makeTable(string sTitle, double fontSizeTitle);
};

