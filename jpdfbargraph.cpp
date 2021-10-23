#include "jpdfbargraph.h"

void JPDFBARGRAPH::addRegionData(int indexRegion, vector<double>& regionData)
{
	// regionData values are within the interval [0.0, 1.0] of the y-axis scale.
	vRegionData[indexRegion] = regionData;
}
vector<double> JPDFBARGRAPH::angledTRtoBL(double xTR, double yTR, double textLength, double textHeight, double thetaRad)
{
	// Returns the bottom-left coordinates of a rectangle angled about its BL corner.
	vector<double> BL(2);
	BL[0] = xTR + (textHeight * sin(thetaRad)) - (textLength * cos(thetaRad));
	BL[1] = yTR - (textHeight * cos(thetaRad)) - (textLength * sin(thetaRad));
	return BL;
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
void JPDFBARGRAPH::drawAxisX(vector<string> vsValue, double rotationDeg)
{
	// Rotation is measured counter-clockwise from the horizontal. Pivot is BL.
	xValues.assign(vsValue.begin(), vsValue.end());
	int indexVVS, numLine, numXVal = xValues.size();
	vRegionData.resize(numXVal, vector<double>());
	double thetaRad = rotationDeg * 3.141592 / 180.0;

	// Impose the left margin as a firm left boundary on angled x-axis names.
	vector<vector<string>> vvsLine(numXVal, vector<string>());  // Form [xValue index][line0, line1, ...].
	vector<double> vFontHeight(numXVal), vHeight(numXVal);
	double space = (bargraphBLTR[1][0] - yAxisBLTR[1][0]) / (double)numXVal;
	vector<vector<double>> textBLTR(2, vector<double>(2));
	textBLTR[0][0] = bargraphBLTR[0][0];
	textBLTR[1][0] = yAxisBLTR[1][0];
	textBLTR[0][1] = 0.0;  // Temporary, will be corrected later. 
	double topXMinTR = yAxisBLTR[1][0] - space;
	for (int ii = 0; ii < numXVal; ii++) {
		topXMinTR += space;
		textBLTR[1][0] += space;
		vFontHeight[ii] = fontSize;
		textBLTR[1][1] = xMaxHeightPercentage * (bargraphBLTR[1][1] - bargraphBLTR[0][1]);
		vvsLine[ii] = splitAtSpaceBox(xValues[ii], textBLTR, vFontHeight[ii], thetaRad, padding, topXMinTR);
		vHeight[ii] = textBLTR[1][1];
	}

	// Determine the x-axis' box dimensions.
	vector<int> heightMinMax = jf.minMax(vHeight);
	xAxisBLTR.resize(2, vector<double>(2));
	xAxisBLTR[0] = bargraphBLTR[0];
	xAxisBLTR[1][0] = bargraphBLTR[1][0];
	xAxisBLTR[1][1] = xAxisBLTR[0][1] + vHeight[heightMinMax[1]] + tickLength;

	// Paint the x-axis angled names, with the text aligned to TR corner.
	double lineHeight, lineLength, subspace, xBegin;
	vector<double> textTR = { 0.0, xAxisBLTR[1][1] - tickLength };
	for (int ii = 0; ii < numXVal; ii++) {
		numLine = vvsLine[ii].size();
		subspace = space / (double)(numLine + 1);
		xBegin = yAxisBLTR[1][0] + ((double)ii * space);
		lineHeight = vFontHeight[ii] + padding;
		for (int jj = 0; jj < numLine; jj++) {
			textTR[0] = xBegin + ((double)(jj + 1) * subspace);
			textBoxAngled(vvsLine[ii][jj], textTR, "TR", vFontHeight[ii], thetaRad);
		}
	}

	// Paint the tick lines.
	vector<vector<double>> startStop = xAxisBLTR;
	startStop[0][0] = yAxisBLTR[1][0];
	startStop[0][1] = xAxisBLTR[1][1];
	vector<double> vTickPos(numXVal + 1);
	for (int ii = 0; ii < vTickPos.size(); ii++) {
		vTickPos[ii] = startStop[0][0] + ((double)ii * space);
	}
	drawAxis(startStop, vTickPos, black, lineThickness);
}
void JPDFBARGRAPH::drawAxisY(vector<double>& minMax, string unit)
{
	// Assumes that the y-axis BLTR has already been defined, except for the bottom yCoord.
	sUnit = unit;
	int axisTickLines, decimalPlaces, numTicks, testTick;
	string displayUnit;
	if (unit.size() == 1) {
		axisTickLines = 1;
		displayUnit = "";
	}
	else {
		axisTickLines = 2;
		displayUnit = "(" + unit + ")";
	}
	yAxisBLTR[0][1] = xAxisBLTR[1][1];
	double height = bargraphBLTR[1][1] - xAxisBLTR[1][1] - fontSize;
	numTicks = int(floor(height / (3.0 * fontSize))) + 1;
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
	for (int ii = 0; ii < numTicks; ii++)
	{
		if (ii == numTicks - 1) { minMax[1] = minMax[0] + ((double)ii * bandwidth); }
		vsTick[ii] = jf.doubleToCommaString(minMax[0] + ((double)ii * bandwidth), decimalPlaces);
		if (axisTickLines == 1) { vsTick[ii] += " (" + unit + ")"; }
	}

	vector<vector<double>> textBLTR(2, vector<double>(2));
	textBLTR[0][0] = yAxisBLTR[0][0];
	if (axisTickLines == 2) { textBLTR[0][0] += (1.5 * fontSize); }  // For a vertically-aligned unit.
	textBLTR[1][0] = yAxisBLTR[1][0] - tickLength - padding;
	vector<vector<double>> startStop(2, vector<double>(2));
	startStop[0][0] = yAxisBLTR[1][0];
	startStop[1][0] = startStop[0][0];
	startStop[0][1] = yAxisBLTR[0][1] + (0.5 * fontSize);
	startStop[1][1] = yAxisBLTR[1][1] - (0.5 * fontSize);
	vector<double> vTickPos(numTicks);
	double space = (startStop[1][1] - startStop[0][1]) / ((double)numTicks - 1.0);
	for (int ii = 0; ii < numTicks; ii++)
	{
		vTickPos[ii] = startStop[0][1] + ((double)ii * space);
		textBLTR[0][1] = vTickPos[ii] - (0.5 * fontSize) + padding;
		textBLTR[1][1] = textBLTR[0][1] + fontSize;
		textBox(textBLTR, vsTick[ii], "right", fontSize);
	}
	drawAxis(startStop, vTickPos, black, lineThickness);

	if (axisTickLines == 2)
	{
		double textWidth = (double)HPDF_Page_TextWidth(page, displayUnit.c_str());
		if (textWidth == 0.0) { jf.err("TextWidth-jpdfbargraph.drawAxisY"); }
		double thetaRad = 3.141592 / 2.0;
		vector<double> unitBL(2);
		unitBL[0] = yAxisBLTR[0][0] + fontSize;
		unitBL[1] = yAxisBLTR[0][1] + (0.5 * height) - (0.5 * textWidth);
		textBoxAngled(displayUnit, unitBL, "BL", fontSize, thetaRad);
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
void JPDFBARGRAPH::initAxisY(vector<double>& minMax, string unit)
{
	// Estimate the width needed by the y-axis.
	double topVal = mf.roundingCeil(minMax[1]);
	int decimalPlaces;
	if (unit[0] == '%') { decimalPlaces = 1; }
	else { decimalPlaces = 0; }
	string sTop = jf.doubleToCommaString(topVal, decimalPlaces);
	if (unit.size() == 1) {
		sTop += " (" + unit + ")";
	}
	HPDF_STATUS error = HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
	if (error != HPDF_OK) { jf.err("SetRGBFill-jpdfbargraph.initAxisY"); }
	error = HPDF_Page_SetFontAndSize(page, font, fontSize);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfbargraph.initAxisY"); }
	double textWidthTop = (double)HPDF_Page_TextWidth(page, sTop.c_str());
	if (textWidthTop == 0.0) { jf.err("TextWidth-jpdfbargraph.initAxisY"); }
	if (unit.size() > 1) {
		textWidthTop += (2.0 * fontSize);  // For the vertically-aligned unit.
	}
	yAxisBLTR.resize(2, vector<double>(2));
	yAxisBLTR[0][0] = bargraphBLTR[0][0];
	yAxisBLTR[1][0] = yAxisBLTR[0][0] + textWidthTop;
	yAxisBLTR[1][1] = bargraphBLTR[1][1];
}
vector<string> JPDFBARGRAPH::splitAtSpaceBox(string tooLong, vector<vector<double>>& boxBLTR, double& fontHeight, double thetaRad, double textPadding, double topXMinTR)
{
	// Splits the string into substrings which will fit into boxBLTR at the given angle,
	// minimizing the number of lines. Will not exceed the initial font size, but will
	// reduce as necessary to fit. Returns by reference the font size used in the solution. 
	// topXMinTR is the minimum x-coordinate tolerated for the top line's TR corner.
	vector<vector<string>> vvsLine;  // Form [numLine candidate index][line0, line1, ...].
	vector<double> vFontBest, vMaxLength;
	double fontHeightBest, fontHeightMax, fontHeightTemp, maxLengthX, maxLengthY, textLength, textWidth;
	string sLine, temp;
	HPDF_STATUS error;
	size_t pos1;
	double textLongest = -1.0;
	double localTopXMinTR = topXMinTR - boxBLTR[0][0];
	vector<string> vsText = jf.splitByMarker(tooLong, ' ');
	int indexCandidate, indexLine, numLine = 0;
	bool fit, success = 0;
	while (!success) {
		numLine++;

		// Determine the largest font size which could be used with this numLine value.
		fontHeightMax = (boxBLTR[1][0] - boxBLTR[0][0]) * sin(thetaRad);
		fontHeightMax -= ((localTopXMinTR * sin(thetaRad)) + ((numLine + 1) * textPadding));
		fontHeightMax /= (double)numLine;
		fontHeightTemp = floor(min(fontHeightMax, fontHeight));
		if (vFontBest.size() > 0 && vFontBest[indexCandidate] > fontHeightTemp) {
			break;  // Success!
		}
		else {
			indexCandidate = vvsLine.size();
			vvsLine.push_back(vector<string>(numLine));
		}

		// Determine the largest font size which can fit the whole text on this numLine.
		fit = 0;
		while (!fit) {
			// Determine the final line's maximum length, constrained by X or Y. 
			vMaxLength.resize(numLine);  // Is in reverse order.
			maxLengthX = (boxBLTR[1][0] - boxBLTR[0][0]) - ((fontHeightTemp + textPadding) / sin(thetaRad));
			maxLengthX /= cos(thetaRad);
			maxLengthY = (boxBLTR[1][1] - boxBLTR[0][1]) - (fontHeightTemp * cos(thetaRad));
			maxLengthY /= sin(thetaRad);
			vMaxLength[0] = min(maxLengthX, maxLengthY);  // Use the tightest constraint.

			// Extrapolate backwards for the other lines' max lengths.
			for (int ii = 1; ii < numLine; ii++) {
				vMaxLength[ii] = vMaxLength[0] * sin(thetaRad);
				vMaxLength[ii] -= ((double)ii * ((fontHeightTemp + textPadding) / cos(thetaRad)));
				vMaxLength[ii] /= sin(thetaRad);
			}

			error = HPDF_Page_SetFontAndSize(page, font, fontHeightTemp);
			if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfbargraph.splitAtSpaceBox"); }
			sLine.clear();
			indexLine = 0;
			for (int ii = vsText.size() - 1; ii >= 0; ii--) {
				if (ii == vsText.size() - 1) { temp = vsText[ii]; }
				else { temp = vsText[ii] + " "; }
				sLine.insert(0, temp);
				textLength = (double)HPDF_Page_TextWidth(page, sLine.c_str());
				if (textLength == 0.0) { jf.err("TextWidth-jpdfbargraph.splitAtSpaceBox"); }
				if (textLength > vMaxLength[indexLine]) {
					if (indexLine < numLine - 1) {  // We have empty lines as yet untapped.
						pos1 = sLine.find(' ') + 1;
						vvsLine[indexCandidate][vvsLine[indexCandidate].size() - 1 - indexLine] = sLine.substr(pos1);
						while (temp.back() == ' ') { temp.pop_back(); }
						if (ii > 0) {  // Keep working.
							sLine = temp;
							indexLine++;
						}
						else {  // This font size fits, however it may not be ideally distributed.
							vvsLine[indexCandidate][0] = temp;
							double filledNow, filledNext;
							for (int jj = 0; jj < numLine - 1; jj++) {
								textLength = (double)HPDF_Page_TextWidth(page, vvsLine[indexCandidate][jj].c_str());
								if (textLength == 0.0) { jf.err("TextWidth-jpdfbargraph.splitAtSpaceBox"); }
								filledNow = textLength / vMaxLength[jj];
								textLength = (double)HPDF_Page_TextWidth(page, vvsLine[indexCandidate][jj + 1].c_str());
								if (textLength == 0.0) { jf.err("TextWidth-jpdfbargraph.splitAtSpaceBox"); }
								filledNext = textLength / vMaxLength[jj + 1];
								while (filledNow < filledNext) {
									pos1 = vvsLine[indexCandidate][jj + 1].find(' ');
									temp = vvsLine[indexCandidate][jj + 1].substr(0, pos1);
									vvsLine[indexCandidate][jj] += " " + temp;
									textLength = (double)HPDF_Page_TextWidth(page, vvsLine[indexCandidate][jj].c_str());
									if (textLength == 0.0) { jf.err("TextWidth-jpdfbargraph.splitAtSpaceBox"); }
									if (textLength > vMaxLength[jj]) {
										vvsLine[indexCandidate][jj].resize(vvsLine[indexCandidate][jj].size() - temp.size() - 1);
										break;
									}
									filledNow = textLength / vMaxLength[jj];
									temp = vvsLine[indexCandidate][jj + 1].substr(pos1 + 1);
									vvsLine[indexCandidate][jj + 1] = temp;
									textLength = (double)HPDF_Page_TextWidth(page, vvsLine[indexCandidate][jj + 1].c_str());
									if (textLength == 0.0) { jf.err("TextWidth-jpdfbargraph.splitAtSpaceBox"); }
									filledNext = textLength / vMaxLength[jj + 1];
								}
								vFontBest.push_back(fontHeightTemp);
								fit = 1;
							}
						}
					}
					else {  // Failure. Reduce the font size and try again.
						fontHeightTemp -= 1.0;
						break;
					}
				}
				else if (ii == 0) {  // Note the current font size, and perhaps try again with an extra line.
					vvsLine[indexCandidate][vvsLine[indexCandidate].size() - 1 - indexLine] = sLine;
					vFontBest.push_back(fontHeightTemp);
					fit = 1;
				}
			}
		}

	}

	vector<int> fontMinMax = jf.minMax(vFontBest);
	indexCandidate = fontMinMax[1];
	error = HPDF_Page_SetFontAndSize(page, font, vFontBest[indexCandidate]);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfbargraph.splitAtSpaceBox"); }
	textLongest = (double)HPDF_Page_TextWidth(page, vvsLine[indexCandidate][vvsLine[indexCandidate].size() - 1].c_str());
	if (textLongest == 0.0) { jf.err("TextWidth-jpdfbargraph.splitAtSpaceBox"); }

	// Update the fontHeight and the box TR to the best values found.
	fontHeight = vFontBest[indexCandidate];
	boxBLTR[1][1] = (textLongest * sin(thetaRad)) + (fontHeight * cos(thetaRad));
	return vvsLine[indexCandidate];
}
vector<string> JPDFBARGRAPH::splitAtSpaceEven(string tooLong, int pieces)
{
	// Splits a string into roughly even-sized pieces, cutting only where there are
	// spaces within the given input string. 
	int count = 0, pivot, spot, tilt;
	char space;
	size_t pos1 = tooLong.find(' '), pos2;
	while (pos1 < tooLong.size()) {
		count++;
		pos1 = tooLong.find(' ', pos1 + 1);
	}

	vector<int> vSpot(pieces - 1);
	if (count < pieces - 1) { jf.err("Insufficient number of spaces-jpdfbargraph.splitAtSpace"); }
	else if (count == pieces - 1) {
		vSpot[0] = tooLong.find(' ');
		for (int ii = 1; ii < vSpot.size(); ii++) {
			vSpot[ii] = tooLong.find(' ', vSpot[ii - 1] + 1);
		}
	}
	else {
		for (int ii = 1; ii < pieces; ii++) {
			pivot = ii * (tooLong.size() / pieces);
			space = tooLong[pivot];
			tilt = 0;
			spot = pivot;
			while (space != ' ') {
				tilt++;
				spot = pivot + tilt;
				if (spot < tooLong.size()) { space = tooLong[spot]; }
				if (space == ' ') { break; }
				spot = pivot - tilt;
				if (ii == 1 && spot >= 0) { space = tooLong[spot]; }
				else if (ii > 1 && spot > vSpot[ii - 2]) { space = tooLong[spot]; }
			}
			vSpot[ii - 1] = spot;
		}
	}

	vector<string> vPieces(pieces);
	for (int ii = 0; ii < pieces; ii++) {
		if (ii == 0) { pos1 = 0; }
		else { pos1 = pos2 + 1; }
		if (ii == pieces - 1) { pos2 = tooLong.size() - 1; }
		else { pos2 = vSpot[ii]; }
		vPieces[ii] = tooLong.substr(pos1, pos2 - pos1);
	}

	return vPieces;
}
void JPDFBARGRAPH::textBox(vector<vector<double>> boxBLTR, string sText, string alignment, double fontsize)
{
	// Accepted alignment strings are "left", "right", "center", "justify".
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, fontsize);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfbargraph.textBox"); }
	double textLength = (double)HPDF_Page_TextWidth(page, sText.c_str());
	if (textLength == 0.0) { jf.err("TextWidth-jpdfbargraph.textBox"); }
	double textWidth = boxBLTR[1][0] - boxBLTR[0][0];
	while (textLength > textWidth) {
		fontsize -= 1.0;
		error = HPDF_Page_SetFontAndSize(page, font, fontsize);
		if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfbargraph.textBox"); }
		textLength = (double)HPDF_Page_TextWidth(page, sText.c_str());
		if (textLength == 0.0) { jf.err("TextWidth-jpdfbargraph.textBox"); }
	}
	error = HPDF_Page_BeginText(page);
	if (error != HPDF_OK) { jf.err("BeginText-jpdfbargraph.textBox"); }
	if (alignment == "right") {
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_RIGHT, NULL);
	}
	else if (alignment == "center") {
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_CENTER, NULL);
	}
	else if (alignment == "justify") {
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_JUSTIFY, NULL);
	}
	else {
		error = HPDF_Page_TextRect(page, boxBLTR[0][0], boxBLTR[1][1], boxBLTR[1][0], boxBLTR[0][1], sText.c_str(), HPDF_TALIGN_LEFT, NULL);
	}
	if (error != HPDF_OK) { jf.err("TextRect-jpdfbargraph.textBox"); }
	error = HPDF_Page_EndText(page);
	if (error != HPDF_OK) { jf.err("EndText-jpdfbargraph.textBox"); }
}
void JPDFBARGRAPH::textBoxAngled(string sText, vector<double> corner, string sCorner, double fontsize, double thetaRad)
{
	// BL is the (xCoord, yCoord) of the text box's bottom-left corner, which also
	// serves as the pivot of rotation. If the given corner is top-right (TR)
	// instead of bottom-left, a conversion will be made before painting the text.
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, fontsize);
	if (error != HPDF_OK) { jf.err("SetFontAndSize-jpdfbargraph.textBoxAngled"); }
	vector<double> BL;
	if (sCorner == "TR") {
		double textLength = (double)HPDF_Page_TextWidth(page, sText.c_str());
		if (textLength == 0.0) { jf.err("TextWidth-jpdfbargraph.textBoxAngled"); }
		BL = angledTRtoBL(corner[0], corner[1], textLength, fontsize, thetaRad);
	}
	else { BL = corner; }
	error = HPDF_Page_BeginText(page);
	if (error != HPDF_OK) { jf.err("BeginText-jpdfbargraph.textBoxAngled"); }
	error = HPDF_Page_SetTextMatrix(page, cos(thetaRad), sin(thetaRad), -sin(thetaRad), cos(thetaRad), BL[0] + (fontsize / 2.0), BL[1] + (fontsize / 2.0));
	if (error != HPDF_OK) { jf.err("SetTextMatrix-jpdfbargraph.textBoxAngled"); }
	error = HPDF_Page_ShowText(page, sText.c_str());
	if (error != HPDF_OK) { jf.err("ShowText-jpdfbargraph.textBoxAngled"); }
	error = HPDF_Page_EndText(page);
	if (error != HPDF_OK) { jf.err("EndText-jpdfbargraph.textBoxAngled"); }
}
