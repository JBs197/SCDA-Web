#include "jpdftable.h"

using namespace std;

void JPDFTABLE::addColourBars(vector<vector<double>>& vColour, int iRow, int iCol)
{
	// Adds a list of colours to a single table cell.
	vvCell[iRow][iCol].vBarColour = vColour;
}
void JPDFTABLE::addText(vector<string>& vsText)
{
	// Adds one string of text to each cell, in sequential order.
	int indexText = 0, max = vsText.size();
	for (int ii = 0; ii < numCol; ii++)
	{
		for (int jj = 0; jj < numRow; jj++)
		{
			vvCell[jj][ii].vsText.push_back(vsText[indexText]);
			indexText++;
			if (indexText == max) { return; }
		}
	}
}
void JPDFTABLE::addText(string& text, int iRow, int iCol)
{
	// Adds one string of text to a specific cell.
	vvCell[iRow][iCol].vsText.push_back(text);
}
void JPDFTABLE::addTextList(vector<string>& vsText, int iRow, int iCol)
{
	// Adds a list of strings to a specific cell.
	for (int ii = 0; ii < vsText.size(); ii++)
	{
		vvCell[iRow][iCol].vsText.push_back(vsText[ii]);
	}
}
void JPDFTABLE::drawBackgroundColour()
{
	HPDF_STATUS error;
	double width, height;
	for (int ii = 0; ii < numRow; ii++)
	{
		for (int jj = 0; jj < numCol; jj++)
		{
			width = vvCell[ii][jj].cellBLTR[1][0] - vvCell[ii][jj].cellBLTR[0][0];
			height = vvCell[ii][jj].cellBLTR[1][1] - vvCell[ii][jj].cellBLTR[0][1];
			error = HPDF_Page_SetRGBFill(page, vvCell[ii][jj].backgroundColour[0], vvCell[ii][jj].backgroundColour[1], vvCell[ii][jj].backgroundColour[2]);
			if (error != HPDF_OK) { err("SetRGBFill-jpdftable.drawBackgroundColour"); }
			error = HPDF_Page_Rectangle(page, vvCell[ii][jj].cellBLTR[0][0], vvCell[ii][jj].cellBLTR[0][1], width, height);
			if (error != HPDF_OK) { err("Rectangle-jpdftable.drawBackgroundColour"); }
			error = HPDF_Page_Fill(page);
			if (error != HPDF_OK) { err("Fill-jpdftable.drawBackgroundColour"); }
		}
	}
}
void JPDFTABLE::drawColourBars(int barWidth)
{
	// Draw every cell's pre-loaded list of colours, as rectangles of cell height and given width.
	double dWidth = (double)barWidth;
	for (int ii = 0; ii < numCol; ii++) {
		for (int jj = 0; jj < numRow; jj++) {
			vvCell[jj][ii].drawCellBar(page, dWidth);
		}
	}
}
void JPDFTABLE::drawColSplit()
{
	// Draw a thin vertical line through every cell, at the beginning of its rightmost vBLTR.
	double padding = 0.0, xMax, xPos;
	int index;
	vector<vector<double>> startStop(2, vector<double>(2));
	startStop[0][1] = boxBLTR[0][1];
	startStop[1][1] = boxBLTR[1][1];
	for (int ii = 0; ii < numCol; ii++)
	{
		xMax = 0.0;
		for (int jj = 0; jj < numRow; jj++)
		{
			index = vvCell[jj][ii].vBLTR.size() - 1;
			xPos = vvCell[jj][ii].vBLTR[index][0][0];
			if (xPos > xMax)
			{
				xMax = xPos;
				padding = vvCell[jj][ii].padding;
			}
		}
		xMax += padding;
		startStop[0][0] = xMax;
		startStop[1][0] = xMax;
		drawLine(startStop, colourListDouble[0], 1.0);
		for (int jj = 0; jj < numRow; jj++)
		{
			index = vvCell[jj][ii].vBLTR.size() - 1;
			vvCell[jj][ii].splitPos.push_back(xMax);
			vvCell[jj][ii].vBLTR[index][0][0] = xMax + vvCell[jj][ii].padding;
		}
	}
}
void JPDFTABLE::drawFrames()
{
	// Draws the value box frame, as well as the column divisor lines.
	double thickness = max(borderThickness, 1.0);
	auto frameBLTR = boxBLTR;
	frameBLTR[0][0] -= floor(0.5 * thickness);
	frameBLTR[1][0] += floor(0.5 * thickness);
	frameBLTR[0][1] -= floor(0.5 * thickness);
	frameBLTR[1][1] += floor(0.5 * thickness);
	drawRectRounded(frameBLTR, colourListDouble[0], thickness, -1.0);

	thickness = max(borderThickness - 1.0, 1.0);
	vector<vector<double>> startStop(2, vector<double>(2));
	startStop[0][1] = boxBLTR[0][1];
	startStop[1][1] = boxBLTR[1][1];
	for (int ii = 1; ii < numCol; ii++)
	{
		startStop[0][0] = vvCell[0][ii].cellBLTR[0][0];
		startStop[1][0] = startStop[0][0];
		drawLine(startStop, colourListDouble[0], thickness);
	}

	thickness = max(borderThickness - 2.0, 1.0);
	int numSplits;
	for (int ii = 0; ii < numCol; ii++)
	{
		numSplits = vvCell[0][ii].vBLTR.size() - 1;
		for (int jj = 0; jj < numSplits; jj++)
		{
			startStop[0][0] = vvCell[0][ii].vBLTR[jj][1][0];
			startStop[1][0] = startStop[0][0];
			drawLine(startStop, colourListDouble[0], thickness);
		}
	}
}
void JPDFTABLE::drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness)
{
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { err("SetLineWidth-jpdftable.drawLine"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdftable.drawLine"); }
	error = HPDF_Page_MoveTo(page, startStop[0][0], startStop[0][1]);
	if (error != HPDF_OK) { err("MoveTo-jpdftable.drawLine"); }
	error = HPDF_Page_LineTo(page, startStop[1][0], startStop[1][1]);
	if (error != HPDF_OK) { err("LineTo-jpdftable.drawLine"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { err("Stroke-jpdftable.drawLine"); }
}
void JPDFTABLE::drawRect(vector<vector<double>> rectBLTR, vector<double> colour, double thickness)
{
	// This variant will only draw the perimeter.
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { err("SetLineWidth-jpdf.drawRect"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdf.drawRect"); }
	error = HPDF_Page_MoveTo(page, rectBLTR[0][0], rectBLTR[0][1]);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRect"); }
	error = HPDF_Page_Rectangle(page, rectBLTR[0][0], rectBLTR[0][1], rectBLTR[1][0] - rectBLTR[0][0], rectBLTR[1][1] - rectBLTR[0][1]);
	if (error != HPDF_OK) { err("Rectangle-jpdf.drawRect"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { err("Stroke-jpdf.drawRect"); }
}
void JPDFTABLE::drawRect(vector<vector<double>> rectBLTR, vector<vector<double>> vColour, double thickness)
{
	// This variant will draw the perimeter using the first colour, and fill the 
	// rectangle using the second colour. 
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { err("SetLineWidth-jpdf.drawRect"); }
	error = HPDF_Page_SetRGBStroke(page, vColour[0][0], vColour[0][1], vColour[0][2]);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdf.drawRect"); }
	error = HPDF_Page_SetRGBFill(page, vColour[1][0], vColour[1][1], vColour[1][2]);
	if (error != HPDF_OK) { err("SetRGBFill-jpdf.drawRect"); }
	error = HPDF_Page_MoveTo(page, rectBLTR[0][0], rectBLTR[0][1]);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRect"); }
	error = HPDF_Page_Rectangle(page, rectBLTR[0][0], rectBLTR[0][1], rectBLTR[1][0] - rectBLTR[0][0], rectBLTR[1][1] - rectBLTR[0][1]);
	if (error != HPDF_OK) { err("Rectangle-jpdf.drawRect"); }
	error = HPDF_Page_FillStroke(page);
	if (error != HPDF_OK) { err("FillStroke-jpdf.drawRect"); }
}
void JPDFTABLE::drawRectRounded(vector<vector<double>> rectBLTR, vector<double> colour, double thickness, double radius)
{
	// This variant will only draw the perimeter.
	double cornerRadius;
	if (radius > 0.0) { cornerRadius = radius; }
	else { cornerRadius = 2.0 * thickness; }

	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { err("SetLineWidth-jpdf.drawRectRounded"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdf.drawRectRounded"); }
	error = HPDF_Page_MoveTo(page, rectBLTR[0][0] + cornerRadius, rectBLTR[1][1]);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRectRounded"); }

	error = HPDF_Page_LineTo(page, rectBLTR[1][0] - cornerRadius, rectBLTR[1][1]);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRectRounded"); }
	error = HPDF_Page_Arc(page, rectBLTR[1][0] - cornerRadius, rectBLTR[1][1] - cornerRadius, cornerRadius, 0.0, 90.0);
	if (error != HPDF_OK) { err("Arc-jpdf.drawRectRounded"); }
	error = HPDF_Page_LineTo(page, rectBLTR[1][0], rectBLTR[0][1] + cornerRadius);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRectRounded"); }
	error = HPDF_Page_Arc(page, rectBLTR[1][0] - cornerRadius, rectBLTR[0][1] + cornerRadius, cornerRadius, 90.0, 180.0);
	if (error != HPDF_OK) { err("Arc-jpdf.drawRectRounded"); }
	error = HPDF_Page_LineTo(page, rectBLTR[0][0] + cornerRadius, rectBLTR[0][1]);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRectRounded"); }
	error = HPDF_Page_Arc(page, rectBLTR[0][0] + cornerRadius, rectBLTR[0][1] + cornerRadius, cornerRadius, 180.0, 270.0);
	if (error != HPDF_OK) { err("Arc-jpdf.drawRectRounded"); }
	error = HPDF_Page_LineTo(page, rectBLTR[0][0], rectBLTR[1][1] - cornerRadius);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRectRounded"); }
	error = HPDF_Page_Arc(page, rectBLTR[0][0] + cornerRadius, rectBLTR[1][1] - cornerRadius, cornerRadius, 270.0, 360.0);
	if (error != HPDF_OK) { err("Arc-jpdf.drawRectRounded"); }

	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { err("Stroke-jpdf.drawRectRounded"); }
}
void JPDFTABLE::drawRectRounded(vector<vector<double>> rectBLTR, vector<vector<double>> vColour, double thickness, double radius)
{
	// This variant will draw the perimeter using the first colour, and fill the 
	// rectangle using the second colour. 
	double cornerRadius;
	if (radius > 0.0) { cornerRadius = radius; }
	else { cornerRadius = 2.0 * thickness; }

	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { err("SetLineWidth-jpdf.drawRectRounded"); }
	error = HPDF_Page_SetRGBStroke(page, vColour[0][0], vColour[0][1], vColour[0][2]);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdf.drawRectRounded"); }
	error = HPDF_Page_SetRGBFill(page, vColour[1][0], vColour[1][1], vColour[1][2]);
	if (error != HPDF_OK) { err("SetRGBFill-jpdf.drawRectRounded"); }
	error = HPDF_Page_MoveTo(page, rectBLTR[0][0] + cornerRadius, rectBLTR[1][1]);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRectRounded"); }

	error = HPDF_Page_LineTo(page, rectBLTR[1][0] - cornerRadius, rectBLTR[1][1]);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRectRounded"); }
	error = HPDF_Page_Arc(page, rectBLTR[1][0] - cornerRadius, rectBLTR[1][1] - cornerRadius, cornerRadius, 0.0, 90.0);
	if (error != HPDF_OK) { err("Arc-jpdf.drawRectRounded"); }
	error = HPDF_Page_LineTo(page, rectBLTR[1][0], rectBLTR[0][1] + cornerRadius);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRectRounded"); }
	error = HPDF_Page_Arc(page, rectBLTR[1][0] - cornerRadius, rectBLTR[0][1] + cornerRadius, cornerRadius, 90.0, 180.0);
	if (error != HPDF_OK) { err("Arc-jpdf.drawRectRounded"); }
	error = HPDF_Page_LineTo(page, rectBLTR[0][0] + cornerRadius, rectBLTR[0][1]);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRectRounded"); }
	error = HPDF_Page_Arc(page, rectBLTR[0][0] + cornerRadius, rectBLTR[0][1] + cornerRadius, cornerRadius, 180.0, 270.0);
	if (error != HPDF_OK) { err("Arc-jpdf.drawRectRounded"); }
	error = HPDF_Page_LineTo(page, rectBLTR[0][0], rectBLTR[1][1] - cornerRadius);
	if (error != HPDF_OK) { err("MoveTo-jpdf.drawRectRounded"); }
	error = HPDF_Page_Arc(page, rectBLTR[0][0] + cornerRadius, rectBLTR[1][1] - cornerRadius, cornerRadius, 270.0, 360.0);
	if (error != HPDF_OK) { err("Arc-jpdf.drawRectRounded"); }

	error = HPDF_Page_FillStroke(page);
	if (error != HPDF_OK) { err("FillStroke-jpdf.drawRectRounded"); }
}
vector<vector<double>> JPDFTABLE::drawTable()
{
	// Returns the table's BLTR.
	drawBackgroundColour();
	drawFrames();
	drawTitle();
	return tableBLTR;
}
void JPDFTABLE::drawText(int index)
{
	// Index refers to each cell's vsText index.
	HPDF_STATUS error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);  // Black.
	if (error != HPDF_OK) { err("SetRGBFill-jpdftable.drawText"); }
	string text;
	for (int ii = 0; ii < numCol; ii++) {
		for (int jj = 0; jj < numRow; jj++) {
			if (vvCell[jj][ii].vsText.size() > index) {
				text = vvCell[jj][ii].vsText[index];
				vvCell[jj][ii].drawCellTextPlain(page, text);
			}
		}
	}
}
void JPDFTABLE::drawTextList(int italicFreq, string separator)
{
	// Draws the prepared string list for every cell. If italicFreq > 0, then the
	// text is rendered using an italic font once per intalicFreq.
	HPDF_STATUS error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
	if (error != HPDF_OK) { err("SetRGBFill-jpdftable.drawTextList"); }
	for (int ii = 0; ii < numRow; ii++) {
		for (int jj = 0; jj < numCol; jj++) {
			vvCell[ii][jj].drawCellTextList(page, italicFreq, separator);
		}
	}
}
void JPDFTABLE::drawTitle()
{
	if (title.size() < 1) { return; }
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, fontTitle, fontSizeTitle);
	if (error != HPDF_OK) { err("SetFontAndSize-jpdftable.drawTitle"); }
	error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
	if (error != HPDF_OK) { err("SetRGBFill-jpdftable.drawTitle"); }
	HPDF_Page_BeginText(page);
	error = HPDF_Page_TextOut(page, titleBLTR[0][0] + 2.0, titleBLTR[0][1] + 5.0, title.c_str());
	if (error != HPDF_OK) { err("TextOut-jpdftable.drawTitle"); }
	error = HPDF_Page_EndText(page);
}
void JPDFTABLE::err(string message)
{
	string errorMessage = "JPDFTABLE error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void JPDFTABLE::initColour()
{
	colourList.resize(5);
	colourList[0] = { 0, 0, 0, 255 };  // Black
	colourList[1] = { 255, 255, 255, 255 };  // White
	colourList[2] = { 210, 210, 210, 255 };  // Light Grey
	colourList[3] = { 200, 200, 255, 255 };  // SelectedWeak
	colourList[4] = { 150, 150, 192, 255 };  // SelectedStrong

	colourListDouble.resize(colourList.size());
	for (int ii = 0; ii < colourListDouble.size(); ii++) {
		for (int jj = 0; jj < colourListDouble[ii].size(); jj++) {
			colourListDouble[ii][jj] = (double)colourList[ii][jj] / 255.0;
		}
	}
}
void JPDFTABLE::initTitle(string sTitle, HPDF_Font& font, double fontSize)
{
	// tableBLTR defines the entire object, while boxTLBR is for the rows/columns only;
	title = sTitle;
	fontTitle = font;
	fontSizeTitle = fontSize;
	titleBLTR.resize(2, vector<double>(2));
	titleBLTR[1] = tableBLTR[1];
	titleBLTR[0][0] = tableBLTR[0][0];
	titleBLTR[0][1] = tableBLTR[1][1] - fontSizeTitle;
	boxBLTR.resize(2, vector<double>(2));
	boxBLTR[0] = tableBLTR[0];
	boxBLTR[1][0] = tableBLTR[1][0];
	boxBLTR[1][1] = titleBLTR[0][1];
}
void JPDFTABLE::setColourBackground(vector<int> vColourIndex)
{
	// This variant will use the same colour for all columns in a row.
	int rowIndex = 0;
	for (int ii = 0; ii < numRow; ii++)
	{
		for (int jj = 0; jj < numCol; jj++)
		{
			vvCell[ii][jj].backgroundColour = colourListDouble[vColourIndex[rowIndex]];
		}
		if (rowIndex < vColourIndex.size() - 1) { rowIndex++; }
		else { rowIndex = 0; }
	}
}
void JPDFTABLE::setColourBackground(vector<vector<int>> vvColourIndex)
{
	// If a dimension of vvColourIndex is too small, it will restart from its beginning.
	int rowIndex = 0, colIndex = 0;
	for (int ii = 0; ii < numRow; ii++)
	{
		colIndex = 0;
		for (int jj = 0; jj < numCol; jj++)
		{
			vvCell[ii][jj].backgroundColour = colourListDouble[vvColourIndex[rowIndex][colIndex]];
			if (colIndex < vvColourIndex[rowIndex].size() - 1) { colIndex++; }
			else { colIndex = 0; }
		}
		if (rowIndex < vvColourIndex.size() - 1) { rowIndex++; }
		else { rowIndex = 0; }
	}
}
void JPDFTABLE::setRowCol(vector<int>& vNumLine, vector<double>& vRowHeight, int iCol)
{
	numRow = vNumLine.size();
	numCol = iCol;
	vvCell.resize(numRow, vector<JPDFCELL>());
	double cellWidth = floor((boxBLTR[1][0] - boxBLTR[0][0]) / (double)numCol);
	double segmentHeight;
	vector<vector<double>> cellBLTR(2, vector<double>(2));
	vector<vector<double>> bltr(2, vector<double>(2));
	cellBLTR[0][1] = boxBLTR[1][1];
	for (int ii = 0; ii < numRow; ii++)
	{
		cellBLTR[1][1] = cellBLTR[0][1];
		cellBLTR[0][1] = cellBLTR[1][1] - vRowHeight[ii];
		cellBLTR[1][0] = boxBLTR[0][0];
		cellBLTR[0][0] = cellBLTR[1][0] - cellWidth;
		segmentHeight = vRowHeight[ii] / (double)vNumLine[ii];
		for (int jj = 0; jj < numCol; jj++)
		{
			cellBLTR[0][0] += cellWidth;
			if (jj > 0) { cellBLTR[0][0] += 1.0; }
			cellBLTR[1][0] += cellWidth;
			vvCell[ii].push_back(JPDFCELL(cellBLTR));
			vvCell[ii][jj].font = fontTitle;
			if (fontItalic) { vvCell[ii][jj].fontItalic = fontItalic; }
			vvCell[ii][jj].rowIndex = ii;
			vvCell[ii][jj].colIndex = jj;

			bltr = cellBLTR;
			bltr[0][1] = cellBLTR[1][1] - segmentHeight + vvCell[ii][jj].padding;
			bltr[1][1] -= vvCell[ii][jj].padding;
			bltr[0][0] += vvCell[ii][jj].padding;
			bltr[1][0] -= vvCell[ii][jj].padding;
			vvCell[ii][jj].vBLTR.push_back(bltr);
		}
	}
}
