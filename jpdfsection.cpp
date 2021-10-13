#include "jpdfsection.h"

void JPDFSECTION::makeBargraph()
{
	jpBargraph = make_unique<JPDFBARGRAPH>(page, font, sectionBLTR);
}
void JPDFSECTION::makeMap()
{
	jpMap = make_unique<JPDFMAP>(page, font, sectionBLTR);
}
void JPDFSECTION::makeTable(string sTitle, double fontSizeTitle)
{
	jpTable = make_unique<JPDFTABLE>(page, sectionBLTR, sTitle, font, fontSizeTitle);
	if (fontItalic) { jpTable->fontItalic = fontItalic; }
}
