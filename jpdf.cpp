#include "jpdf.h"

void JPDFBARGRAPH::addRegionData(int indexRegion, vector<double>& regionData)
{
	// regionData values are within the interval [0.0, 1.0] of the y-axis scale.
	vRegionData[indexRegion] = regionData;
}
void JPDFBARGRAPH::addValuesX(vector<string>& vsValue, double rotationDeg)
{
	// Rotation is measured counter-clockwise from the horizontal. Pivot is BL.
	xValues = vsValue;
	vRegionData.resize(vsValue.size(), vector<double>());
	double thetaRad = rotationDeg * 3.141592 / 180.0;
	int maxIndex = -1, maxLength = 0;
	for (int ii = 0; ii < xValues.size(); ii++)
	{
		if (xValues[ii].size() > maxLength) 
		{ 
			maxLength = xValues[ii].size(); 
			maxIndex = ii;
		}
	}
	HPDF_STATUS error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdfbargraph.addDrawValuesX"); }
	error = HPDF_Page_SetFontAndSize(page, font, fontSize);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfbargraph.addDrawValuesX"); }
	float textWidth = HPDF_Page_TextWidth(page, xValues[maxIndex].c_str());
	if (textWidth == 0.0) { jf.err("TextWidth-jpdfbargraph.addDrawValuesX"); }
	double height = (double)textWidth * sin(thetaRad);
	height += (fontSize * cos(thetaRad));
	height += tickLength;
	xAxisBLTR.resize(2, vector<double>(2));
	xAxisBLTR[0] = bargraphBLTR[0];
	xAxisBLTR[1][0] = bargraphBLTR[1][0];
	xAxisBLTR[1][1] = xAxisBLTR[0][1] + height;
}
void JPDFBARGRAPH::drawAxis(vector<vector<double>> startStop, vector<double> vTick, vector<double> colour, double thickness)
{
	// Generalized function to paint the (x or y) axis line and its tick marks.
	drawLine(startStop, colour, thickness);
	vector<vector<double>> startStopTick(2, vector<double>(2));
	if (startStop[0][0] == startStop[1][0])  // Vertical axis
	{
		startStopTick[0][0] = startStop[0][0];
		if (thickness > 1.0) { startStopTick[0][0] += (0.5 * thickness); }
		startStopTick[1][0] = startStop[0][0] - tickLength;
		for (int ii = 0; ii < vTick.size(); ii++)
		{
			startStopTick[0][1] = vTick[ii];
			startStopTick[1][1] = vTick[ii];
			drawLine(startStopTick, colour, thickness);
		}
	}
	else if (startStop[0][1] == startStop[1][1])  // Horizontal axis
	{
		startStopTick[0][1] = startStop[0][1];
		if (thickness > 1.0) { startStopTick[0][1] += (0.5 * thickness); }
		startStopTick[1][1] = startStop[0][1] - tickLength;
		for (int ii = 0; ii < vTick.size(); ii++)
		{
			startStopTick[0][0] = vTick[ii];
			startStopTick[1][0] = vTick[ii];
			drawLine(startStopTick, colour, thickness);
		}
	}
	else { jf.err("Invalid axis startStop-jpdfbargraph.drawAxis"); }
}
void JPDFBARGRAPH::drawAxisX(vector<string>& vsValue, double rotationDeg)
{
	vector<vector<double>> startStop(2, vector<double>(2));
	startStop[1] = xAxisBLTR[1];
	startStop[0][0] = yAxisBLTR[1][0];
	startStop[0][1] = startStop[1][1];
	vector<double> vTickPos(vsValue.size() + 1);
	double space = (startStop[1][0] - startStop[0][0]) / (double)vsValue.size();
	for (int ii = 0; ii < vTickPos.size(); ii++)
	{
		vTickPos[ii] = startStop[0][0] + ((double)ii * space);
	}
	drawAxis(startStop, vTickPos, black, lineThickness);

	double textWidth;
	double thetaRad = rotationDeg * 3.141592 / 180.0;
	vector<double> valueBL(2), valueTR(2);
	valueTR[0] = startStop[0][0] - (0.5 * space);
	valueTR[1] = startStop[0][1] - tickLength;
	HPDF_STATUS error;
	HPDF_Page_BeginText(page);
	for (int ii = 0; ii < vsValue.size(); ii++)
	{
		valueTR[0] += space;
		textWidth = (double)HPDF_Page_TextWidth(page, vsValue[ii].c_str());
		if (textWidth == 0.0) { jf.err("TextWidth-jpdfbargraph.drawAxisX"); }
		valueBL[0] = valueTR[0] + (fontSize * sin(thetaRad)) - (textWidth * cos(thetaRad));
		valueBL[1] = valueTR[1] - (fontSize * cos(thetaRad)) - (textWidth * sin(thetaRad));
		error = HPDF_Page_SetTextMatrix(page, cos(thetaRad), sin(thetaRad), -sin(thetaRad), cos(thetaRad), valueBL[0], valueBL[1]);
		if (error != HPDF_OK) { jf.err("SetTextMatrix-jpdfbargraph.drawAxisX"); }
		error = HPDF_Page_ShowText(page, vsValue[ii].c_str());
		if (error != HPDF_OK) { jf.err("ShowText-jpdfbargraph.drawAxisX"); }
	}
	HPDF_Page_EndText(page);
}
void JPDFBARGRAPH::drawAxisY(vector<double>& minMax, string unit)
{
	// Note that minMax will be adjusted to the final axis min/max tick marks.
	sUnit = unit;
	int axisTickLines, decimalPlaces, numTicks, testTick;
	string displayUnit;
	double height = bargraphBLTR[1][1] - xAxisBLTR[1][1];
	if (unit.size() == 1) 
	{ 
		axisTickLines = 1; 
		displayUnit = "";
	}
	else 
	{ 
		axisTickLines = 2; 
		displayUnit = "(" + unit + ")";
	}
	numTicks = int(floor((height - fontSize) / (3.0 * fontSize))) + 1;
	double bandwidthTemp = (minMax[1] - minMax[0]) / (double)(numTicks - 1);

	string sNum = to_string(minMax[1]);
	size_t pos1 = sNum.find_first_of("123456789");
	size_t pos2 = sNum.find_last_of("123456789") + 1;
	size_t posDecimal = sNum.find('.', pos1);
	int sigDigTop = pos2 - pos1;
	if (posDecimal < pos2) { sigDigTop--; }
	int sigDigBot;
	if (minMax[0] != 0.0)
	{
		sNum = to_string(minMax[0]);
		pos1 = sNum.find_first_of("123456789");
		pos2 = sNum.find_last_of("123456789") + 1;
		posDecimal = sNum.find('.', pos1);
		sigDigBot = pos2 - pos1;
		if (posDecimal < pos2) { sigDigBot--; }
	}
	else { sigDigBot = 0; }
	int sigDig = max(sigDigTop, sigDigBot);
	sNum = to_string(bandwidthTemp);
	posDecimal = sNum.find('.', pos1);
	pos1 = sNum.find_first_of("123456789");
	pos2 = pos1 + sigDig;
	int diff = pos2 - posDecimal;

	double bandwidth, dVal;
	bandwidth = mf.rounding(bandwidthTemp, diff);
	if (bandwidth > bandwidthTemp)  // Rounded up, so extra space on the top.
	{
		testTick = numTicks - 2;
		for (int ii = testTick; ii > 0; ii--)
		{
			dVal = (double)ii * bandwidth;
			if (dVal > minMax[1]) { numTicks--; }
			else { break; }
		}
	}
	else if (bandwidth < bandwidthTemp)  // Rounded down, so lacking space on the top.
	{
		dVal = (double)(numTicks - 1) * bandwidth;
		while (dVal < minMax[1])
		{
			numTicks++;
			dVal = (double)(numTicks - 1) * bandwidth;
		}
	}
	
	vector<string> vsTick(numTicks);
	if (unit[0] == '%') { decimalPlaces = 1; }
	else { decimalPlaces = 0; }
	int indexMax = -1;
	int maxLength = 0;
	for (int ii = 0; ii < numTicks; ii++)
	{
		if (ii == numTicks - 1) { minMax[1] = minMax[0] + ((double)ii * bandwidth); }
		vsTick[ii] = jf.doubleToCommaString(minMax[0] + ((double)ii * bandwidth), decimalPlaces);
		if (axisTickLines == 1) { vsTick[ii] += " (" + unit + ")"; }
		if (vsTick[ii].size() > maxLength)
		{
			maxLength = vsTick[ii].size();
			indexMax = ii;
		}
	}
	double textWidth = (double)HPDF_Page_TextWidth(page, vsTick[indexMax].c_str());
	if (textWidth == 0.0) { jf.err("TextWidth-jpdfbargraph.drawAxisY"); }
	double width = textWidth + tickLength + padding;
	if (axisTickLines == 2) { width += (1.5 * fontSize); }

	yAxisBLTR.resize(2, vector<double>(2));
	yAxisBLTR[1][1] = bargraphBLTR[1][1];
	yAxisBLTR[0][1] = yAxisBLTR[1][1] - height;
	yAxisBLTR[0][0] = bargraphBLTR[0][0];
	yAxisBLTR[1][0] = yAxisBLTR[0][0] + width;

	vector<vector<double>> startStop(2, vector<double>(2));
	startStop[0][0] = yAxisBLTR[1][0];
	startStop[1][0] = startStop[0][0];
	startStop[0][1] = yAxisBLTR[0][1] + (0.5 * fontSize);
	startStop[1][1] = yAxisBLTR[1][1] - (0.5 * fontSize);
	vector<double> vTickPos(vsTick.size());
	double space = (startStop[1][1] - startStop[0][1]) / ((double)numTicks - 1.0);
	for (int ii = 0; ii < vTickPos.size(); ii++)
	{
		vTickPos[ii] = startStop[0][1] + ((double)ii * space);
	}
	drawAxis(startStop, vTickPos, black, lineThickness);

	vector<vector<double>> textBLTR(2, vector<double>(2));
	textBLTR[0][0] = yAxisBLTR[0][0];
	textBLTR[1][0] = yAxisBLTR[1][0] - tickLength - padding;
	for (int ii = 0; ii < vsTick.size(); ii++)
	{
		textBLTR[0][1] = vTickPos[ii] - (0.5 * fontSize) + padding;
		textBLTR[1][1] = textBLTR[0][1] + fontSize;
		textBox(textBLTR, vsTick[ii], "right", fontSize);
	}
	if (axisTickLines == 2)
	{
		textWidth = (double)HPDF_Page_TextWidth(page, displayUnit.c_str());
		if (textWidth == 0.0) { jf.err("TextWidth-jpdfbargraph.drawAxisY"); }
		double thetaRad = 3.141592 / 2.0;
		vector<double> unitBL(2);
		unitBL[0] = yAxisBLTR[0][0] + fontSize;
		unitBL[1] = yAxisBLTR[0][1] + (0.5 * (yAxisBLTR[1][1] - yAxisBLTR[0][1])) - (0.5 * textWidth);
		HPDF_Page_BeginText(page);
		HPDF_STATUS error = HPDF_Page_SetTextMatrix(page, cos(thetaRad), sin(thetaRad), -sin(thetaRad), cos(thetaRad), unitBL[0], unitBL[1]);
		if (error != HPDF_OK) { jf.err("SetTextMatrix-jpdfbargraph.drawAxisY"); }
		error = HPDF_Page_ShowText(page, displayUnit.c_str());
		if (error != HPDF_OK) { jf.err("ShowText-jpdfbargraph.drawAxisY"); }
		HPDF_Page_EndText(page);
	}
}
void JPDFBARGRAPH::drawData(vector<vector<double>>& seriesColour)
{
	dataBLTR.resize(2, vector<double>(2));
	dataBLTR[1] = bargraphBLTR[1];
	dataBLTR[0][0] = yAxisBLTR[1][0];
	dataBLTR[0][1] = xAxisBLTR[1][1];
	int numRegions = vRegionData.size();
	double space = (dataBLTR[1][0] - dataBLTR[0][0]) / (double)numRegions;
	int numSeries = seriesColour.size();
	double barWidth = (space - (2.0 * padding)) / (double)numSeries;
	double yAxisHeight = yAxisBLTR[1][1] - yAxisBLTR[0][1] - fontSize;
	double stub = 0.5 * fontSize;

	double barHeight;
	vector<double> barBL(2);
	barBL[1] = dataBLTR[0][1] + (0.5 * lineThickness);
	HPDF_STATUS error = HPDF_Page_SetRGBStroke(page, black[0], black[1], black[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdfbargraph.drawData"); }
	error = HPDF_Page_SetLineWidth(page, 1.0);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdfbargraph.drawData"); }
	for (int ii = 0; ii < numRegions; ii++)
	{
		barBL[0] = dataBLTR[0][0] + ((double)ii * space) + padding;
		for (int jj = 0; jj < numSeries; jj++)
		{
			barHeight = (vRegionData[ii][jj] * yAxisHeight) + stub;
			error = HPDF_Page_SetRGBFill(page, seriesColour[jj][0], seriesColour[jj][1], seriesColour[jj][2]);
			if (error != HPDF_OK) { jf.err("SetRGBFill-jpdfbargraph.drawData"); }
			error = HPDF_Page_Rectangle(page, barBL[0], barBL[1], barWidth, barHeight);
			if (error != HPDF_OK) { jf.err("Rectangle-jpdfbargraph.drawData"); }
			error = HPDF_Page_FillStroke(page);
			if (error != HPDF_OK) { jf.err("FillStroke-jpdfbargraph.drawData"); }
			barBL[0] += barWidth;
		}
	}

}
void JPDFBARGRAPH::drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness)
{
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawLine"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawLine"); }
	error = HPDF_Page_MoveTo(page, startStop[0][0], startStop[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawLine"); }
	error = HPDF_Page_LineTo(page, startStop[1][0], startStop[1][1]);
	if (error != HPDF_OK) { jf.err("LineTo-jpdf.drawLine"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawLine"); }
}
void JPDFBARGRAPH::textBox(vector<vector<double>> boxBLTR, string sText, string alignment, int fontsize)
{
	// Accepted alignment strings are "left", "right", "center", "justify".
	HPDF_STATUS error;
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

void JPDFCELL::drawCellBar(HPDF_Page& page, double barWidth, JFUNC& jf)
{
	// Coloured bars take the full cell height, rather than the vBLTR/text height.
	HPDF_STATUS error;
	int index = vBLTR.size() - 1;
	double xCoord = vBLTR[index][0][0];
	double yCoord = cellBLTR[0][1] + padding;
	double barHeight = cellBLTR[1][1] - padding - yCoord;
	for (int ii = 0; ii < vBarColour.size(); ii++)
	{
		error = HPDF_Page_SetRGBFill(page, vBarColour[ii][0], vBarColour[ii][1], vBarColour[ii][2]);
		if (error != HPDF_OK) { jf.err("SetRGBFill-jpdfcell.drawCellBar"); }
		error = HPDF_Page_Rectangle(page, xCoord, yCoord, barWidth, barHeight);
		if (error != HPDF_OK) { jf.err("Rectangle-jpdfcell.drawCellBar"); }
		error = HPDF_Page_Fill(page);
		if (error != HPDF_OK) { jf.err("FillStroke-jpdfcell.drawCellBar"); }
		xCoord += barWidth;
	}
	vector<vector<double>> newBLTR = vBLTR[index];
	vBLTR[index][1][0] = xCoord;
	xCoord += padding;
	newBLTR[0][0] = xCoord;
	vBLTR.push_back(newBLTR);
}
void JPDFCELL::drawCellText(HPDF_Page& page, string text, JFUNC& jf)
{
	string piece1, piece2;
	bool inPieces = 0;
	int index = vBLTR.size() - 1;
	double fontHeight = vBLTR[index][1][1] - vBLTR[index][0][1];
	float availableWidth = vBLTR[index][1][0] - vBLTR[index][0][0];

	float textWidth = HPDF_Page_TextWidth(page, text.c_str());
	if (textWidth == 0.0) { jf.err("TextWidth-jpdfcell.drawCellText"); }
	else if (availableWidth < textWidth)
	{
		if (vBLTR[index][0][1] - cellBLTR[0][1] > fontHeight)  // If enough space is available for another line...
		{
			inPieces = 1;
			size_t pos1 = text.size();
			while (availableWidth < textWidth)
			{
				pos1 = text.rfind(' ', pos1 - 1);
				if (pos1 > text.size()) { break; }
				piece1 = text.substr(0, pos1);
				textWidth = HPDF_Page_TextWidth(page, piece1.c_str());
				if (textWidth == 0.0) { jf.err("TextWidth-jpdfcell.drawCellText"); }
			}
			if (pos1 < text.size()) { piece2 = text.substr(pos1 + 1); }
			else
			{
				piece1 = "";
				piece2 = text;
			}			
		}
		else { jf.err("Insufficient space for text-jpdfcell.drawCellText"); }
	}

	HPDF_STATUS error = HPDF_Page_BeginText(page);
	if (error != HPDF_OK) { jf.err("BeginText-jpdfcell.drawCellText"); }
	double bottomPadding = (vBLTR[index][1][1] - vBLTR[index][0][1] - fontHeight);
	if (!inPieces)
	{
		error = HPDF_Page_TextRect(page, vBLTR[index][0][0], vBLTR[index][1][1], vBLTR[index][1][0], vBLTR[index][0][1], text.c_str(), HPDF_TALIGN_LEFT, NULL);
		if (error != HPDF_OK) { jf.err("TextOut-jpdfcell.drawCellText"); }
	}
	else
	{
		error = HPDF_Page_TextRect(page, vBLTR[index][0][0], vBLTR[index][1][1], vBLTR[index][1][0], vBLTR[index][0][1], piece1.c_str(), HPDF_TALIGN_LEFT, NULL);
		if (error != HPDF_OK) { jf.err("TextOut-jpdfcell.drawCellText"); }
		vector<vector<double>> newLineBLTR(2, vector<double>(2));
		if (splitPos.size() > 0)
		{
			newLineBLTR[0][0] = splitPos[splitPos.size() - 1] + padding;
		}
		else { newLineBLTR[0][0] = cellBLTR[0][0] + padding; }
		newLineBLTR[1][0] = cellBLTR[1][0] - padding;
		newLineBLTR[0][1] = vBLTR[index][0][1] - fontHeight - (2.0 * padding);
		newLineBLTR[1][1] = vBLTR[index][1][1] - fontHeight - (2.0 * padding);
		vBLTR.push_back(newLineBLTR);
		index++;
		error = HPDF_Page_TextRect(page, vBLTR[index][0][0], vBLTR[index][1][1], vBLTR[index][1][0], vBLTR[index][0][1], piece2.c_str(), HPDF_TALIGN_LEFT, NULL);
		if (error != HPDF_OK) { jf.err("TextOut-jpdfcell.drawCellText"); }
		textWidth = HPDF_Page_TextWidth(page, piece2.c_str());
		if (textWidth == 0.0) { jf.err("TextWidth-jpdfcell.drawCellText"); }
	}
	error = HPDF_Page_EndText(page);
	if (error != HPDF_OK) { jf.err("EndText-jpdfcell.drawCellText"); }

	vector<vector<double>> newBLTR = vBLTR[index];
	vBLTR[index][1][0] = vBLTR[index][0][0] + textWidth;
	newBLTR[0][0] += textWidth;
	vBLTR.push_back(newBLTR);
}
void JPDFCELL::drawCellTextItalic(HPDF_Page& page, int indexText, JFUNC& jf)
{
	if (vsText.size() <= indexText) { jf.err("Text not initialized-jpdfcell.drawCellTextItalic"); }
	int index = vBLTR.size() - 1;
	float fontHeight = vBLTR[index][1][1] - vBLTR[index][0][1];
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, fontItalic, fontHeight);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfcell.drawCellTextItalic"); }
	drawCellText(page, vsText[indexText], jf);
}
void JPDFCELL::drawCellTextItalic(HPDF_Page& page, string text, JFUNC& jf)
{
	int index = vBLTR.size() - 1;
	float fontHeight = vBLTR[index][1][1] - vBLTR[index][0][1];
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, fontItalic, fontHeight);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfcell.drawCellTextItalic"); }
	drawCellText(page, text, jf);
}
void JPDFCELL::drawCellTextPlain(HPDF_Page& page, int indexText, JFUNC& jf)
{
	if (vsText.size() <= indexText) { jf.err("Text not initialized-jpdfcell.drawCellTextPlain"); }
	int index = vBLTR.size() - 1;
	float fontHeight = vBLTR[index][1][1] - vBLTR[index][0][1];
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, fontHeight);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfcell.drawCellTextPlain"); }
	drawCellText(page, vsText[indexText], jf);
}
void JPDFCELL::drawCellTextPlain(HPDF_Page& page, string text, JFUNC& jf)
{
	int index = vBLTR.size() - 1;
	float fontHeight = vBLTR[index][1][1] - vBLTR[index][0][1];
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, fontHeight);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfcell.drawCellTextPlain"); }
	drawCellText(page, text, jf);
}
double JPDFCELL::getMaxFontHeight()
{
	int index = vBLTR.size() - 1;
	if (index < 0) { return -1.0; }
	double maxHeight = vBLTR[index][1][1] - vBLTR[index][0][1] - (2.0 * padding);
	return maxHeight;
}

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
			if (error != HPDF_OK) { jf.err("SetRGBFill-jpdftable.drawBackgroundColour"); }
			error = HPDF_Page_Rectangle(page, vvCell[ii][jj].cellBLTR[0][0], vvCell[ii][jj].cellBLTR[0][1], width, height);
			if (error != HPDF_OK) { jf.err("Rectangle-jpdftable.drawBackgroundColour"); }
			error = HPDF_Page_Fill(page);
			if (error != HPDF_OK) { jf.err("Fill-jpdftable.drawBackgroundColour"); }
		}
	}
}
void JPDFTABLE::drawColourBars(int barWidth)
{
	// Draw every cell's pre-loaded list of colours, as rectangles of cell height and given width.
	double dWidth = (double)barWidth;
	for (int ii = 0; ii < numCol; ii++)
	{
		for (int jj = 0; jj < numRow; jj++)
		{
			vvCell[jj][ii].drawCellBar(page, dWidth, jf);
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
	frameBLTR[0][1] -= (0.5 * thickness);
	drawRect(frameBLTR, colourListDouble[0], thickness);

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
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdftable.drawLine"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdftable.drawLine"); }
	error = HPDF_Page_MoveTo(page, startStop[0][0], startStop[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdftable.drawLine"); }
	error = HPDF_Page_LineTo(page, startStop[1][0], startStop[1][1]);
	if (error != HPDF_OK) { jf.err("LineTo-jpdftable.drawLine"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdftable.drawLine"); }
}
void JPDFTABLE::drawRect(vector<vector<double>> rectBLTR, vector<double> colour, double thickness)
{
	// This variant will only draw the perimeter.
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdftable.drawRect"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdftable.drawRect"); }
	error = HPDF_Page_MoveTo(page, rectBLTR[0][0], rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdftable.drawRect"); }
	error = HPDF_Page_Rectangle(page, rectBLTR[0][0], rectBLTR[0][1], rectBLTR[1][0] - rectBLTR[0][0], rectBLTR[1][1] - rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("Rectangle-jpdftable.drawRect"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawRect"); }
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
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdftable.drawValues"); }
	for (int ii = 0; ii < numCol; ii++)
	{
		for (int jj = 0; jj < numRow; jj++)
		{
			vvCell[jj][ii].drawCellTextPlain(page, index, jf);
		}
	}
}
void JPDFTABLE::drawTextListItalic(int italicFreq)
{
	// Draws the prepared string list for every cell, using an italic font once per intalicFreq.
	int indexSegment, numSegment;
	HPDF_STATUS error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdftable.drawTextListItalic"); }
	for (int ii = 0; ii < numRow; ii++)
	{
		for (int jj = 0; jj < numCol; jj++)
		{
			indexSegment = 0;
			numSegment = vvCell[ii][jj].vsText.size();
			for (int kk = 0; kk < numSegment; kk++)
			{
				if (kk > 0) { vvCell[ii][jj].drawCellTextPlain(page, " | ", jf); }
				if (kk % italicFreq == italicFreq - 1)
				{
					vvCell[ii][jj].drawCellTextItalic(page, kk, jf);
				}
				else { vvCell[ii][jj].drawCellTextPlain(page, kk, jf); }
			}
		}
	}
}
void JPDFTABLE::drawTitle()
{
	if (title.size() < 1) { return; }
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, fontTitle, fontSizeTitle);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdftable.drawTitle"); }
	error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdftable.drawTitle"); }
	HPDF_Page_BeginText(page);
	error = HPDF_Page_TextOut(page, titleBLTR[0][0] + 2.0, titleBLTR[0][1] + 3.0, title.c_str());
	if (error != HPDF_OK) { jf.err("TextOut-jpdftable.drawTitle"); }
	error = HPDF_Page_EndText(page);
}
void JPDFTABLE::initTitle(string sTitle, HPDF_Font& font, int fontSize)
{
	// tableBLTR defines the entire object, while boxTLBR is for the rows/columns only;
	title = sTitle;
	fontTitle = font;
	fontSizeTitle = fontSize;
	titleBLTR.resize(2, vector<double>(2));
	titleBLTR[1] = tableBLTR[1];
	titleBLTR[0][0] = tableBLTR[0][0];
	titleBLTR[0][1] = tableBLTR[1][1] - (fontSizeTitle + 4.0);
	boxBLTR.resize(2, vector<double>(2));
	boxBLTR[0] = tableBLTR[0];
	boxBLTR[1][0] = tableBLTR[1][0];
	boxBLTR[1][1] = titleBLTR[0][1];
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

int JPDF::addBarGraph(vector<vector<double>>& bargraphBLTR)
{
	// Return the new bar graph's index within the vector.
	int index = vBarGraph.size();
	if (fontAdded) { vBarGraph.push_back(JPDFBARGRAPH(page, fontAdded, bargraphBLTR)); }
	else { vBarGraph.push_back(JPDFBARGRAPH(page, fontDefault, bargraphBLTR)); }
	return index;
}
int JPDF::addTable(int numCol, vector<int> vNumLine, vector<double> vRowHeight, string title, double fontSizeTitle)
{
	// Return the new table's index within the vector.
	if (vNumLine.size() != vRowHeight.size()) { jf.err("Size mismatch-jpdf.addTable"); }
	int indexTable = vTable.size();
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
	if (fontAdded) { vTable.push_back(JPDFTABLE(page, tableBLTR, title, fontAdded, fontSizeTitle)); }
	else { vTable.push_back(JPDFTABLE(page, tableBLTR, title, fontDefault, fontSizeTitle)); }
	if (fontAddedItalic) { vTable[indexTable].fontItalic = fontAddedItalic; }
	vTable[indexTable].setRowCol(vNumLine, vRowHeight, numCol);
	vTable[indexTable].colourListDouble = colourListDouble;
	return indexTable;
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
void JPDF::drawCircle(vector<double> coordCenter, double radius, vector<double> colour, double thickness)
{
	// This variant will only draw the perimeter.
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawCircle"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawCircle"); }
	error = HPDF_Page_MoveTo(page, coordCenter[0], coordCenter[1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawCircle"); }
	error = HPDF_Page_Circle(page, coordCenter[0], coordCenter[1], radius);
	if (error != HPDF_OK) { jf.err("Circle-jpdf.drawCircle"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawCircle"); }
}
void JPDF::drawCircle(vector<double> coordCenter, double radius, vector<vector<double>> vColour, double thickness)
{
	// This variant will draw the perimeter using the first colour, and fill the 
	// circle using the second colour. 
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawCircle"); }
	error = HPDF_Page_SetRGBStroke(page, vColour[0][0], vColour[0][1], vColour[0][2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawCircle"); }
	error = HPDF_Page_SetRGBFill(page, vColour[1][0], vColour[1][1], vColour[1][2]);
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.drawCircle"); }
	error = HPDF_Page_MoveTo(page, coordCenter[0], coordCenter[1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawCircle"); }
	error = HPDF_Page_Circle(page, coordCenter[0], coordCenter[1], radius);
	if (error != HPDF_OK) { jf.err("Circle-jpdf.drawCircle"); }
	error = HPDF_Page_FillStroke(page);
	if (error != HPDF_OK) { jf.err("FillStroke-jpdf.drawCircle"); }
}
void JPDF::drawGradientH(vector<float> heightStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos)
{
	if (vColour.size() != vColourPos.size()) { jf.err("Size mismatch-jpdf.drawGradientH"); }
	int colourIndex = -1;
	int xPos = vColourPos[0];
	int xStop = vColourPos[vColourPos.size() - 1];
	int frontier = vColourPos[colourIndex + 1];
	int xInc = 1;
	if (xStop < xPos) { xInc = -1; }
	float dR = 0.0, dG = 0.0, dB = 0.0;
	float r = (float)vColour[0][0];
	float g = (float)vColour[0][1];
	float b = (float)vColour[0][2];
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, 1.0);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawGradientH"); }

	while (xPos != xStop)
	{
		if (xPos >= frontier)
		{
			colourIndex++;
			frontier = vColourPos[colourIndex + 1];
			dR = (float)((vColour[colourIndex + 1][0] - vColour[colourIndex][0]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
			dG = (float)((vColour[colourIndex + 1][1] - vColour[colourIndex][1]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
			dB = (float)((vColour[colourIndex + 1][2] - vColour[colourIndex][2]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
		}

		r += dR;
		if (r > 1.0) { r = 1.0; }
		if (r < 0.0) { r = 0.0; }
		g += dG;
		if (g > 1.0) { g = 1.0; }
		if (g < 0.0) { g = 0.0; }
		b += dB;
		if (b > 1.0) { b = 1.0; }
		if (b < 0.0) { b = 0.0; }

		error = HPDF_Page_SetRGBStroke(page, r, g, b);
		if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawGradientH"); }
		error = HPDF_Page_MoveTo(page, (float)xPos, heightStartStop[0]);
		if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientH"); }
		error = HPDF_Page_LineTo(page, (float)xPos, heightStartStop[1]);
		if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientH"); }
		error = HPDF_Page_Stroke(page);
		if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawGradientH"); }

		xPos += xInc;
	}
	error = HPDF_Page_SetRGBStroke(page, r, g, b);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawGradientH"); }
	error = HPDF_Page_MoveTo(page, (float)xPos, heightStartStop[0]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientH"); }
	error = HPDF_Page_LineTo(page, (float)xPos, heightStartStop[1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientH"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawGradientH"); }

}
void JPDF::drawGradientV(vector<float> widthStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos)
{
	if (vColour.size() != vColourPos.size()) { jf.err("Size mismatch-jpdf.drawGradientV"); }
	int colourIndex = -1;
	int yPos = vColourPos[0];
	int yStop = vColourPos[vColourPos.size() - 1];
	int frontier = vColourPos[colourIndex + 1];
	int yInc = 1;
	if (yStop < yPos) { yInc = -1; }
	float dR, dG, dB;
	float r = (float)vColour[0][0];
	float g = (float)vColour[0][1];
	float b = (float)vColour[0][2];
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, 1.0);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawGradientV"); }

	while (yPos != yStop)
	{
		if (yPos >= frontier)
		{
			colourIndex++;
			frontier = vColourPos[colourIndex + 1];
			dR = (float)((vColour[colourIndex + 1][0] - vColour[colourIndex][0]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
			dG = (float)((vColour[colourIndex + 1][1] - vColour[colourIndex][1]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
			dB = (float)((vColour[colourIndex + 1][2] - vColour[colourIndex][2]) / (vColourPos[colourIndex + 1] - vColourPos[colourIndex]));
		}

		r += dR;
		if (r > 1.0) { r = 1.0; }
		if (r < 0.0) { r = 0.0; }
		g += dG;
		if (g > 1.0) { g = 1.0; }
		if (g < 0.0) { g = 0.0; }
		b += dB;
		if (b > 1.0) { b = 1.0; }
		if (b < 0.0) { b = 0.0; }

		error = HPDF_Page_SetRGBStroke(page, r, g, b);
		if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawGradientV"); }
		error = HPDF_Page_MoveTo(page, widthStartStop[0], (float)yPos);
		if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientV"); }
		error = HPDF_Page_LineTo(page, widthStartStop[1], (float)yPos);
		if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientV"); }
		error = HPDF_Page_Stroke(page);
		if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawGradientV"); }

		yPos += yInc;
	}
	error = HPDF_Page_SetRGBStroke(page, r, g, b);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawGradientV"); }
	error = HPDF_Page_MoveTo(page, widthStartStop[0], (float)yPos);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientV"); }
	error = HPDF_Page_LineTo(page, widthStartStop[1], (float)yPos);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawGradientV"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawGradientV"); }

}
void JPDF::drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness)
{
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawLine"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawLine"); }
	error = HPDF_Page_MoveTo(page, startStop[0][0], startStop[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawLine"); }
	error = HPDF_Page_LineTo(page, startStop[1][0], startStop[1][1]);
	if (error != HPDF_OK) { jf.err("LineTo-jpdf.drawLine"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawLine"); }
}
void JPDF::drawRect(vector<vector<double>> rectBLTR, vector<double> colour, double thickness)
{
	// This variant will only draw the perimeter.
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawRect"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawRect"); }
	error = HPDF_Page_MoveTo(page, rectBLTR[0][0], rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawRect"); }
	error = HPDF_Page_Rectangle(page, rectBLTR[0][0], rectBLTR[0][1], rectBLTR[1][0] - rectBLTR[0][0], rectBLTR[1][1] - rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("Rectangle-jpdf.drawRect"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { jf.err("Stroke-jpdf.drawRect"); }
}
void JPDF::drawRect(vector<vector<double>> rectBLTR, vector<vector<double>> vColour, double thickness)
{
	// This variant will draw the perimeter using the first colour, and fill the 
	// rectangle using the second colour. 
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawRect"); }
	error = HPDF_Page_SetRGBStroke(page, vColour[0][0], vColour[0][1], vColour[0][2]);
	if (error != HPDF_OK) { jf.err("SetRGBStroke-jpdf.drawRect"); }
	error = HPDF_Page_SetRGBFill(page, vColour[1][0], vColour[1][1], vColour[1][2]);
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.drawRect"); }
	error = HPDF_Page_MoveTo(page, rectBLTR[0][0], rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawRect"); }
	error = HPDF_Page_Rectangle(page, rectBLTR[0][0], rectBLTR[0][1], rectBLTR[1][0] - rectBLTR[0][0], rectBLTR[1][1] - rectBLTR[0][1]);
	if (error != HPDF_OK) { jf.err("Rectangle-jpdf.drawRect"); }
	error = HPDF_Page_FillStroke(page);
	if (error != HPDF_OK) { jf.err("FillStroke-jpdf.drawRect"); }
}
void JPDF::drawRegion(vector<vector<double>>& vvdPath, vector<double> colour)
{
	// This variant will fill the inside of the path without drawing a border.
	HPDF_STATUS error = HPDF_Page_SetRGBFill(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.drawRegion"); }
	error = HPDF_Page_SetLineWidth(page, 0.0);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.drawRegion"); }
	error = HPDF_Page_MoveTo(page, vvdPath[0][0], vvdPath[0][1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.drawRegion"); }
	for (int ii = 1; ii < vvdPath.size(); ii++)
	{
		error = HPDF_Page_LineTo(page, vvdPath[ii][0], vvdPath[ii][1]);
		if (error != HPDF_OK) { jf.err("LineTo-jpdf.drawRegion"); }
	}
	error = HPDF_Page_ClosePathFillStroke(page);
	if (error != HPDF_OK) { jf.err("ClosePath-jpdf.drawRegion"); }
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
	initColour();

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
void JPDF::initColour()
{
	colourList.resize(5);
	colourList[0] = { 0, 0, 0, 255 };  // Black
	colourList[1] = { 255, 255, 255, 255 };  // White
	colourList[2] = { 210, 210, 210, 255 };  // Light Grey
	colourList[3] = { 200, 200, 255, 255 };  // SelectedWeak
	colourList[4] = { 150, 150, 192, 255 };  // SelectedStrong

	colourListDouble.resize(colourList.size());
	for (int ii = 0; ii < colourListDouble.size(); ii++)
	{
		colourListDouble[ii] = jf.rgbxToDouble(colourList[ii]);
	}
}
void JPDF::parameterSectionBottom(vector<string>& vsParameter, vector<int>& vColour)
{
	// Will print the parameter list at the bottom of the active page. 
	// At the end, the cursor value is set to the bottom-left corner of the page's 
	// remaining usable space. 

	// Load fonts and prepare the page.
	HPDF_UINT maxChars;
	HPDF_Font font, fontItalic;
	if (fontAdded) { font = fontAdded; }
	else { font = fontDefault; }
	if (fontAddedItalic) { fontItalic = fontAddedItalic; }
	else { fontItalic = NULL; }
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, fontSize);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.appendParameterSection"); }

	// Break the parameter list into lines of text, and determine the total height.
	vector<vector<string>> vvsParameter;
	float textWidth = HPDF_Page_GetWidth(page) - (2.0 * margin) - 4.0;
	float textHeight = breakListFitWidth(vsParameter, textWidth, vvsParameter);

	// Write the list of parameters, assigning background colours from vColour.
	vector<string> vsTemp;
	vector<double> vRGBA;
	size_t pos1, pos2;
	int index, italic, numLines;
	float parameterHeight;
	cursor[1] = margin + 2.0;  // Bottom of text box.
	error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);  // Black.
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.appendParameterSection"); }
	HPDF_Page_BeginText(page);
	for (int ii = vvsParameter.size() - 1; ii >= 0; ii--)
	{
		// If the assigned background colour is not white, colour it.
		cursor[0] = margin;
		numLines = vvsParameter[ii].size();
		if (vColour[ii] != 1)
		{
			HPDF_Page_EndText(page);
			parameterHeight = (float)numLines * lineHeight;
			vRGBA = colourListDouble[vColour[ii]];
			error = HPDF_Page_SetRGBFill(page, vRGBA[0], vRGBA[1], vRGBA[2]);
			if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.appendParameterSection"); }
			error = HPDF_Page_MoveTo(page, cursor[0], cursor[1]);
			if (error != HPDF_OK) { jf.err("MoveTo-jpdf.appendParameterSection"); }
			error = HPDF_Page_Rectangle(page, cursor[0], cursor[1], textWidth + 3.0, parameterHeight);
			if (error != HPDF_OK) { jf.err("Rectangle-jpdf.appendParameterSection"); }
			error = HPDF_Page_Fill(page);
			if (error != HPDF_OK) { jf.err("Fill-jpdf.appendParameterSection"); }
			error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);  // Black, for text.
			if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.appendParameterSection"); }
			HPDF_Page_BeginText(page);
		}

		// If italics are available, apply them to odd-numbered segments. Then, print the text.
		string temp, segmentor = " | ";
		if (fontItalic)
		{
			float segWidth = HPDF_Page_TextWidth(page, segmentor.c_str());
			cursor[1] += (float)numLines * lineHeight;
			italic = 0;
			for (int jj = 0; jj < vvsParameter[ii].size(); jj++)
			{
				cursor[0] = margin + 2.0;
				cursor[1] -= lineHeight;
				pos1 = vvsParameter[ii][jj].find_first_not_of(" |");
				pos2 = vvsParameter[ii][jj].find('|', pos1) - 1;
				while (pos2 < vvsParameter[ii][jj].size())
				{
					temp = vvsParameter[ii][jj].substr(pos1, pos2 - pos1);
					if (italic) { error = HPDF_Page_SetFontAndSize(page, fontItalic, fontSize); }
					else { error = HPDF_Page_SetFontAndSize(page, font, fontSize); }
					if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.appendParameterSection"); }

					error = HPDF_Page_TextOut(page, cursor[0], cursor[1] + 4.0, temp.c_str());
					if (error != HPDF_OK) { jf.err("TextOut-jpdf.appendParameterSection"); }
					cursor[0] += HPDF_Page_TextWidth(page, temp.c_str());

					if (italic)
					{
						error = HPDF_Page_SetFontAndSize(page, font, fontSize);
						if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.appendParameterSection"); }
						italic = 0;
					}
					else { italic = 1; }
					error = HPDF_Page_TextOut(page, cursor[0], cursor[1] + 4.0, segmentor.c_str());
					if (error != HPDF_OK) { jf.err("TextOut-jpdf.appendParameterSection"); }
					cursor[0] += segWidth;

					pos1 = vvsParameter[ii][jj].find_first_not_of(" |", pos2 + 1);
					if (pos1 > vvsParameter[ii][jj].size()) { break; }
					pos2 = vvsParameter[ii][jj].find('|', pos1) - 1;
				}
				if (pos1 < pos2)  // This line ends with a segment.
				{
					temp = vvsParameter[ii][jj].substr(pos1);
					if (italic) { error = HPDF_Page_SetFontAndSize(page, fontItalic, fontSize); }
					else { error = HPDF_Page_SetFontAndSize(page, font, fontSize); }
					if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.appendParameterSection"); }

					error = HPDF_Page_TextOut(page, cursor[0], cursor[1] + 4.0, temp.c_str());
					if (error != HPDF_OK) { jf.err("TextOut-jpdf.appendParameterSection"); }
				}
				else {}  // This line ends with a segmentor.
			}
			cursor[1] += (float)numLines * lineHeight;
		}
		else
		{
			cursor[1] += (float)numLines * lineHeight;
			for (int jj = 0; jj < vvsParameter[ii].size(); jj++)
			{
				cursor[0] = margin + 2.0;
				cursor[1] -= lineHeight;
				error = HPDF_Page_TextOut(page, cursor[0], cursor[1] + 4.0, vvsParameter[ii][jj].c_str());
				if (error != HPDF_OK) { jf.err("TextOut-jpdf.appendParameterSection"); }
			}
			cursor[1] += (float)numLines * lineHeight;
		}
	}
	HPDF_Page_EndText(page);

	// Draw a rectangle around the "parameter" section.
	cursor[0] = margin;
	cursor[1] = margin;
	error = HPDF_Page_SetLineWidth(page, 3.0);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.appendParameterSection"); }
	error = HPDF_Page_MoveTo(page, cursor[0], cursor[1]);
	if (error != HPDF_OK) { jf.err("MoveTo-jpdf.appendParameterSection"); }
	error = HPDF_Page_Rectangle(page, cursor[0], cursor[1], HPDF_Page_GetWidth(page) - (float)(2 * margin), textHeight + 2.0);
	if (error != HPDF_OK) { jf.err("Rectangle-jpdf.appendParameterSection"); }
	error = HPDF_Page_ClosePathStroke(page);
	if (error != HPDF_OK) { jf.err("ClosePathStroke-jpdf.appendParameterSection"); }

	// Write the box title.
	cursor[1] = margin + textHeight + 4.0;
	error = HPDF_Page_SetFontAndSize(page, font, fontSize + 2);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.appendParameterSection"); }
	error = HPDF_Page_BeginText(page);
	error = HPDF_Page_TextOut(page, cursor[0], cursor[1], "Parameters");
	if (error != HPDF_OK) { jf.err("TextOut-jpdf.appendParameterSection"); }
	error = HPDF_Page_EndText(page);

	// Prepare the cursor for the next section.
	cursor[1] += (float)fontSize + 8.0;
}
void JPDF::sectionListBoxed(string sTitle, vector<string>& vsList)
{
	vector<int> vColour;
	vColour.assign(vsList.size(), 1);  // White background.
	sectionListBoxed(sTitle, vsList, vColour, 1);
}
void JPDF::sectionListBoxed(string sTitle, vector<string>& vsList, vector<int>& vColour)
{
	sectionListBoxed(sTitle, vsList, vColour, 1);
}
void JPDF::sectionListBoxed(string sTitle, vector<string>& vsList, vector<int>& vColour, int numCol)
{
	// Background colours will be applied section-wide, by cycling through the provided list.
	int indexText, myFontSize;
	int numCell = vsList.size();
	HPDF_UINT printed;
	HPDF_Font font;
	if (fontAdded) { font = fontAdded; }
	else { font = fontDefault; }

	float rowWidth = HPDF_Page_GetWidth(page) - (2.0 * margin);
	float colWidth = rowWidth / (float)numCol;

	int numRow = numCell / numCol;
	if (numCell % numCol > 0) { numRow++; }
	float sectionHeight = (float)numRow * lineHeight;
	cursor[1] += sectionHeight;

	// Write the list from top->bottom (for all columns), and assigning background colours.
	float textWidth;
	vector<double> vRGBA;
	int indexColour = 0;
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, fontSize);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.sectionListBoxed"); }
	for (int ii = 0; ii < numRow; ii++)
	{
		cursor[0] = margin;
		cursor[1] -= lineHeight;
		if (vColour[indexColour] != 1)  // If background is non-white, paint it.
		{
			vRGBA = colourListDouble[vColour[indexColour]];
			error = HPDF_Page_SetRGBFill(page, vRGBA[0], vRGBA[1], vRGBA[2]);
			if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.sectionListBoxed"); }
			error = HPDF_Page_Rectangle(page, cursor[0], cursor[1], rowWidth, lineHeight);
			if (error != HPDF_OK) { jf.err("Rectangle-jpdf.sectionListBoxed"); }
			error = HPDF_Page_Fill(page);
			if (error != HPDF_OK) { jf.err("Fill-jpdf.sectionListBoxed"); }
			error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
			if (error != HPDF_OK) { jf.err("SetRGBFill-jpdf.sectionListBoxed"); }
		}
		HPDF_Page_BeginText(page);
		for (int jj = 0; jj < numCol; jj++)
		{
			indexText = (jj * numRow) + ii;
			if ( indexText >= numCell) { continue; }
			if (vsList[indexText].size() < 1) { continue; }

			cursor[0] = margin + ((float)jj * colWidth);
			error = HPDF_Page_TextRect(page, cursor[0] + 2.0, cursor[1] + lineHeight, cursor[0] + colWidth, cursor[1], vsList[indexText].c_str(), HPDF_TALIGN_LEFT, &printed);
			if (error != HPDF_OK)
			{
				jf.err("Insufficient width-jpdf.sectionListBoxed");
			}
			if (printed != vsList[indexText].size())
			{
				jf.err("Insufficient width-jpdf.sectionListBoxed");
			}
		}
		HPDF_Page_EndText(page);
		if (indexColour == vColour.size() - 1) { indexColour = 0; }
		else { indexColour++; }
	}

	// Draw a box frame.
	cursor[0] = margin;
	error = HPDF_Page_SetLineWidth(page, 3.0);
	if (error != HPDF_OK) { jf.err("SetLineWidth-jpdf.sectionListBoxed"); }
	error = HPDF_Page_Rectangle(page, cursor[0], cursor[1], rowWidth, sectionHeight);
	if (error != HPDF_OK) { jf.err("Rectangle-jpdf.sectionListBoxed"); }
	error = HPDF_Page_ClosePathStroke(page);
	if (error != HPDF_OK) { jf.err("ClosePathStroke-jpdf.sectionListBoxed"); }

	// Draw vertical lines between columns.
	vector<vector<double>> startStop(2, vector<double>(2));
	startStop[0][1] = cursor[1];
	startStop[1][1] = cursor[1] + sectionHeight;
	for (int ii = 1; ii < numCol; ii++)
	{
		startStop[0][0] = margin + ((float)ii * colWidth);
		startStop[1][0] = startStop[0][0];
		drawLine(startStop, colourListDouble[0], 2.0);
	}

	// Write the title.
	cursor[1] += sectionHeight;
	error = HPDF_Page_SetFontAndSize(page, font, fontSize + 2);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdf.sectionListBoxed"); }
	HPDF_Page_BeginText(page);
	error = HPDF_Page_TextOut(page, cursor[0], cursor[1] + 3.0, sTitle.c_str());
	if (error != HPDF_OK) { jf.err("TextOut-jpdf.sectionListBoxed"); }
	error = HPDF_Page_EndText(page);

	// Prepare the cursor for the next section.
	cursor[1] += (float)fontSize + 8.0;
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
