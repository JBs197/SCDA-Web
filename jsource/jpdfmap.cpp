#include "jpdfmap.h"

void JPDFMAP::displaceChildToParent(vector<vector<double>>& vvdBorder, vector<vector<double>>& childFrame, vector<double> dispParentTL)
{
	vector<double> vdShift(2);
	vdShift[0] = dispParentTL[0] - childFrame[0][0];
	vdShift[1] = dispParentTL[1] - childFrame[0][1];
	for (int ii = 0; ii < vvdBorder.size(); ii++)
	{
		vvdBorder[ii][0] += vdShift[0];
		vvdBorder[ii][1] += vdShift[1];
	}
}
void JPDFMAP::displaceParentToFrame(vector<vector<double>>& parentBorderKM, vector<vector<double>>& parentFrameKM)
{
	for (int ii = 0; ii < parentBorderKM.size(); ii++)
	{
		parentBorderKM[ii][0] -= parentFrameKM[0][0];
		parentBorderKM[ii][1] -= parentFrameKM[0][1];
	}
}
void JPDFMAP::drawCircle(vector<double> coordCenter, double radius, vector<double> colour, double thickness)
{
	// This variant will only draw the perimeter.
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { err("SetLineWidth-jpdfmap.drawCircle"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdfmap.drawCircle"); }
	error = HPDF_Page_MoveTo(page, coordCenter[0], coordCenter[1]);
	if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawCircle"); }
	error = HPDF_Page_Circle(page, coordCenter[0], coordCenter[1], radius);
	if (error != HPDF_OK) { err("Circle-jpdfmap.drawCircle"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { err("Stroke-jpdfmap.drawCircle"); }
}
void JPDFMAP::drawCircle(vector<double> coordCenter, double radius, vector<vector<double>> vColour, double thickness)
{
	// This variant will draw the perimeter using the first colour, and fill the 
	// circle using the second colour. 
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { err("SetLineWidth-jpdfmap.drawCircle"); }
	error = HPDF_Page_SetRGBStroke(page, vColour[0][0], vColour[0][1], vColour[0][2]);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdfmap.drawCircle"); }
	error = HPDF_Page_SetRGBFill(page, vColour[1][0], vColour[1][1], vColour[1][2]);
	if (error != HPDF_OK) { err("SetRGBFill-jpdfmap.drawCircle"); }
	error = HPDF_Page_MoveTo(page, coordCenter[0], coordCenter[1]);
	if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawCircle"); }
	error = HPDF_Page_Circle(page, coordCenter[0], coordCenter[1], radius);
	if (error != HPDF_OK) { err("Circle-jpdfmap.drawCircle"); }
	error = HPDF_Page_FillStroke(page);
	if (error != HPDF_OK) { err("FillStroke-jpdfmap.drawCircle"); }
}
void JPDFMAP::drawGradientBar(vector<vector<double>> rectBLTR, double frameThickness)
{
	bool orientation;
	int numColour = keyColour.size();
	if (rectBLTR[1][1] - rectBLTR[0][1] > rectBLTR[1][0] - rectBLTR[0][0])  // Vertical bar.
	{
		orientation = 1;
	}
	else { orientation = 0; }  // Horizontal bar.

	vector<float> thickStartStop = { (float)rectBLTR[0][!orientation], (float)rectBLTR[1][!orientation] };
	double bandwidth = (rectBLTR[1][orientation] - rectBLTR[0][orientation]) / (double)(numColour - 1);
	vector<int> vColourPos(numColour);
	vColourPos[0] = floor(rectBLTR[0][orientation]);
	vColourPos[numColour - 1] = ceil(rectBLTR[1][orientation]);
	for (int ii = 1; ii < numColour - 1; ii++)
	{
		vColourPos[ii] = int(round(rectBLTR[0][orientation] + ((double)ii * bandwidth)));
	}
	if (orientation) { drawGradientV(thickStartStop, keyColour, vColourPos); }
	else { drawGradientH(thickStartStop, keyColour, vColourPos); }

	if (frameThickness > 0.0) {
		drawRect(rectBLTR, { 0.0, 0.0, 0.0 }, frameThickness);
	}
}
void JPDFMAP::drawGradientBar(vector<vector<double>> rectBLTR, vector<vector<double>> gradientBLTR, double frameThickness)
{
	// This variant is meant for a gradient which does not completely fill the rectangle.
	bool orientation;
	int numColour = keyColour.size();
	if (rectBLTR[1][1] - rectBLTR[0][1] > rectBLTR[1][0] - rectBLTR[0][0])  // Vertical bar.
	{
		orientation = 1;
	}
	else { orientation = 0; }  // Horizontal bar.

	vector<float> thickStartStop = { (float)gradientBLTR[0][!orientation], (float)gradientBLTR[1][!orientation] };
	double bandwidth = (gradientBLTR[1][orientation] - gradientBLTR[0][orientation]) / (double)(numColour - 1);
	vector<int> vColourPos(numColour);
	vColourPos[0] = floor(gradientBLTR[0][orientation]);
	vColourPos[numColour - 1] = ceil(gradientBLTR[1][orientation]);
	for (int ii = 1; ii < numColour - 1; ii++)
	{
		vColourPos[ii] = int(round(gradientBLTR[0][orientation] + ((double)ii * bandwidth)));
	}
	if (orientation) { drawGradientV(thickStartStop, keyColour, vColourPos); }
	else { drawGradientH(thickStartStop, keyColour, vColourPos); }

	if (frameThickness > 0.0) {
		drawRect(rectBLTR, { 0.0, 0.0, 0.0 }, frameThickness);
	}
}
void JPDFMAP::drawGradientH(vector<float> heightStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos)
{
	if (vColour.size() != vColourPos.size()) { err("Size mismatch-jpdfmap.drawGradientH"); }
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
	if (error != HPDF_OK) { err("SetLineWidth-jpdfmap.drawGradientH"); }

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
		if (error != HPDF_OK) { err("SetRGBStroke-jpdfmap.drawGradientH"); }
		error = HPDF_Page_MoveTo(page, (float)xPos, heightStartStop[0]);
		if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawGradientH"); }
		error = HPDF_Page_LineTo(page, (float)xPos, heightStartStop[1]);
		if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawGradientH"); }
		error = HPDF_Page_Stroke(page);
		if (error != HPDF_OK) { err("Stroke-jpdfmap.drawGradientH"); }

		xPos += xInc;
	}
	error = HPDF_Page_SetRGBStroke(page, r, g, b);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdfmap.drawGradientH"); }
	error = HPDF_Page_MoveTo(page, (float)xPos, heightStartStop[0]);
	if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawGradientH"); }
	error = HPDF_Page_LineTo(page, (float)xPos, heightStartStop[1]);
	if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawGradientH"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { err("Stroke-jpdfmap.drawGradientH"); }

}
void JPDFMAP::drawGradientV(vector<float> widthStartStop, vector<vector<double>>& vColour, vector<int>& vColourPos)
{
	if (vColour.size() != vColourPos.size()) { err("Size mismatch-jpdfmap.drawGradientV"); }
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
	if (error != HPDF_OK) { err("SetLineWidth-jpdfmap.drawGradientV"); }

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
		if (error != HPDF_OK) { err("SetRGBStroke-jpdfmap.drawGradientV"); }
		error = HPDF_Page_MoveTo(page, widthStartStop[0], (float)yPos);
		if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawGradientV"); }
		error = HPDF_Page_LineTo(page, widthStartStop[1], (float)yPos);
		if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawGradientV"); }
		error = HPDF_Page_Stroke(page);
		if (error != HPDF_OK) { err("Stroke-jpdfmap.drawGradientV"); }

		yPos += yInc;
	}
	error = HPDF_Page_SetRGBStroke(page, r, g, b);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdfmap.drawGradientV"); }
	error = HPDF_Page_MoveTo(page, widthStartStop[0], (float)yPos);
	if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawGradientV"); }
	error = HPDF_Page_LineTo(page, widthStartStop[1], (float)yPos);
	if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawGradientV"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { err("Stroke-jpdfmap.drawGradientV"); }

}
vector<vector<double>> JPDFMAP::drawLegend(int indexDataset)
{
	if (sUnit.size() < 1) { err("No unit initialized-wtpaint.paintLegendBar"); }
	double width = scalebarBLTR[1][0] - scalebarBLTR[0][0];
	if (width <= 0.0) { err("Invalid barTLBR width-wtpaint.paintLegendBar"); }
	double height = scalebarBLTR[1][1] - scalebarBLTR[0][1];
	if (height <= 0.0) { err("Invalid barTLBR height-wtpaint.paintLegendBar"); }
	bool barVertical = 0;
	int numColour = keyColour.size();
	if (height > width) { barVertical = 1; }

	vector<vector<double>> legendTickMarks;  // Form [child bar, parent bar][ticks].
	if (legendBarDouble == 1)
	{
		legendTickMarks.resize(2, vector<double>());
		legendTickMarks[0] = jsb.getTickValues(indexDataset, numColour, { 0 });  // Exclude the parent region.
		legendTickMarks[1].resize(4);
		legendTickMarks[1][0] = legendTickMarks[0][0];
		legendTickMarks[1][1] = legendTickMarks[0][legendTickMarks[0].size() - 1];
		legendTickMarks[1][2] = jsb.getDatasetValue(indexDataset, 0);
		legendTickMarks[1][3] = jf.roundingCeil(legendTickMarks[1][2]);
	}
	else
	{
		legendTickMarks.resize(1, vector<double>());
		legendTickMarks[0] = jsb.getTickValues(indexDataset, numColour);
	}
	legendMin = legendTickMarks[0][0];
	legendMax = legendTickMarks[0][legendTickMarks[0].size() - 1];

	vector<vector<double>> rectBLTR(2, vector<double>(2));
	if (barVertical)
	{
		rectBLTR[0][0] = scalebarBLTR[0][0] + barNumberWidth + (0.5 * barThickness);
		rectBLTR[0][1] = scalebarBLTR[0][1] + barThickness;
		rectBLTR[1][0] = rectBLTR[0][0] + barThickness;
		rectBLTR[1][1] = scalebarBLTR[1][1] - barThickness;
	}
	else
	{
		rectBLTR[0][0] = scalebarBLTR[0][0] + barThickness;
		rectBLTR[1][1] = scalebarBLTR[1][1] - ((double)legendTickLines * barNumberHeight) - (0.5 * barThickness);
		rectBLTR[1][0] = scalebarBLTR[1][0] - barThickness;
		rectBLTR[0][1] = rectBLTR[1][1] - barThickness;
	}

	if (legendMin != legendMax) { drawGradientBar(rectBLTR, 2.0); }
	else {
		vector<vector<double>> colour(2, vector<double>());
		colour[0] = { 0.0, 0.0, 0.0 };
		colour[1] = extraColour;
		drawRect(rectBLTR, colour, 2.0);
	}
	drawLegendTicks(rectBLTR, legendTickMarks[0], 2.0);

	if (legendBarDouble == 1)
	{
		if (barVertical)
		{
			rectBLTR[0][0] += (1.5 * barThickness);
			rectBLTR[1][0] += (1.5 * barThickness);
		}
		else
		{
			rectBLTR[0][1] -= (1.5 * barThickness);
			rectBLTR[1][1] -= (1.5 * barThickness);
		}
		vector<vector<double>> gradientBLTR = rectBLTR;
		double percentage = (legendTickMarks[1][1] - legendTickMarks[1][0]) / (legendTickMarks[1][3] - legendTickMarks[1][0]);
		double length;
		if (barVertical)
		{
			length = gradientBLTR[1][1] - gradientBLTR[0][1];
			gradientBLTR[1][1] = gradientBLTR[0][1] + (length * percentage);
		}
		else
		{
			length = gradientBLTR[1][0] - gradientBLTR[0][0];
			gradientBLTR[1][0] = gradientBLTR[0][0] + (length * percentage);
		}
		drawGradientBar(rectBLTR, gradientBLTR, 2.0);
		drawLegendTicks(rectBLTR, legendTickMarks, 2.0);
	}
	else if (legendBarDouble == 2)
	{
		double dotRadius = (barThickness - 6.0) / 2.0;
		drawLegendTicks(rectBLTR, legendTickMarks[0], jsb.getDatasetValue(indexDataset, 0), dotRadius, 2.0);
	}

	return legendTickMarks;
}
void JPDFMAP::drawLegendTicks(vector<vector<double>> rectBLTR, vector<double>& tickValues, double tickThickness)
{
	// This variant is for the child bar.
	double dTemp, length;
	string displayUnit, suffix, temp;
	vector<vector<double>> startStop(2, vector<double>(2));
	vector<vector<double>> textBLTR(2, vector<double>(2));
	vector<double> black = { 0.0, 0.0, 0.0 };
	bool orientation;
	if (rectBLTR[1][1] - rectBLTR[0][1] > rectBLTR[1][0] - rectBLTR[0][0])  // Vertical bar.
	{
		orientation = 1;
		length = rectBLTR[1][1] - rectBLTR[0][1];
	}
	else  // Horizontal bar.
	{
		orientation = 0;
		length = rectBLTR[1][0] - rectBLTR[0][0];
	}
	int numColour = keyColour.size();
	double bandWidth = 1.0 / (double)(numColour - 1);
	vector<string> vsVal = jf.doubleToCommaString(tickValues, 1);
	if (legendTickLines == 1) { suffix = " (" + sUnit + ")"; }
	else
	{
		if (displayData.size() == 1) { displayUnit = "(" + sUnit + ")"; }
		else
		{
			suffix = " %";
			displayUnit = "(of population)";
		}
	}
	for (int ii = 0; ii < numColour; ii++)
	{
		dTemp = (double)ii * bandWidth;
		if (orientation)
		{
			startStop[0][0] = rectBLTR[0][0];
			startStop[1][0] = startStop[0][0] - (0.5 * barThickness);
			startStop[0][1] = rectBLTR[0][1] + (dTemp * length);
			startStop[1][1] = startStop[0][1];
		}
		else
		{
			startStop[0][0] = rectBLTR[0][0] + (dTemp * length);
			startStop[1][0] = startStop[0][0];
			startStop[0][1] = rectBLTR[1][1];
			startStop[1][1] = startStop[0][1] + (0.5 * barThickness);
		}
		drawLine(startStop, black, tickThickness);

		temp = vsVal[ii] + suffix;
		if (orientation) {
			textBLTR[0][0] = startStop[1][0] - barNumberWidth;
			textBLTR[0][1] = startStop[1][1] + (barNumberHeight * (((double)legendTickLines * 0.5) - 1.0));
			textBLTR[1][0] = startStop[1][0];
			textBLTR[1][1] = textBLTR[0][1] + barNumberHeight;
			textBox(textBLTR, temp, "right", fontSize);
			if (legendTickLines > 1) {
				textBLTR[0][1] -= barNumberHeight;
				textBLTR[1][1] -= barNumberHeight;
				textBox(textBLTR, displayUnit, "right", fontSize);
			}
		}
		else {
			textBLTR[0][0] = startStop[0][0] - (barNumberWidth * 0.5);
			textBLTR[0][1] = startStop[1][1] + (barNumberHeight * ((double)legendTickLines - 1.0));
			textBLTR[1][0] = textBLTR[0][0] + barNumberWidth;
			textBLTR[1][1] = textBLTR[0][1] + barNumberHeight;
			textBox(textBLTR, temp, "center", fontSize);
			if (legendTickLines > 1) {
				textBLTR[0][1] -= barNumberHeight;
				textBLTR[1][1] -= barNumberHeight;
				textBox(textBLTR, displayUnit, "center", fontSize);
			}
		}
	}
}
void JPDFMAP::drawLegendTicks(vector<vector<double>> rectBLTR, vector<vector<double>>& tickValues, double tickThickness)
{
	// This variant is for the parent bar.
	double dPercent, dPrevious, length;
	string displayUnit, suffix, temp;
	vector<vector<double>> startStop(2, vector<double>(2));
	vector<vector<double>> textBLTR(2, vector<double>(2));
	vector<double> black = { 0.0, 0.0, 0.0 };
	bool orientation, skew;
	if (rectBLTR[1][1] - rectBLTR[0][1] > rectBLTR[1][0] - rectBLTR[0][0])  // Vertical bar.
	{
		orientation = 1;
		length = rectBLTR[1][1] - rectBLTR[0][1];
	}
	else  // Horizontal bar.
	{
		orientation = 0;
		length = rectBLTR[1][0] - rectBLTR[0][0];
	}
	double dRadius = (barThickness - 6.0) / 2.0;
	vector<string> vsVal = jf.doubleToCommaString(tickValues[1], 1);
	if (legendTickLines == 1) { suffix = " (" + sUnit + ")"; }
	else
	{
		if (sUnit == "% of population") { 
			suffix = " %";
			displayUnit = "(of population)";
		} 
		else {
			displayUnit = "(" + sUnit + ")";
		}
	}
	for (int ii = 0; ii < 4; ii++)
	{
		dPercent = (tickValues[1][ii] - tickValues[1][0]) / (tickValues[1][3] - tickValues[1][0]);
		skew = 0;
		switch (ii)
		{
		case 0:
		{
			if (orientation)
			{
				startStop[0][0] = rectBLTR[1][0];
				startStop[1][0] = startStop[0][0] + (0.75 * barThickness);
				startStop[0][1] = rectBLTR[0][1];
				startStop[1][1] = startStop[0][1];
			}
			else
			{
				startStop[0][0] = rectBLTR[0][0];
				startStop[1][0] = startStop[0][0];
				startStop[0][1] = rectBLTR[0][1];
				startStop[1][1] = startStop[0][1] - (0.75 * barThickness);
			}
			break;
		}
		case 1:
		{
			if (orientation)
			{
				startStop[0][0] = rectBLTR[0][0];
				startStop[1][0] = rectBLTR[1][0];
				startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
				startStop[1][1] = startStop[0][1];
				drawLine(startStop, black, tickThickness);

				if (dPercent * length < barNumberHeight * 3.0)
				{
					skew = 1;
					startStop[0][0] = rectBLTR[1][0];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = rectBLTR[0][1] + (barNumberHeight * 3.0);
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);
				}
				else if ((1.0 - dPercent) * length < 6.0 * barNumberHeight) {
					skew = 1;
					startStop[0][0] = rectBLTR[1][0];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = rectBLTR[1][1] - (6.0 * barNumberHeight);
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);
				}
				else {
					startStop[0][0] = rectBLTR[1][0];
					startStop[1][0] = startStop[0][0] + (0.75 * barThickness);
					startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
					startStop[1][1] = startStop[0][1];
				}
				dPrevious = startStop[1][1];
			}
			else
			{
				startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
				startStop[1][0] = startStop[0][0];
				startStop[0][1] = rectBLTR[0][1];
				startStop[1][1] = startStop[0][1] + (1.0 * barThickness);
				drawLine(startStop, black, tickThickness);
				
				if (dPercent * length < barNumberWidth) {
					skew = 1;
					startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
					startStop[1][0] = startStop[0][0];
					startStop[0][1] = rectBLTR[0][1];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = rectBLTR[0][0] + barNumberWidth;
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					drawLine(startStop, black, tickThickness);
				}
				else if ((1.0 - dPercent) * length < (2.0 * barNumberWidth)) {
					skew = 1;
					startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
					startStop[1][0] = startStop[0][0];
					startStop[0][1] = rectBLTR[0][1];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = rectBLTR[1][0] - (2.0 * barNumberWidth);
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					drawLine(startStop, black, tickThickness);
				}
				else {
					startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
					startStop[1][0] = startStop[0][0];
					startStop[0][1] = rectBLTR[0][1];
					startStop[1][1] = startStop[0][1] - (0.75 * barThickness);
				}
				dPrevious = startStop[1][0];
			}
			break;
		}
		case 2:
		{
			vector<double> dot(2);
			if (orientation)
			{
				dot[0] = rectBLTR[0][0] + (0.5 * barThickness);
				dot[1] = rectBLTR[0][1] + (dPercent * length);
				drawCircle(dot, dRadius, { black, black }, 2.0);
				if ((dPercent * length) + rectBLTR[0][1] < dPrevious + (3.0 * barNumberHeight)) {
					skew = 1;
					startStop[0][0] = rectBLTR[1][0];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = dPrevious + (3.0 * barNumberHeight);
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);
				}
				else if ((1.0 - dPercent) * length < 3.0 * barNumberHeight)
				{
					skew = 1;
					startStop[0][0] = rectBLTR[1][0];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = rectBLTR[1][1] - (3.0 * barNumberHeight);
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);
				}
				else
				{
					startStop[0][0] = rectBLTR[1][0];
					startStop[1][0] = startStop[0][0] + (0.75 * barThickness);
					startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
					startStop[1][1] = startStop[0][1];
				}
			}
			else
			{
				dot[0] = rectBLTR[0][0] + (dPercent * length);
				dot[1] = rectBLTR[0][1] + (0.5 * barThickness);
				drawCircle(dot, dRadius, { black, black }, 2.0);
				if ((dPercent * length) + rectBLTR[0][0] < dPrevious + barNumberWidth) {
					skew = 1;
					startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
					startStop[1][0] = startStop[0][0];
					startStop[0][1] = rectBLTR[0][1];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = dPrevious + barNumberWidth;
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					drawLine(startStop, black, tickThickness);
				}
				else if ((1.0 - dPercent) * length < barNumberWidth)
				{
					skew = 1;
					startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
					startStop[1][0] = startStop[0][0];
					startStop[0][1] = rectBLTR[0][1];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = rectBLTR[1][0] - barNumberWidth;
					startStop[1][1] = startStop[0][1];
					drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					drawLine(startStop, black, tickThickness);
				}
				else
				{
					startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
					startStop[1][0] = startStop[0][0];
					startStop[0][1] = rectBLTR[0][1];
					startStop[1][1] = startStop[0][1] - (0.75 * barThickness);
				}
			}
			break;
		}
		case 3:
		{
			if (orientation)
			{
				startStop[0][0] = rectBLTR[1][0];
				startStop[1][0] = startStop[0][0] + (0.75 * barThickness);
				startStop[0][1] = rectBLTR[1][1];
				startStop[1][1] = startStop[0][1];
			}
			else
			{
				startStop[0][0] = rectBLTR[1][0];
				startStop[1][0] = startStop[0][0];
				startStop[0][1] = rectBLTR[0][1];
				startStop[1][1] = startStop[0][1] - (0.75 * barThickness);
			}
			break;
		}
		}
		if (!skew) { drawLine(startStop, black, tickThickness); }

		if (ii != 2) { temp = vsVal[ii] + suffix; }
		else { temp = mapRegion[0] + suffix; }

		if (orientation)
		{
			textBLTR[0][0] = rectBLTR[1][0] + barThickness;
			textBLTR[0][1] = startStop[1][1] + (barNumberHeight * (((double)legendTickLines * 0.5) - 1.0));
			textBLTR[1][0] = textBLTR[0][0] + barNumberWidth;
			textBLTR[1][1] = textBLTR[0][1] + barNumberHeight;
			textBox(textBLTR, temp, "left", fontSize);
			if (legendTickLines > 1)
			{
				textBLTR[0][1] -= barNumberHeight;
				textBLTR[1][1] -= barNumberHeight;
				textBox(textBLTR, displayUnit, "left", fontSize);
			}
		}
		else
		{
			textBLTR[0][0] = startStop[1][0] - (barNumberWidth * 0.5);
			textBLTR[0][1] = rectBLTR[0][1] - (barNumberHeight * 2.0);
			textBLTR[1][0] = textBLTR[0][0] + barNumberWidth;
			textBLTR[1][1] = textBLTR[0][1] + barNumberHeight;
			textBox(textBLTR, temp, "center", fontSize);
			if (legendTickLines > 1)
			{
				textBLTR[0][1] -= barNumberHeight;
				textBLTR[1][1] -= barNumberHeight;
				textBox(textBLTR, displayUnit, "center", fontSize);
			}
		}
	}
}
void JPDFMAP::drawLegendTicks(vector<vector<double>> rectBLTR, vector<double>& tickValues, double parentValue, double dotRadius, double tickThickness)
{
	// This variant is for the parent tick mark on a single bar.
	bool orientation;
	double length;
	string alignment, displayUnit, temp;
	if (rectBLTR[1][1] - rectBLTR[0][1] > rectBLTR[1][0] - rectBLTR[0][0])  // Vertical bar.
	{
		orientation = 1;
		length = rectBLTR[1][1] - rectBLTR[0][1];
	}
	else  // Horizontal bar.
	{
		orientation = 0;
		length = rectBLTR[1][0] - rectBLTR[0][0];
	}
	double dPercent = (parentValue - tickValues[0]) / (tickValues[tickValues.size() - 1] - tickValues[0]);
	if (legendTickLines == 1) { displayUnit = " (" + sUnit + ")"; }
	else
	{
		if (displayData.size() == 1) { displayUnit = "(" + sUnit + ")"; }  // # of persons
		else { displayUnit = "(% of population)"; }
	}
	vector<double> dot(2);
	vector<double> black = { 0.0, 0.0, 0.0 };
	vector<vector<double>> startStop(2, vector<double>(2));
	vector<vector<double>> textBLTR(2, vector<double>(2));
	if (orientation)
	{
		dot[0] = rectBLTR[0][0] + (0.5 * barThickness);
		dot[1] = rectBLTR[0][1] + (dPercent * length);
		startStop[0][0] = rectBLTR[1][0];
		startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
		startStop[1][0] = startStop[0][0] + (0.5 * barThickness);
		startStop[1][1] = startStop[0][1];
		textBLTR[0][0] = startStop[1][0];
		textBLTR[0][1] = startStop[1][1] + (0.5 * barNumberHeight);
		textBLTR[1][0] = textBLTR[0][0] + barNumberWidth;
		textBLTR[1][1] = textBLTR[0][1] + barNumberHeight;
		alignment = "left";
	}
	else
	{
		dot[0] = rectBLTR[0][0] + (dPercent * length);
		dot[1] = rectBLTR[0][1] + (0.5 * barThickness);
		startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
		startStop[0][1] = rectBLTR[0][1];
		startStop[1][0] = startStop[0][0];
		startStop[1][1] = startStop[0][1] - (0.5 * barThickness);
		textBLTR[0][0] = startStop[0][0] - (0.5 * barNumberWidth);
		textBLTR[0][1] = startStop[1][1] - barNumberHeight;
		textBLTR[1][0] = startStop[0][0] + (0.5 * barNumberWidth);
		textBLTR[1][1] = startStop[1][1];
		alignment = "center";
	}
	drawCircle(dot, dotRadius, { black, black }, 2.0);
	drawLine(startStop, black, 2.0);
	if (legendTickLines == 1)
	{
		temp = mapRegion[0] + displayUnit;
		textBox(textBLTR, temp, alignment, fontSize);
	}
	else
	{
		textBox(textBLTR, mapRegion[0], alignment, fontSize);
		textBLTR[0][1] -= barNumberHeight;
		textBLTR[1][1] -= barNumberHeight;
		textBox(textBLTR, displayUnit, alignment, fontSize);
	}
}
void JPDFMAP::drawLine(vector<vector<double>> startStop, vector<double> colour, double thickness)
{
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { err("SetLineWidth-jpdfmap.drawLine"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdfmap.drawLine"); }
	error = HPDF_Page_MoveTo(page, startStop[0][0], startStop[0][1]);
	if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawLine"); }
	error = HPDF_Page_LineTo(page, startStop[1][0], startStop[1][1]);
	if (error != HPDF_OK) { err("LineTo-jpdfmap.drawLine"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { err("Stroke-jpdfmap.drawLine"); }
}
void JPDFMAP::drawMap()
{
	int indexDataset = jsb.getActiveIndex();
	vector<vector<double>> legendTickMarks = drawLegend(indexDataset);  // Form [child bar, parent bar][tick values].
	vector<vector<double>> mapColour = jsb.getDatasetColour(keyColour, legendTickMarks[0], indexDataset);

	int numRegion = mapBorder.size();
	if (mapColour.size() != numRegion) { err("Size mismatch-jpdfmap.drawMap"); }
	for (int ii = 0; ii < numRegion; ii++)
	{
		// Special case wherein parent is completely obscured.
		if (mapRegion[ii] == "Canada") { mapColour[ii] = { 0.0, 0.0, 0.0 }; }
		
		if (ii == selIndex) {  // This non-parent region was selected by the user. Draw dotted.
			drawRegion(mapBorder[ii], mapColour[ii], 1);
		}
		else {  // Standard solid border.
			drawRegion(mapBorder[ii], mapColour[ii]);
		}
	}
}
void JPDFMAP::drawRect(vector<vector<double>> rectBLTR, vector<double> colour, double thickness)
{
	// This variant will only draw the perimeter.
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { err("SetLineWidth-jpdfmap.drawRect"); }
	error = HPDF_Page_SetRGBStroke(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdfmap.drawRect"); }
	error = HPDF_Page_Rectangle(page, rectBLTR[0][0], rectBLTR[0][1], rectBLTR[1][0] - rectBLTR[0][0], rectBLTR[1][1] - rectBLTR[0][1]);
	if (error != HPDF_OK) { err("Rectangle-jpdfmap.drawRect"); }
	error = HPDF_Page_Stroke(page);
	if (error != HPDF_OK) { err("Stroke-jpdfmap.drawRect"); }
}
void JPDFMAP::drawRect(vector<vector<double>> rectBLTR, vector<vector<double>> vColour, double thickness)
{
	// This variant will draw the perimeter using the first colour, and fill the 
	// rectangle using the second colour. 
	HPDF_STATUS error = HPDF_Page_SetLineWidth(page, thickness);
	if (error != HPDF_OK) { err("SetLineWidth-jpdfmap.drawRect"); }
	error = HPDF_Page_SetRGBStroke(page, vColour[0][0], vColour[0][1], vColour[0][2]);
	if (error != HPDF_OK) { err("SetRGBStroke-jpdfmap.drawRect"); }
	error = HPDF_Page_SetRGBFill(page, vColour[1][0], vColour[1][1], vColour[1][2]);
	if (error != HPDF_OK) { err("SetRGBFill-jpdfmap.drawRect"); }
	error = HPDF_Page_Rectangle(page, rectBLTR[0][0], rectBLTR[0][1], rectBLTR[1][0] - rectBLTR[0][0], rectBLTR[1][1] - rectBLTR[0][1]);
	if (error != HPDF_OK) { err("Rectangle-jpdfmap.drawRect"); }
	error = HPDF_Page_FillStroke(page);
	if (error != HPDF_OK) { err("FillStroke-jpdfmap.drawRect"); }
}
void JPDFMAP::drawRegion(vector<vector<double>>& vvdPath, vector<double> colour, bool dotted)
{
	HPDF_STATUS error = HPDF_Page_SetRGBFill(page, colour[0], colour[1], colour[2]);
	if (error != HPDF_OK) { err("SetRGBFill-jpdfmap.drawRegion"); }
	error = HPDF_Page_SetLineWidth(page, 1.0);
	if (error != HPDF_OK) { err("SetLineWidth-jpdfmap.drawRegion"); }
	if (dotted) {
		HPDF_UINT16 dash[1] = { 3 };
		error = HPDF_Page_SetDash(page, dash, 1, 1);
		if (error != HPDF_OK) { err("SetDash-jpdfmap.drawRegion"); }
	}
	error = HPDF_Page_MoveTo(page, vvdPath[0][0], vvdPath[0][1]);
	if (error != HPDF_OK) { err("MoveTo-jpdfmap.drawRegion"); }
	for (int ii = 1; ii < vvdPath.size(); ii++)
	{
		error = HPDF_Page_LineTo(page, vvdPath[ii][0], vvdPath[ii][1]);
		if (error != HPDF_OK) { err("LineTo-jpdf.drawRegion"); }
	}
	error = HPDF_Page_ClosePathFillStroke(page);
	if (error != HPDF_OK) { err("ClosePath-jpdfmap.drawRegion"); }
	if (dotted) {
		error = HPDF_Page_SetDash(page, nullptr, 0, 0);
		if (error != HPDF_OK) { err("SetDash-jpdfmap.drawRegion"); }
	}
}
void JPDFMAP::err(string message)
{
	string errorMessage = "JPDFMAP error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
vector<double> JPDFMAP::getChildTL(vector<vector<double>>& vvdBorder, vector<vector<double>>& childFrameKM, vector<vector<double>>& parentFrameKM)
{
	if (PPKM <= 0.0) { err("No PPKM-jpdfmap.getChildTL"); }
	if (childFrameKM.size() != 2) { err("Missing frameKM-jpdfmap.getChildTL"); }
	vector<double> vdTL(2);
	vdTL[0] = childFrameKM[0][0] - parentFrameKM[0][0];
	vdTL[0] *= PPKM;
	vdTL[1] = childFrameKM[0][1] - parentFrameKM[0][1];
	vdTL[1] *= PPKM;
	return vdTL;
}
unordered_map<string, double> JPDFMAP::getMapRegionData(int& decimalPlaces)
{
	int indexDataset = jsb.getActiveIndex();
	if (indexDataset < 0) { err("jScaleBar missing initialization-jpdfmap.getMapRegionData"); }
	decimalPlaces = jsb.vDecimalPlaces[indexDataset];
	unordered_map<string, double> mapRegionData = jsb.getMapDatasetLabel(mapRegion, indexDataset);
	return mapRegionData;
}
void JPDFMAP::kmToPixel(vector<vector<vector<double>>> frame, vector<vector<vector<double>>>& border)
{
	if (legendBarDouble < 0 || legendTickLines < 0) { err("Missing legend init-jpdfmap.kmToPixel"); }
	mapBorder = border;
	initImgBar(frame[0]);
	displaceParentToFrame(mapBorder[0], frame[0]);
	scaleParentToPage(mapBorder[0], frame[0]);
	vector<double> vCenter, dispParentTL;  // dispParent TL is child's TL pixel displacement from parent TL.
	for (int ii = 1; ii < mapRegion.size(); ii++)
	{
		if (mapRegion[ii].back() == '!')
		{
			selIndex = ii;
			mapRegion[ii].pop_back();
		}
		dispParentTL = getChildTL(mapBorder[ii], frame[ii], frame[0]);
		scaleChildToPage(mapBorder[ii], frame[ii]);
		displaceChildToParent(mapBorder[ii], frame[ii], dispParentTL);
	}
	vector<double> delta = { imgBLTR[0][0], imgBLTR[1][1] };
	for (int ii = 0; ii < mapRegion.size(); ii++)
	{
		jf.coordReflectY(mapBorder[ii], 0.0);
		jf.coordDisplacement(mapBorder[ii], delta);
	}
}
void JPDFMAP::initColour()
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

	keyColour.resize(6);
	keyColour[0] = { 1.0, 0.0, 0.0 };  // Red
	keyColour[1] = { 1.0, 1.0, 0.0 };  // Yellow
	keyColour[2] = { 0.0, 1.0, 0.0 };  // Green
	keyColour[3] = { 0.0, 1.0, 1.0 };  // Teal
	keyColour[4] = { 0.0, 0.0, 1.0 };  // Blue
	keyColour[5] = { 0.5, 0.0, 1.0 };  // Violet
	extraColour = { 1.0, 0.0, 0.5 };  // Pink
	unknownColour = { 0.7, 0.7, 0.7 };  // Grey
}
void JPDFMAP::initScaleBar(string unit, vector<int> viIndex, int barDouble, int tickLines)
{
	int index;
	double power;
	vector<vector<double>> mapDataTemp;
	sUnit = unit;
	jsb.addDataset(mapData);
	displayData = viIndex;
	legendBarDouble = barDouble;
	legendTickLines = tickLines;
	if (displayData.size() > 1) {
		index = jsb.makeDataset(displayData, '/');
	}
	else {
		index = displayData[0];
	}
	jsb.setUnit(index, sUnit);
}
void JPDFMAP::initImgBar(vector<vector<double>>& parentFrameKM)
{
	// mapBLTR has a unit of pixels, while parentFrameKM has a unit of KM.
	double parentWidthKM = parentFrameKM[1][0] - parentFrameKM[0][0];
	double parentHeightKM = parentFrameKM[1][1] - parentFrameKM[0][1];
	double parentAspectRatio = parentWidthKM / parentHeightKM;
	double dWidth = mapBLTR[1][0] - mapBLTR[0][0];
	double dHeight = mapBLTR[1][1] - mapBLTR[0][1];
	scalebarBLTR.resize(2, vector<double>(2));
	imgBLTR.resize(2, vector<double>(2));
	imgBLTR[0][0] = mapBLTR[0][0];
	imgBLTR[1][1] = mapBLTR[1][1];

	double barSpaceHorizontal, barSpaceVertical, extraSpace;
	if (legendBarDouble == 0)
	{
		barSpaceHorizontal = barNumberWidth + (1.5 * barThickness);
		barSpaceVertical = ((double)legendTickLines * barNumberHeight) + (1.5 * barThickness);
	}
	else if (legendBarDouble == 1)
	{
		barSpaceHorizontal = (2.0 * barNumberWidth) + (4.0 * barThickness);
		barSpaceVertical = (2.0 * (double)legendTickLines * barNumberHeight) + (4.0 * barThickness);
	}
	else if (legendBarDouble == 2)
	{
		barSpaceHorizontal = (2.0 * barNumberWidth) + (2.0 * barThickness);
		barSpaceVertical = ((double)(2 * legendTickLines) * barNumberHeight) + (2.0 * barThickness);
	}

	double spaceARifVertical = (dWidth - barSpaceHorizontal) / dHeight;
	double spaceARifHorizontal = dWidth / (dHeight - barSpaceVertical);
	if (abs(spaceARifVertical - parentAspectRatio) < abs(spaceARifHorizontal - parentAspectRatio))
	{
		// The legend bar will be vertical.
		if (spaceARifVertical < parentAspectRatio)
		{
			// The limiting factor will be the parent image's width.
			imgBLTR[1][0] = imgBLTR[0][0] + dWidth - barSpaceHorizontal - 1.0;
			imgBLTR[0][1] = imgBLTR[1][1] - ((imgBLTR[1][0] - imgBLTR[0][0]) / parentAspectRatio);
			scalebarBLTR[0][0] = imgBLTR[1][0] + 1.0;
			scalebarBLTR[1][1] = imgBLTR[1][1];
			scalebarBLTR[1][0] = scalebarBLTR[0][0] + barSpaceHorizontal;
			scalebarBLTR[0][1] = imgBLTR[0][1];
		}
		else
		{
			// The limiting factor will be the parent image's height.
			imgBLTR[0][1] = mapBLTR[0][1];
			imgBLTR[1][0] = imgBLTR[0][0] + ((imgBLTR[1][1] - imgBLTR[0][1]) * parentAspectRatio);
			scalebarBLTR[0][0] = imgBLTR[1][0] + 1.0;
			scalebarBLTR[0][1] = imgBLTR[0][1];
			scalebarBLTR[1][0] = scalebarBLTR[0][0] + barSpaceHorizontal;
			scalebarBLTR[1][1] = imgBLTR[1][1];

			// Automatically apply horizontal centering.
			extraSpace = mapBLTR[1][0] - scalebarBLTR[1][0];
			extraSpace /= 2.0;
			imgBLTR[0][0] += extraSpace;
			imgBLTR[1][0] += extraSpace;
			scalebarBLTR[0][0] += extraSpace;
			scalebarBLTR[1][0] += extraSpace;
		}
	}
	else
	{
		// The legend bar will be horizontal.
		if (spaceARifHorizontal < parentAspectRatio)
		{
			// The limiting factor will be the parent image's width.
			imgBLTR[1][0] = mapBLTR[1][0];
			imgBLTR[0][1] = imgBLTR[1][1] - ((imgBLTR[1][0] - imgBLTR[0][0]) / parentAspectRatio);
			scalebarBLTR[1][1] = imgBLTR[0][1] - 1.0;
			scalebarBLTR[0][1] = scalebarBLTR[1][1] - barSpaceVertical;
			scalebarBLTR[0][0] = imgBLTR[0][0];
			scalebarBLTR[1][0] = imgBLTR[1][0];
		}
		else
		{
			// The limiting factor will be the parent image's height.
			scalebarBLTR[0][1] = mapBLTR[0][1];
			scalebarBLTR[1][1] = scalebarBLTR[0][1] + barSpaceVertical;
			scalebarBLTR[0][0] = imgBLTR[0][0];
			scalebarBLTR[1][0] = mapBLTR[1][0];
			imgBLTR[0][1] = scalebarBLTR[1][1] + 1.0;
			imgBLTR[1][0] = imgBLTR[0][0] + ((imgBLTR[1][1] - imgBLTR[0][1]) * parentAspectRatio);

			// Automatically apply horizontal centering.
			extraSpace = mapBLTR[1][0] - imgBLTR[1][0];
			extraSpace /= 2.0;
			imgBLTR[0][0] += extraSpace;
			imgBLTR[1][0] += extraSpace;
		}
	}

}
void JPDFMAP::scaleChildToPage(vector<vector<double>>& vvdBorder, vector<vector<double>>& vvdFrame)
{
	if (PPKM <= 0.0) { err("Missing PPKM-jpdfmap.scaleChildToPage"); }
	vvdFrame[0][0] *= PPKM;
	vvdFrame[0][1] *= PPKM;
	vvdFrame[1][0] *= PPKM;
	vvdFrame[1][1] *= PPKM;
	for (int ii = 0; ii < vvdBorder.size(); ii++)
	{
		vvdBorder[ii][0] *= PPKM;
		vvdBorder[ii][1] *= PPKM;
	}
}
void JPDFMAP::scaleParentToPage(vector<vector<double>>& parentBorder, vector<vector<double>>& parentFrameKM)
{
	// parentBorder begins in KM, leaves in pixels.
	double xRatio = (parentFrameKM[1][0] - parentFrameKM[0][0]) / (imgBLTR[1][0] - imgBLTR[0][0]);
	double yRatio = (parentFrameKM[1][1] - parentFrameKM[0][1]) / (imgBLTR[1][1] - imgBLTR[0][1]);
	double ratio = max(xRatio, yRatio);
	PPKM = 1.0 / ratio;
	for (int ii = 0; ii < parentBorder.size(); ii++)
	{
		parentBorder[ii][0] *= PPKM;
		parentBorder[ii][1] *= PPKM;
	}
}
void JPDFMAP::setValues(vector<string>& region, vector<vector<double>>& data)
{
	for (int ii = 0; ii < region.size(); ii++) {
		if (region[ii].back() == '!') {
			selIndex = ii;
			region[ii].pop_back();
			break;
		}
	}
	mapData = data;
	mapRegion = region;
}
void JPDFMAP::textBox(vector<vector<double>> boxBLTR, string sText, string alignment)
{
	// This variant uses the default font size.
	textBox(boxBLTR, sText, alignment, fontSize);
}
void JPDFMAP::textBox(vector<vector<double>> boxBLTR, string sText, string alignment, int fontsize)
{
	// Accepted alignment strings are "left", "right", "center", "justify".
	HPDF_STATUS error = HPDF_Page_SetFontAndSize(page, font, fontsize);
	if (error != HPDF_OK) { err("SetFontAndSize-jpdfmap.textBox"); }
	double textWidth = HPDF_Page_TextWidth(page, sText.c_str());
	HPDF_Page_BeginText(page);
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
	if (error != HPDF_OK) { err("TextRect-jpdfmap.textBox"); }
	HPDF_Page_EndText(page);
}