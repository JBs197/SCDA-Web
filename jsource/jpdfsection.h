#pragma once
#define HPDF_DLL
#include <hpdf.h>
#include <mutex>
#include "jpdfbargraph.h"
#include "jpdfmap.h"
#include "jpdftable.h"
#include "jsort.h"

struct JPDFSECTION
{
    HPDF_Font font, fontItalic;
    std::unique_ptr<JPDFBARGRAPH> jpBargraph = nullptr;
    std::unique_ptr<JPDFMAP> jpMap = nullptr;
    std::unique_ptr<JPDFTABLE> jpTable = nullptr;
    JSCALEBAR jsb;
    HPDF_Page& page;
    const std::vector<std::vector<double>> sectionBLTR;  // Maximum area accorded to the section.

    JPDFSECTION(HPDF_Page& pageRef, HPDF_Font& Font, std::vector<std::vector<double>> bltr) : page(pageRef), font(Font), sectionBLTR(bltr) {}
    ~JPDFSECTION() {}

    void makeBargraph();
    void makeMap();
    void makeTable(std::string sTitle, double fontSizeTitle);
};

