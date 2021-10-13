#include "jpdf.h"

int JPDF::addBargraph(vector<vector<double>>& bargraphBLTR)
{
	// Return the new bar graph's index within the vector.
	int index = vSection.size();
	HPDF_Font font;
	if (fontAdded) { font = fontAdded; }
	else { font = fontDefault; }
	unique_ptr<JPDFSECTION> jpSection = make_unique<JPDFSECTION>(page, font, bargraphBLTR);
	jpSection->makeBargraph();
	vSection.push_back(move(jpSection));
	return index;
}
int JPDF::addMap(vector<vector<double>>& mapBLTR)
{
	// Return the new bar graph's index within the vector.
	int index = vSection.size();
	HPDF_Font font;
	if (fontAdded) { font = fontAdded; }
	else { font = fontDefault; }
	unique_ptr<JPDFSECTION> jpSection = make_unique<JPDFSECTION>(page, font, mapBLTR);
	jpSection->makeMap();
	vSection.push_back(move(jpSection));
	return index;
}
int JPDF::addTable(int numCol, vector<int> vNumLine, vector<double> vRowHeight, string title, double fontSizeTitle)
{
	// Return the new table's index within the vector.
	if (vNumLine.size() != vRowHeight.size()) { jf.err("Size mismatch-jpdf.addTable"); }
	int index = vSection.size();
	int numRow = vRowHeight.size();
	double height = 0.0; 
	for (int ii = 0; ii < numRow; ii++)
	{
		height += vRowHeight[ii];
	}
	height += fontSizeTitle + 4.0;
	vector<vector<double>> tableBLTR(2, vector<double>(2));
	tableBLTR[0] = cursor;
	tableBLTR[1][0] = HPDF_Page_GetWidth(page) - margin;
	tableBLTR[1][1] = tableBLTR[0][1] + height;

	HPDF_Font font;
	if (fontAdded) { font = fontAdded; }
	else { font = fontDefault; }
	unique_ptr<JPDFSECTION> jpSection = make_unique<JPDFSECTION>(page, font, tableBLTR);
	if (fontAddedItalic) { jpSection->fontItalic = fontAddedItalic; }
	jpSection->makeTable(title, fontSizeTitle);
	jpSection->jpTable->setRowCol(vNumLine, vRowHeight, numCol);
	vSection.push_back(move(jpSection));
	return index;
}
float JPDF::breakListFitWidth(vector<string>& vsList, float textWidth, vector<vector<string>>& vvsList)
{
	// For a given vsList and max width of text, split each list item as necessary into
	// multiple lines such that the max width is never exceeded. Return the total height.
	vvsList.resize(vsList.size(), vector<string>());
	int index, numLines = 0;
	HPDF_UINT maxChars;
	for (int ii = 0; ii < vsList.size(); ii++)
	{
		index = 0;
		vvsList[ii] = { vsList[ii] };
		maxChars = HPDF_Page_MeasureText(page, vvsList[ii][index].c_str(), textWidth, HPDF_TRUE, NULL);
		while (maxChars < vvsList[ii][index].size())
		{
			vvsList[ii].push_back(vvsList[ii][index].substr(maxChars));
			vvsList[ii][index].resize(maxChars - 1);
			index++;
			maxChars = HPDF_Page_MeasureText(page, vvsList[ii][index].c_str(), textWidth, HPDF_TRUE, NULL);
		}
		numLines += vvsList[ii].size();
	}
	float textHeight = (float)numLines * lineHeight;
	return textHeight;
}
vector<double> JPDF::getPageDimensions()
{
	// Returns the (width, height) of the active page (inside the margins), in pixels.
	vector<double> vdDim(2);
	vdDim[0] = (double)(HPDF_Page_GetWidth(page) - (2.0 * margin));
	if (vdDim[0] <= 0.0) { jf.err("GetWidth-jpdf.getPageDimensions"); }
	vdDim[1] = (double)(HPDF_Page_GetHeight(page) - (2.0 * margin));
	if (vdDim[1] <= 0.0) { jf.err("GetHeight-jpdf.getPageDimensions"); }
	return vdDim;
}
HPDF_UINT32 JPDF::getPDF(string& sPDF)
{
	// Return the complete PDF as a string. 
	sPDF.clear();
	HPDF_STATUS error = HPDF_SaveToStream(pdf);
	if (error != HPDF_OK) { jf.err("SaveToStream-jpdf.getPDF"); }
	HPDF_UINT32 streamSize = HPDF_GetStreamSize(pdf);
	if (!streamSize) { jf.err("GetStreamSize-jpdf.getPDF"); }
	auto buffer = new HPDF_BYTE[streamSize];
	error = HPDF_ReadFromStream(pdf, buffer, &streamSize);
	if (error != HPDF_OK) { jf.err("ReadFromStream-jpdf.getPDF"); }
	sPDF.resize(streamSize);
	for (int ii = 0; ii < streamSize; ii++)
	{
		sPDF[ii] = (char)buffer[ii];
	}
	delete[] buffer;
	error = HPDF_ResetStream(pdf);
	if (error != HPDF_OK) { jf.err("ResetStream-jpdf.getPDF"); }
	return streamSize;
}
int JPDF::getNumLines(vector<string>& vsText, double width, int iFontHeight)
{
	int numLines = 1;
	size_t pos1 = 0;
	string piece1, piece2;
	float fontHeight = (float)iFontHeight, textWidth;
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, fontAdded, fontHeight);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.getNumLines"); }
	float separatorWidth = HPDF_Page_TextWidth(page, " | ");
	if (separatorWidth == 0.0) { jf.err("TextWidth-jpdf.getNumLines"); }
	float separatorWidthShort = HPDF_Page_TextWidth(page, " |");
	if (separatorWidthShort == 0.0) { jf.err("TextWidth-jpdf.getNumLines"); }

	double remains = width;
	for (int ii = 0; ii < vsText.size(); ii++)
	{
		if (ii > 0) 
		{ 
			remains -= separatorWidth; 
			if (remains < 0.0)
			{
				numLines++;
				remains += (separatorWidth - separatorWidthShort);
				if (remains < 0.0)
				{
					remains = width - separatorWidthShort;
				}
				else
				{
					remains = width;
				}
			}
		}
		textWidth = HPDF_Page_TextWidth(page, vsText[ii].c_str());
		if (textWidth == 0.0) { jf.err("TextWidth-jpdf.getNumLines"); }
		if (remains < textWidth)
		{
			numLines++;
			piece1.clear();
			piece2.clear();
			pos1 = vsText[ii].size();
			while (remains < textWidth)
			{
				pos1 = vsText[ii].rfind(' ', pos1 - 1);
				if (pos1 > vsText[ii].size()) { break; }
				piece1 = vsText[ii].substr(0, pos1);
				textWidth = HPDF_Page_TextWidth(page, piece1.c_str());
				if (textWidth == 0.0) { jf.err("TextWidth-jpdfcell.jpdf.getNumLines"); }
			}
			if (pos1 < vsText[ii].size())
			{
				piece2 = vsText[ii].substr(pos1 + 1);
				textWidth = HPDF_Page_TextWidth(page, piece2.c_str());
				if (textWidth == 0.0) { jf.err("TextWidth-jpdfcell.jpdf.getNumLines"); }
			}
			else
			{
				textWidth = HPDF_Page_TextWidth(page, vsText[ii].c_str());
				if (textWidth == 0.0) { jf.err("TextWidth-jpdf.getNumLines"); }
			}
			remains = width - textWidth;
		}
		else { remains -= textWidth; }
	}
	return numLines;
}
bool JPDF::hasFont()
{
	if (fontAdded) { return 1; }
	return 0;
}
void JPDF::init()
{
	pdf = HPDF_New(error_handler, NULL);
	if (!pdf) { jf.err("Failed to create pdf object-jpdf.init"); }
	HPDF_UseUTFEncodings(pdf);
	HPDF_SetCurrentEncoder(pdf, "UTF-8");
	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
	fontDefault = HPDF_GetFont(pdf, "Helvetica", NULL); 
	page = HPDF_AddPage(pdf);
	HPDF_STATUS error = HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);
	if (error != HPDF_OK) { jf.err("Page_SetSize-jpdf.init"); }
	cursor = { margin, HPDF_Page_GetHeight(page) - margin };  // Top-left corner within the default margin.
	lineHeight = (float)fontSize + 4.0;
}
void JPDF::setFont(string filePath)
{
	auto fontName = HPDF_LoadTTFontFromFile(pdf, filePath.c_str(), HPDF_TRUE);
	fontAdded = HPDF_GetFont(pdf, fontName, NULL);
}
void JPDF::setFont(string filePath, string filePathItalic)
{
	auto fontName = HPDF_LoadTTFontFromFile(pdf, filePath.c_str(), HPDF_TRUE);
	fontAdded = HPDF_GetFont(pdf, fontName, "UTF-8");
	auto fontNameItalic = HPDF_LoadTTFontFromFile(pdf, filePathItalic.c_str(), HPDF_TRUE);
	fontAddedItalic = HPDF_GetFont(pdf, fontNameItalic, "UTF-8");
}
void JPDF::setFontSize(int size)
{
	fontSize = size;
	lineHeight = (float)fontSize + 4.0;
}
void JPDF::textBox(vector<vector<double>> boxBLTR, string sText, string alignment)
{
	// This variant uses the default font size.
	textBox(boxBLTR, sText, alignment, fontSize);
}
void JPDF::textBox(vector<vector<double>> boxBLTR, string sText, string alignment, int fontsize)
{
	// Accepted alignment strings are "left", "right", "center", "justify".
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, fontDefault, fontsize);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.textBox"); }
	HPDF_Page_BeginText(page);
	if (alignment == "right")
	{
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_RIGHT, NULL);
	}
	else if (alignment == "center")
	{
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_CENTER, NULL);
	}
	else if (alignment == "justify")
	{
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_JUSTIFY, NULL);
	}
	else
	{
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_LEFT, NULL);
	}
	if (error != HPDF_OK) { jf.err("TextRect-jpdf.textBox"); }
	HPDF_Page_EndText(page);
}
