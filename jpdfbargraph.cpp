#include "jpdfbargraph.h"

void JPDFBARGRAPH::addRegionData(int indexRegion, vector<double>& regionData)
{
	// regionData values are within the interval [0.0, 1.0] of the y-axis scale.
	vRegionData[indexRegion] = regionData;
}
void JPDFBARGRAPH::addValuesX(vector<string> vsValue, double rotationDeg)
{
	// Rotation is measured counter-clockwise from the horizontal. Pivot is BL.
	xValues.assign(vsValue.begin(), vsValue.end());
	int numRegion = xValues.size();
	vRegionData.resize(numRegion, vector<double>());
	double thetaRad = rotationDeg * 3.141592 / 180.0;
	int maxIndex = -1, maxLength = 0;
	for (int ii = 0; ii < numRegion; ii++)
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
void JPDFBARGRAPH::drawAxisX(double rotationDeg)
{
	vector<vector<double>> startStop(2, vector<double>(2));
	startStop[1] = xAxisBLTR[1];
	startStop[0][0] = yAxisBLTR[1][0];
	startStop[0][1] = startStop[1][1];
	int numRegion = xValues.size();
	vector<double> vTickPos(numRegion + 1);
	double space = (startStop[1][0] - startStop[0][0]) / (double)numRegion;
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
	for (int ii = 0; ii < numRegion; ii++)
	{
		valueTR[0] += space;
		textWidth = (double)HPDF_Page_TextWidth(page, xValues[ii].c_str());
		if (textWidth == 0.0) { jf.err("TextWidth-jpdfbargraph.drawAxisX"); }
		valueBL[0] = valueTR[0] + (fontSize * sin(thetaRad)) - (textWidth * cos(thetaRad));
		valueBL[1] = valueTR[1] - (fontSize * cos(thetaRad)) - (textWidth * sin(thetaRad));
		error = HPDF_Page_SetTextMatrix(page, cos(thetaRad), sin(thetaRad), -sin(thetaRad), cos(thetaRad), valueBL[0], valueBL[1]);
		if (error != HPDF_OK) { jf.err("SetTextMatrix-jpdfbargraph.drawAxisX"); }
		error = HPDF_Page_ShowText(page, xValues[ii].c_str());
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
			dVal = minMax[0] + ((double)ii * bandwidth);
			if (dVal > minMax[1]) { numTicks--; }
			else { break; }
		}
	}
	else if (bandwidth < bandwidthTemp)  // Rounded down, so lacking space on the top.
	{
		dVal = minMax[0] + ((double)(numTicks - 1) * bandwidth);
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
