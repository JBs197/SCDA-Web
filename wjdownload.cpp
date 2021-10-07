#include "wjdownload.h"

void WJRCSV::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
	response.setMimeType("csv");
	response.setContentLength(length);
	response.out().write(sCSV.c_str(), length);
}

void WJRPDF::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
	response.setMimeType("pdf");
	string sPDF;
	pdfSize = jpdf.getPDF(sPDF);
	if (!pdfSize)
	{
		error = "WJRPDF ERROR: pdfSize is zero";
		response.out().write(error.c_str(), error.size());
		return;
	}
	response.setContentLength(pdfSize);
	response.out().write(sPDF.c_str(), pdfSize);
}

void WJDOWNLOAD::adjustLineEditWidth()
{
	Wt::WString wsTemp = leFileName->text();
	string temp = wsTemp.toUTF8();
	double dWidth = temp.size() * 10.0;
	double dMax = min(700.0, dWidth);
	leFileName->setMaximumSize(dMax, wlAuto);
}
vector<vector<vector<double>>> WJDOWNLOAD::borderKMToBorderPDF()
{
	vector<vector<vector<double>>> border = mapBorder;
	vector<vector<vector<double>>> frame = mapFrame;
	vector<vector<double>> mapBLTR(2, vector<double>(2));
	mapBLTR[0] = wjrPDFmap->jpdf.cursor;
	vector<double> pageDimensions = wjrPDFmap->jpdf.getPageDimensions();
	mapBLTR[1] = { pageDimensions[0] + wjrPDFmap->jpdf.margin, pageDimensions[1] + wjrPDFmap->jpdf.margin };
	initImgBar(mapBLTR, frame[0]);
	displaceParentToFrame(border[0], frame[0]);
	scaleParentToPage(border[0], frame[0]);
	vector<double> vCenter, dispParentTL;  // dispParent TL is child's TL pixel displacement from parent TL.
	for (int ii = 1; ii < mapRegion.size(); ii++)
	{
		if (mapRegion[ii].back() == '!')
		{
			selIndex = ii;
			mapRegion.pop_back(); 
		}
		dispParentTL = getChildTL(border[ii], frame[ii], frame[0]);
		scaleChildToPage(border[ii], frame[ii]);
		displaceChildToParent(border[ii], frame[ii], dispParentTL);
	}
	vector<double> delta = { imgBLTR[0][0], pageDimensions[1] + wjrPDFmap->jpdf.margin };
	for (int ii = 0; ii < mapRegion.size(); ii++)
	{
		mf.coordReflectY(border[ii], 0.0);
		mf.coordDisplacement(border[ii], delta);
	}
	return border;
}
void WJDOWNLOAD::displaceChildToParent(vector<vector<double>>& vvdBorder, vector<vector<double>>& childFrame, vector<double> dispParentTL)
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
void WJDOWNLOAD::displaceParentToFrame(vector<vector<double>>& parentBorderKM, vector<vector<double>>& parentFrameKM)
{
	for (int ii = 0; ii < parentBorderKM.size(); ii++)
	{
		parentBorderKM[ii][0] -= parentFrameKM[0][0];
		parentBorderKM[ii][1] -= parentFrameKM[0][1];
	}
}
void WJDOWNLOAD::displayCSV(string& sCSV)
{
	if (wjrCSV != nullptr) { wjrCSV.reset(); }
	wjrCSV = makeWJRCSV(sCSV);

	Wt::WString wsCSV = Wt::WString::fromUTF8(sCSV);
	taPreview->setText(wsCSV);
	stackedPreview->setCurrentIndex(0);

	updateStackedPB(selectedMode);
}
void WJDOWNLOAD::displayPDFmap(vector<string>& vsParameter, vector<int>& vChanged)
{
	// Render the active map and its parameter list on a single page. 
	if (wjrPDFmap != nullptr) { wjrPDFmap.reset(); }
	wjrPDFmap = makeWJRPDF();
	int tableFontSize = 12;

	// Firstly, render the parameter section at the bottom.
	vector<int> vColour(vChanged.size());
	for (int ii = 0; ii < vColour.size(); ii++)
	{
		if (vChanged[ii] > 0) 
		{ 
			if (ii % 2 == 1) { vColour[ii] = 4; }
			else { vColour[ii] = 3; }
		}
		else
		{
			if (ii % 2 == 1) { vColour[ii] = 2; }
			else { vColour[ii] = 1; }
		}
	}
	wjrPDFmap->jpdf.setFontSize(tableFontSize);
	wjrPDFmap->jpdf.parameterSectionBottom(vsParameter, vColour);

	// Then, add a coloured region section lacking data values.
	vector<string> vsRegion;
	vsRegion.assign(mapRegion.size(), "");
	int indexTable = wjrPDFmap->jpdf.addTable(2, vsRegion, (double)tableFontSize + 4.0, "Geographic Regions", (double)tableFontSize + 2.0);
	vector<vector<int>> vvColour = { { 1, 1 }, { 2, 2 } };  // White, grey alternating.
	wjrPDFmap->jpdf.vTable[indexTable].setColourBackground(vvColour);
	vector<vector<double>> regionBLTR = wjrPDFmap->jpdf.vTable[indexTable].drawTable();
	wjrPDFmap->jpdf.cursor[0] = regionBLTR[0][0];
	wjrPDFmap->jpdf.cursor[1] = regionBLTR[1][1];
	//wjrPDFmap->jpdf.sectionListBoxed("Geographic Regions", vsRegion, vColour, 2);

	// Convert the raw coordinate data into an HPDF-friendly format.
	vector<vector<vector<double>>> vvvdBorder = borderKMToBorderPDF();
	vector<double> activeData = prepareActiveData();
	unordered_map<string, double> mapRegionData;
	for (int ii = 0; ii < mapRegion.size(); ii++)
	{
		mapRegionData.emplace(mapRegion[ii], activeData[ii]);
	}

	// Insert data and region names into the region section.
	vector<string> vsValue(mapRegion.size());
	string suffix;
	double dVal;
	vsRegion.assign(mapRegion.begin() + 1, mapRegion.end());
	jf.sortAlphabetically(vsRegion);
	if (displayData.size() > 1 || sUnit[0] == '%')
	{
		suffix = " %";
		vsValue[0] = jf.doubleToCommaString(activeData[0], 1) + suffix;
		for (int ii = 0; ii < vsRegion.size(); ii++)
		{
			dVal = mapRegionData.at(vsRegion[ii]);
			vsValue[ii + 1] = jf.doubleToCommaString(dVal, 1) + suffix;
		}
	}
	else if (sUnit[0] == '$')
	{
		suffix = " $";
		vsValue[0] = jf.doubleToCommaString(activeData[0], 0) + suffix;
		for (int ii = 0; ii < vsRegion.size(); ii++)
		{
			dVal = mapRegionData.at(vsRegion[ii]);
			vsValue[ii + 1] = jf.doubleToCommaString(dVal, 0) + suffix;
		}
	}
	else 
	{ 
		vsValue[0] = jf.doubleToCommaString(activeData[0], 0);
		for (int ii = 0; ii < vsRegion.size(); ii++)
		{
			dVal = mapRegionData.at(vsRegion[ii]);
			vsValue[ii + 1] = jf.doubleToCommaString(dVal, 0);
		}
	}
	vsRegion.insert(vsRegion.begin(), mapRegion[0]);
	wjrPDFmap->jpdf.vTable[indexTable].addValues(vsValue);
	wjrPDFmap->jpdf.vTable[indexTable].drawValues(0);
	wjrPDFmap->jpdf.vTable[indexTable].drawColSplit();
	wjrPDFmap->jpdf.vTable[indexTable].addValues(vsRegion);
	wjrPDFmap->jpdf.vTable[indexTable].drawValues(1);

	// Render the legend bar(s).
	drawLegend(activeData);

	// Render the given regions using the data as colour.
	vector<vector<double>> mapColour = getMapColour(activeData);
	drawMap(vvvdBorder, mapColour);

	// Preview screen ... ?
	updateStackedPB(selectedMode);
}
void WJDOWNLOAD::downloadSettings(int mode)
{
	selectedMode = mode;

	// Temporarily disable the download button until the data has been received.
	stackedPB->setDisabled(1);

	// Update the file extension.
	Wt::WString wsTemp;
	if (selectedMode == 3) { wsTemp = ".csv"; }
	else { wsTemp = ".pdf"; }
	textExt->setText(wsTemp);

	// Send out a signal to fetch the requested data.
	previewSignal_.emit(selectedMode);

}
void WJDOWNLOAD::drawLegend(vector<double>& activeData)
{
	if (sUnit.size() < 1) { jf.err("No unit initialized-wtpaint.paintLegendBar"); }
	double width = barBLTR[1][0] - barBLTR[0][0];
	if (width <= 0.0) { jf.err("Invalid barTLBR width-wtpaint.paintLegendBar"); }
	double height = barBLTR[1][1] - barBLTR[0][1];
	if (height <= 0.0) { jf.err("Invalid barTLBR height-wtpaint.paintLegendBar"); }
	bool barVertical = 0;
	int numColour = keyColour.size();
	if (height > width) { barVertical = 1; }
	bool gameOn = 1;

	vector<vector<double>> legendTickMarks = getScaleValues(activeData, numColour);  // Form [child bar, parent bar][ticks].
	legendMin = legendTickMarks[0][0];
	legendMax = legendTickMarks[0][legendTickMarks[0].size() - 1];

	vector<vector<double>> rectBLTR(2, vector<double>(2));
	if (barVertical)
	{
		rectBLTR[0][0] = barBLTR[0][0] + barNumberWidth + (0.5 * barThickness);
		rectBLTR[0][1] = barBLTR[0][1] + barThickness;
		rectBLTR[1][0] = rectBLTR[0][0] + barThickness;
		rectBLTR[1][1] = barBLTR[1][1] - barThickness;
	}
	else
	{
		rectBLTR[0][0] = barBLTR[0][0] + barThickness;
		rectBLTR[1][1] = barBLTR[1][1] - (2.0 * barNumberHeight) - (0.5 * barThickness);
		rectBLTR[1][0] = barBLTR[1][0] - barThickness;
		rectBLTR[0][1] = rectBLTR[1][1] - barThickness;
	}
	drawGradientBar(rectBLTR, 2.0);
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
		drawLegendTicks(rectBLTR, legendTickMarks[0], activeData[0], dotRadius, 2.0);
	}
}
void WJDOWNLOAD::drawLegendTicks(vector<vector<double>> rectBLTR, vector<double>& tickValues, double tickThickness)
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
		if (displayData.size() == 1) { displayUnit = "(" + sUnit + ")"; }  // # of persons
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
		wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);

		temp = vsVal[ii] + suffix;
		if (orientation)
		{
			textBLTR[0][0] = startStop[1][0] - barNumberWidth;
			textBLTR[0][1] = startStop[1][1] + (barNumberHeight * (((double)legendTickLines * 0.5) - 1.0));
			textBLTR[1][0] = startStop[1][0];
			textBLTR[1][1] = textBLTR[0][1] + barNumberHeight;
			wjrPDFmap->jpdf.textBox(textBLTR, temp, "right", legendFontSize);
			if (legendTickLines > 1)
			{
				textBLTR[0][1] -= barNumberHeight;
				textBLTR[1][1] -= barNumberHeight;
				wjrPDFmap->jpdf.textBox(textBLTR, displayUnit, "right", legendFontSize);
			}
		}
		else
		{
			textBLTR[0][0] = startStop[0][0] - (barNumberWidth * 0.5);
			textBLTR[0][1] = startStop[1][1] + (barNumberHeight * ((double)legendTickLines - 1.0));
			textBLTR[1][0] = textBLTR[0][0] + barNumberWidth;
			textBLTR[1][1] = textBLTR[0][1] + barNumberHeight;
			wjrPDFmap->jpdf.textBox(textBLTR, temp, "center", legendFontSize);
			if (legendTickLines > 1)
			{
				textBLTR[0][1] -= barNumberHeight;
				textBLTR[1][1] -= barNumberHeight;
				wjrPDFmap->jpdf.textBox(textBLTR, displayUnit, "center", legendFontSize);
			}
		}
	}

}
void WJDOWNLOAD::drawLegendTicks(vector<vector<double>> rectBLTR, vector<vector<double>>& tickValues, double tickThickness)
{
	// This variant is for the parent bar.
	double dPercent, dTemp, length;
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
		if (displayData.size() == 1) { displayUnit = "(" + sUnit + ")"; }  // # of persons
		else
		{
			suffix = " %";
			displayUnit = "(of population)";
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
				if (dPercent * length > barNumberHeight * 2.0)
				{
					startStop[0][0] = rectBLTR[1][0];
					startStop[1][0] = startStop[0][0] + (0.75 * barThickness);
					startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
					startStop[1][1] = startStop[0][1];
				}
				else
				{
					skew = 1;
					startStop[0][0] = rectBLTR[1][0];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
					startStop[1][1] = startStop[0][1];
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = rectBLTR[0][1] + (barNumberHeight * 2.0);
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[1][1] = startStop[0][1];
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);
				}
			}
			else
			{
				if (dPercent * length > barNumberWidth)
				{
					startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
					startStop[1][0] = startStop[0][0];
					startStop[0][1] = rectBLTR[0][1];
					startStop[1][1] = startStop[0][1] - (0.75 * barThickness);
				}
				else
				{
					skew = 1;
					startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
					startStop[1][0] = startStop[0][0];
					startStop[0][1] = rectBLTR[0][1];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = rectBLTR[0][0] + barNumberWidth;
					startStop[1][1] = startStop[0][1];
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);
				}
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
				wjrPDFmap->jpdf.drawCircle(dot, dRadius, { black, black }, 2.0);
				if ((1.0 - dPercent) * length > barNumberHeight * 2.0)
				{
					startStop[0][0] = rectBLTR[1][0];
					startStop[1][0] = startStop[0][0] + (0.75 * barThickness);
					startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
					startStop[1][1] = startStop[0][1];
				}
				else
				{
					skew = 1;
					startStop[0][0] = rectBLTR[1][0];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[0][1] = rectBLTR[0][1] + (dPercent * length);
					startStop[1][1] = startStop[0][1];
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = rectBLTR[1][1] - (barNumberHeight * 2.0);
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0] + (0.4 * barThickness);
					startStop[1][1] = startStop[0][1];
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);
				}
			}
			else
			{
				dot[0] = rectBLTR[0][0] + (dPercent * length);
				dot[1] = rectBLTR[0][1] + (0.5 * barThickness);
				wjrPDFmap->jpdf.drawCircle(dot, dRadius, { black, black }, 2.0);
				if ((1.0 - dPercent) * length > barNumberWidth)
				{
					startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
					startStop[1][0] = startStop[0][0];
					startStop[0][1] = rectBLTR[0][1];
					startStop[1][1] = startStop[0][1] - (0.75 * barThickness);
				}
				else
				{
					skew = 1;
					startStop[0][0] = rectBLTR[0][0] + (dPercent * length);
					startStop[1][0] = startStop[0][0];
					startStop[0][1] = rectBLTR[0][1];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = rectBLTR[1][0] - barNumberWidth;
					startStop[1][1] = startStop[0][1];
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);

					startStop[0][0] = startStop[1][0];
					startStop[0][1] = startStop[1][1];
					startStop[1][0] = startStop[0][0];
					startStop[1][1] = startStop[0][1] - (0.4 * barThickness);
					wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness);
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
		if (!skew) { wjrPDFmap->jpdf.drawLine(startStop, black, tickThickness); }

		if (ii != 2) { temp = vsVal[ii] + suffix; }
		else { temp = mapRegion[0] + suffix; }
		
		if (orientation)
		{
			textBLTR[0][0] = rectBLTR[1][0] + barThickness;
			textBLTR[0][1] = startStop[1][1] + (barNumberHeight * (((double)legendTickLines * 0.5) - 1.0));
			textBLTR[1][0] = textBLTR[0][0] + barNumberWidth;
			textBLTR[1][1] = textBLTR[0][1] + barNumberHeight;
			wjrPDFmap->jpdf.textBox(textBLTR, temp, "left", legendFontSize);
			if (legendTickLines > 1)
			{
				textBLTR[0][1] -= barNumberHeight;
				textBLTR[1][1] -= barNumberHeight;
				wjrPDFmap->jpdf.textBox(textBLTR, displayUnit, "left", legendFontSize);
			}
		}
		else
		{
			textBLTR[0][0] = startStop[1][0] - (barNumberWidth * 0.5);
			textBLTR[0][1] = rectBLTR[0][1] - (barNumberHeight * 2.0);
			textBLTR[1][0] = textBLTR[0][0] + barNumberWidth;
			textBLTR[1][1] = textBLTR[0][1] + barNumberHeight;
			wjrPDFmap->jpdf.textBox(textBLTR, temp, "center", legendFontSize);
			if (legendTickLines > 1)
			{
				textBLTR[0][1] -= barNumberHeight;
				textBLTR[1][1] -= barNumberHeight;
				wjrPDFmap->jpdf.textBox(textBLTR, displayUnit, "center", legendFontSize);
			}
		}
	}
}
void WJDOWNLOAD::drawLegendTicks(vector<vector<double>> rectBLTR, vector<double>& tickValues, double parentValue, double dotRadius, double tickThickness)
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
		textBLTR[0][0] = startStop[1][0] - (0.5 * barNumberWidth);
		textBLTR[0][1] = startStop[1][1] - barNumberHeight;
		textBLTR[1][0] = textBLTR[0][0] + barNumberWidth;
		textBLTR[1][1] = startStop[1][1];
		alignment = "center";
	}
	wjrPDFmap->jpdf.drawCircle(dot, dotRadius, { black, black }, 2.0);
	wjrPDFmap->jpdf.drawLine(startStop, black, 2.0);
	wjrPDFmap->jpdf.textBox(textBLTR, mapRegion[0], alignment, legendFontSize);
	textBLTR[0][1] -= barNumberHeight;
	textBLTR[1][1] -= barNumberHeight;
	wjrPDFmap->jpdf.textBox(textBLTR, displayUnit, alignment, legendFontSize); 
}
void WJDOWNLOAD::drawGradientBar(vector<vector<double>> rectBLTR, double frameThickness)
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
	if (orientation) { wjrPDFmap->jpdf.drawGradientV(thickStartStop, keyColour, vColourPos); }
	else { wjrPDFmap->jpdf.drawGradientH(thickStartStop, keyColour, vColourPos); }

	if (frameThickness > 0.0)
	{
		wjrPDFmap->jpdf.drawRect(rectBLTR, { 0.0, 0.0, 0.0 }, frameThickness);
	}
}
void WJDOWNLOAD::drawGradientBar(vector<vector<double>> rectBLTR, vector<vector<double>> gradientBLTR, double frameThickness)
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
	if (orientation) { wjrPDFmap->jpdf.drawGradientV(thickStartStop, keyColour, vColourPos); }
	else { wjrPDFmap->jpdf.drawGradientH(thickStartStop, keyColour, vColourPos); }

	if (frameThickness > 0.0)
	{
		wjrPDFmap->jpdf.drawRect(rectBLTR, { 0.0, 0.0, 0.0 }, frameThickness);
	}
}
void WJDOWNLOAD::drawMap(vector<vector<vector<double>>>& vvvdBorder, vector<vector<double>>& vColour)
{
	int numRegion = vvvdBorder.size();
	if (vColour.size() != numRegion) { jf.err("Size mismatch-wjdownload.drawMap"); }
	for (int ii = 0; ii < numRegion; ii++)
	{
		if (mapRegion[ii] == "Canada")  // Special case wherein parent is completely obscured.
		{
			vColour[ii] = { 0.0, 0.0, 0.0 };
		}
		if (ii == selIndex)  // This non-parent region was selected by the user. 
		{
			// DOTLINE
		}
		wjrPDFmap->jpdf.drawRegion(vvvdBorder[ii], vColour[ii]);
	}
}
vector<double> WJDOWNLOAD::getChildTL(vector<vector<double>>& vvdBorder, vector<vector<double>>& childFrameKM, vector<vector<double>>& parentFrameKM)
{
	if (pdfPPKM <= 0.0) { jf.err("No pdfPPKM-wjdownload.getChildTL"); }
	if (childFrameKM.size() != 2) { jf.err("Missing frameKM-wjdownload.getChildTL"); }
	vector<double> vdTL(2);
	vdTL[0] = childFrameKM[0][0] - parentFrameKM[0][0];
	vdTL[0] *= pdfPPKM;
	vdTL[1] = childFrameKM[0][1] - parentFrameKM[0][1];
	vdTL[1] *= pdfPPKM;
	return vdTL;
}
int WJDOWNLOAD::getLegendBarDouble(vector<string>& vsRegion, string sUnit, int displayDataSize)
{
	if (vsRegion.size() > 2 && sUnit == "# of persons" && displayDataSize == 1) { return 1; }
	else if (vsRegion[0] == "Canada") { return 2; }
	else { return 0; }
}
int WJDOWNLOAD::getLegendTickLines(string sUnit)
{
	if (sUnit == "$" || sUnit == "%") { return 1; }
	else { return 2; }
}
vector<vector<double>> WJDOWNLOAD::getMapColour(vector<double>& activeData)
{
	int floor, indexBegin;
	int numRegion = activeData.size();
	vector<vector<double>> vColour(numRegion, vector<double>(3));
	if (legendBarDouble == 1)
	{
		vColour[0] = { 1.0, 1.0, 1.0 };  // Parent.
		indexBegin = 1;
	}
	else { indexBegin = 0; }

	double dR, dG, dB, dVal, percentage, remains;
	if (legendMin == legendMax)  // Every data point is identical ! 
	{
		dVal = activeData[0];
		if (dVal == -1.0)  // Data is missing, so draw as grey.
		{
			for (int ii = indexBegin; ii < numRegion; ii++)
			{
				vColour[ii] = unknownColour;
			}
		}
		else  // There is data, but it's all the same...
		{
			for (int ii = indexBegin; ii < numRegion; ii++)
			{
				vColour[ii] = extraColour;
			}
		}
		return vColour;
	}

	for (int ii = indexBegin; ii < numRegion; ii++)
	{
		dVal = activeData[ii];
		if (dVal == -1.0)  // Data is missing, so draw as grey.
		{
			vColour[ii] = unknownColour;
			continue;
		}
		percentage = (dVal - legendMin) / (legendMax - legendMin);
		if (percentage > 0.9999) { percentage = 0.9999; }
		percentage *= (double)(keyColour.size() - 1);
		floor = int(percentage);
		remains = percentage - (double)floor;
		dR = (keyColour[floor + 1][0] - keyColour[floor][0]) * remains;
		dG = (keyColour[floor + 1][1] - keyColour[floor][1]) * remains;
		dB = (keyColour[floor + 1][2] - keyColour[floor][2]) * remains;
		vColour[ii][0] = keyColour[floor][0] + dR;
		vColour[ii][1] = keyColour[floor][1] + dG;
		vColour[ii][2] = keyColour[floor][2] + dB;
	}

	return vColour;
}
int WJDOWNLOAD::getRadioIndex()
{
	int index = -1;
	if (wbGroup != nullptr)
	{
		index = wbGroup->checkedId();
	}
	return index;
}
vector<vector<double>> WJDOWNLOAD::getScaleValues(vector<double>& activeData, int numTicks)
{
	// Despite returning doubles, this function tries to return whole numbers if possible.
	vector<vector<double>> ticks(1, vector<double>(numTicks));
	vector<double> activeDataChildren;
	vector<int> indexMinMax;
	int iMinScale, iMaxScale, minDivisor, divisor, quotient, remainder, iScaleWidth;
	double dMin = -1.0, dMax = -1.0;
	if (legendBarDouble == 0 || legendBarDouble == 2)
	{
		indexMinMax = jf.minMax(activeData);
		dMin = activeData[indexMinMax[0]];
		dMax = activeData[indexMinMax[1]];
	}
	else if (legendBarDouble == 1)  // Form [child bar, parent bar][ticks].
	{
		ticks.resize(2);
		ticks[1].resize(4);  // Form [child bar min, child bar max, parent, parent bar max].
		activeDataChildren.assign(activeData.begin() + 1, activeData.end());
		indexMinMax = jf.minMax(activeDataChildren);
		dMin = activeDataChildren[indexMinMax[0]];
		dMax = activeDataChildren[indexMinMax[1]];
	}
	else { jf.err("Missing legendBarDouble.wjdownload.getScaleValues"); }

	if (dMin < 0.0) { dMin = 0.0; }
	if (dMax < 0.0) { dMax = 0.0; }
	int iMin = floor(dMin);
	int iMax = ceil(dMax);

	int iNum = iMin;
	int minDigits = 1;
	while (1)
	{
		iNum /= 10;
		if (iNum > 0) { minDigits++; }
		else { break; }
	}
	iNum = iMax;
	int maxDigits = 1;
	while (1)
	{
		iNum /= 10;
		if (iNum > 0) { maxDigits++; }
		else { break; }
	}

	string sMin = "1";
	for (int ii = 0; ii < minDigits - 1; ii++)
	{
		sMin += "0";
	}
	minDivisor = stoi(sMin);
	string sMax = "1";
	for (int ii = 0; ii < maxDigits - 1; ii++)
	{
		sMax += "0";
	}
	divisor = stoi(sMax);

	double idle = 1.0;
	do
	{
		quotient = iMin / minDivisor;
		iMinScale = quotient * minDivisor;

		quotient = iMax / divisor;
		remainder = iMax % divisor;
		if (remainder != 0) { quotient++; }
		iMaxScale = quotient * divisor;

		idle = 1.0 - ((double)(iMax - iMin) / (double)(iMaxScale - iMinScale));
		if (minDivisor <= 2 && divisor <= 2)
		{
			iMaxScale = iMax;
			iMinScale = iMin;
			break;
		}
		else if (minDivisor > 2) { minDivisor /= 2; }
		else if (divisor > 2) { divisor /= 2; }
	} while (idle > legendIdleThreshold);

	if (maxDigits > minDigits + 1) { iMinScale = 0; }

	int iBandWidth = (iMaxScale - iMinScale) / (numTicks - 1);
	int remainderAsIs = iMaxScale - ((numTicks - 1) * iBandWidth) - iMinScale;
	if (iBandWidth == 0)
	{
		double dBandWidth = (double)(iMaxScale - iMinScale) / (double)(numTicks - 1);
		for (int ii = 1; ii < numTicks - 1; ii++)
		{
			ticks[0][ii] = ((double)ii * dBandWidth) + (double)iMinScale;
		}
	}
	else if (iMinScale == 0)
	{
		iMaxScale -= remainderAsIs;
		for (int ii = 1; ii < numTicks - 1; ii++)
		{
			ticks[0][ii] = (double)(ii * iBandWidth);
		}
	}
	else
	{
		if (iBandWidth > iMinScale)
		{
			iNum = iBandWidth / iMinScale;
			int lowScale = iNum * iMinScale;
			int highScale = (iNum + 1) * iMinScale;
			if (abs(iBandWidth - lowScale) < abs(iBandWidth - highScale)) { iScaleWidth = lowScale; }
			else { iScaleWidth = highScale; }
		}
		else { iScaleWidth = iBandWidth; }
		int maxDist, minDist;
		remainderAsIs = iMaxScale - ((numTicks - 1) * iScaleWidth) - iMinScale;
		if (remainderAsIs > 0)  // Scale bar is too long.
		{
			maxDist = iMaxScale - iMax;
			minDist = iMin - iMinScale;
			while (maxDist > minDist && remainderAsIs > 0)
			{
				iMaxScale--;
				remainderAsIs--;
				maxDist--;
			}
			while (maxDist < minDist && remainderAsIs > 0)
			{
				iMinScale++;
				remainderAsIs--;
				minDist--;
			}
			if (remainderAsIs > 0)
			{
				iMinScale += remainderAsIs / 2;
				iMaxScale -= remainderAsIs / 2;
				iMaxScale -= remainderAsIs % 2;
			}
		}
		else if (remainderAsIs < 0)  // Scale bar is too short.
		{
			if (iMinScale == 0) { iMaxScale -= remainderAsIs; }
			else if (iMaxScale == 100) { iMinScale += remainderAsIs; }
			else
			{
				maxDist = iMaxScale - iMax;
				minDist = iMin - iMinScale;
				while (maxDist > minDist && remainderAsIs < 0)
				{
					iMinScale--;
					remainderAsIs++;
					minDist++;
				}
				while (maxDist < minDist && remainderAsIs < 0)
				{
					iMaxScale++;
					remainderAsIs++;
					maxDist++;
				}
				if (remainderAsIs < 0)
				{
					iMinScale -= remainderAsIs / 2;
					iMaxScale += remainderAsIs / 2;
					iMaxScale += remainderAsIs % 2;
				}
			}

			if (iMinScale < 0)
			{
				iMaxScale -= iMinScale;
				iMinScale = 0;
			}
		}
		for (int ii = 1; ii < numTicks - 1; ii++)
		{
			ticks[0][ii] = (double)((ii * iScaleWidth) + iMinScale);
		}
	}
	ticks[0][0] = (double)iMinScale;
	ticks[0][numTicks - 1] = (double)iMaxScale;
	if (legendBarDouble == 0 || legendBarDouble == 2) { return ticks; }

	ticks[1][0] = ticks[0][0];
	ticks[1][1] = ticks[0][numTicks - 1];

	double dMaxParent = activeData[0];
	ticks[1][2] = ceil(dMaxParent);
	iNum = (int)ticks[1][2];
	maxDigits = 1;
	while (1)
	{
		iNum /= 10;
		if (iNum > 0) { maxDigits++; }
		else { break; }
	}

	string sMaxParent = "1";
	for (int ii = 0; ii < maxDigits - 1; ii++)
	{
		sMaxParent += "0";
	}
	divisor = stoi(sMaxParent);
	quotient = (int)ticks[1][2] / divisor;
	quotient++;
	ticks[1][3] = (double)(quotient * divisor);

	return ticks;
}
void WJDOWNLOAD::init()
{
	initColour();
	this->setMaximumSize(maxWidth, maxHeight);
	auto vLayout = make_unique<Wt::WVBoxLayout>();

	auto fileNameBox = makeFileNameBox();
	vLayout->addWidget(move(fileNameBox));

	auto hLayout = make_unique<Wt::WHBoxLayout>();
	auto downloadBox = makeDownloadBox();
	hLayout->addWidget(move(downloadBox));

	auto stackedPreviewUnique = make_unique<Wt::WStackedWidget>();
	stackedPreview = stackedPreviewUnique.get();
	auto textArea = make_unique<Wt::WTextArea>();
	taPreview = textArea.get();
	stackedPreview->addWidget(move(textArea));
	hLayout->addWidget(move(stackedPreviewUnique));
	
	vLayout->addLayout(move(hLayout));
	vLayout->addStretch(1);
	this->setLayout(move(vLayout));
}
void WJDOWNLOAD::initColour()
{
	wcBlack = Wt::WColor(0, 0, 0);
	wcSelected = Wt::WColor(200, 200, 255);
	wcWhite = Wt::WColor(255, 255, 255);
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
void WJDOWNLOAD::initImgBar(vector<vector<double>>& mapBLTR, vector<vector<double>>& parentFrameKM)
{
	// mapBLTR has a unit of pixels, while parentFrameKM has a unit of KM.
	if (legendBarDouble < 0 || legendTickLines < 0) { jf.err("Missing legend init-scaleImgBar"); }
	double parentWidthKM = parentFrameKM[1][0] - parentFrameKM[0][0];
	double parentHeightKM = parentFrameKM[1][1] - parentFrameKM[0][1];
	double parentAspectRatio = parentWidthKM / parentHeightKM;
	double dWidth = mapBLTR[1][0] - mapBLTR[0][0];
	double dHeight = mapBLTR[1][1] - mapBLTR[0][1];
	double margin = wjrPDFmap->jpdf.margin;
	barBLTR.resize(2, vector<double>(2));
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
			barBLTR[0][0] = imgBLTR[1][0] + 1.0;
			barBLTR[1][1] = imgBLTR[1][1];
			barBLTR[1][0] = barBLTR[0][0] + barSpaceHorizontal;
			barBLTR[0][1] = imgBLTR[0][1];
		}
		else
		{
			// The limiting factor will be the parent image's height.
			imgBLTR[0][1] = mapBLTR[0][1];
			imgBLTR[1][0] = imgBLTR[0][0] + ((imgBLTR[1][1] - imgBLTR[0][1]) * parentAspectRatio);
			barBLTR[0][0] = imgBLTR[1][0] + 1.0;
			barBLTR[0][1] = imgBLTR[0][1];
			barBLTR[1][0] = barBLTR[0][0] + barSpaceHorizontal;
			barBLTR[1][1] = imgBLTR[1][1];

			// Automatically apply horizontal centering.
			extraSpace = mapBLTR[1][0] - barBLTR[1][0];
			extraSpace /= 2.0;
			imgBLTR[0][0] += extraSpace;
			imgBLTR[1][0] += extraSpace;
			barBLTR[0][0] += extraSpace;
			barBLTR[1][0] += extraSpace;
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
			barBLTR[1][1] = imgBLTR[0][1] - 1.0;
			barBLTR[0][1] = barBLTR[1][1] - barSpaceVertical;
			barBLTR[0][0] = imgBLTR[0][0];
			barBLTR[1][0] = imgBLTR[1][0];
		}
		else
		{
			// The limiting factor will be the parent image's height.
			barBLTR[0][1] = mapBLTR[0][1];
			barBLTR[1][1] = barBLTR[0][1] + barSpaceVertical;
			barBLTR[0][0] = imgBLTR[0][0];
			barBLTR[1][0] = mapBLTR[1][0];
			imgBLTR[0][1] = barBLTR[1][1] + 1.0;
			imgBLTR[1][0] = imgBLTR[0][0] + ((imgBLTR[1][1] - imgBLTR[0][1]) * parentAspectRatio);

			// Automatically apply horizontal centering.
			extraSpace = mapBLTR[1][0] - imgBLTR[1][0];
			extraSpace /= 2.0;
			imgBLTR[0][0] += extraSpace;
			imgBLTR[1][0] += extraSpace;
		}
	}

}
void WJDOWNLOAD::initJPDF(JPDF& jpdf)
{
	auto app = Wt::WApplication::instance();
	string fontPath = app->docRoot() + "/font/framd.ttf";
	string fontPathItalic = app->docRoot() + "/font/framdit.ttf";
	jpdf.setFont(fontPath, fontPathItalic);
}
void WJDOWNLOAD::initMap(vector<string>& region, vector<vector<vector<double>>>& frame, vector<vector<vector<double>>>& border, vector<vector<double>>& data)
{
	// Save the raw pixel data from the active map, for possible rendering later.
	mapRegion = region;
	mapFrame = frame;
	mapBorder = border;
	auto scaleBar = make_unique<JSCALEBAR>();
	for (int ii = 0; ii < data.size(); ii++)
	{
		scaleBar->addDataset(data[ii], )
	}
}
Wt::WString WJDOWNLOAD::initStyleCSS(shared_ptr<Wt::WMemoryResource>& wmrCSS)
{
	// Complete CSS style sheets for PDF rendering.
	vector<unsigned char> binCSS = wmrCSS->data();
	string sStyle;
	sStyle.resize(binCSS.size());
	for (int ii = 0; ii < binCSS.size(); ii++)
	{
		sStyle[ii] = (char)binCSS[ii];
	}
	Wt::WString wsStyle = Wt::WString::fromUTF8(sStyle);
	return wsStyle;
}
unique_ptr<Wt::WContainerWidget> WJDOWNLOAD::makeDownloadBox()
{
	auto box = make_unique<Wt::WContainerWidget>();
	box->setMaximumSize(120.0, wlAuto);
	auto layoutUnique = make_unique<Wt::WVBoxLayout>();
	auto layout = box->setLayout(move(layoutUnique));

	auto group = make_shared<Wt::WButtonGroup>();
	swap(group, wbGroup);

	Wt::WRadioButton* wrb = nullptr;
	wrb = layout->addWidget(make_unique<Wt::WRadioButton>("All (PDF)"));
	wrb->setInline(0);
	wbGroup->addButton(wrb);
	wrb = layout->addWidget(make_unique<Wt::WRadioButton>("Bar Graph (PDF)"));
	wrb->setInline(0);
	wbGroup->addButton(wrb);
	wrb = layout->addWidget(make_unique<Wt::WRadioButton>("Data Map (PDF)"));
	wrb->setInline(0);
	wbGroup->addButton(wrb);
	//wrb = layout->addWidget(make_unique<Wt::WRadioButton>("Data Table (PDF)"));
	//wrb->setInline(0);
	//wbGroup->addButton(wrb);
	wrb = layout->addWidget(make_unique<Wt::WRadioButton>("Data Table (CSV)"));
	wrb->setInline(0);
	wbGroup->addButton(wrb);
	wbGroup->checkedChanged().connect([=](Wt::WRadioButton* wrb)
		{
			Wt::WButtonGroup* wbg = wrb->group();
			int index = wbg->id(wrb);
			downloadSettings(index);
		});

	auto boxDummy = make_unique<Wt::WContainerWidget>();
	boxDummy->setMinimumSize(wlAuto, 20.0);
	boxDummy->setMaximumSize(wlAuto, 20.0);
	layout->addWidget(move(boxDummy));

	auto dlUnique = make_unique<Wt::WStackedWidget>();
	dlUnique->setMinimumSize(wlAuto, 42.0);
	for (int ii = 0; ii < numDLtypes; ii++)
	{
		auto pbUnique = make_unique<Wt::WPushButton>("Download");
		pbUnique->decorationStyle().setBackgroundColor(wcSelected);
		dlUnique->addWidget(move(pbUnique));
	}
	dlUnique->setCurrentIndex(0);
	dlUnique->setDisabled(1);
	stackedPB = layout->addWidget(move(dlUnique), 0, Wt::AlignmentFlag::Center);

	layout->addStretch(1);
	return box;
}
unique_ptr<Wt::WContainerWidget> WJDOWNLOAD::makeFileNameBox()
{
	auto box = make_unique<Wt::WContainerWidget>();
	auto fileNameLayout = make_unique<Wt::WHBoxLayout>();
	auto fileName = make_unique<Wt::WText>("Filename: ");
	fileNameLayout->addWidget(move(fileName), 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Middle);
	auto lineEditUnique = make_unique<Wt::WLineEdit>();
	lineEditUnique->setText("SCDA Default");
	lineEditUnique->setMaximumSize(120.0, wlAuto);
	lineEditUnique->textInput().connect(this, &WJDOWNLOAD::adjustLineEditWidth);
	leFileName = fileNameLayout->addWidget(move(lineEditUnique));
	auto fileExtension = make_unique<Wt::WText>(".pdf");
	textExt = fileNameLayout->addWidget(move(fileExtension), 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Middle);
	fileNameLayout->addStretch(1);
	box->setLayout(move(fileNameLayout));
	return box;
}
shared_ptr<WJRCSV> WJDOWNLOAD::makeWJRCSV(string& sCSV)
{
	Wt::WString wsFileName = leFileName->text() + textExt->text();
	auto csvTemp = make_shared<WJRCSV>(wsFileName);
	auto buffer = new unsigned char[1];
	csvTemp->setData(buffer, 1);
	csvTemp->length = sCSV.size();
	csvTemp->sCSV = sCSV;
	delete[] buffer;
	return csvTemp;
}
shared_ptr<WJRPDF> WJDOWNLOAD::makeWJRPDF()
{
	Wt::WString wsFileName = leFileName->text() + textExt->text();
	auto pdfTemp = make_shared<WJRPDF>(wsFileName);
	initJPDF(pdfTemp->jpdf);  // Font.
	return pdfTemp;
}
vector<double> WJDOWNLOAD::prepareActiveData()
{
	if (displayData.size() < 1) { jf.err("Missing displayData-wjdownload.prepareActiveData"); }
	size_t numRegion = mapRegion.size();
	if (numRegion != mapData[0].size()) { jf.err("Area size mismatch-wjdownload.prepareActiveData"); }
	vector<double> activeData;
	if (displayData.size() > 1) { activeData = processPercent(); }
	else { activeData = mapData[0]; }
	return activeData;
}
vector<double> WJDOWNLOAD::processPercent()
{
	if (displayData.size() > mapData.size()) { jf.err("Parameter mismatch-wjdownload.processPercent"); }
	int dataSize = mapData[displayData[0]].size();
	vector<double> vdPercent(dataSize);
	for (int ii = 0; ii < dataSize; ii++)
	{
		vdPercent[ii] = 100.0 * mapData[displayData[0]][ii] / mapData[displayData[1]][ii];
	}
	return vdPercent;
}
void WJDOWNLOAD::scaleChildToPage(vector<vector<double>>& vvdBorder, vector<vector<double>>& vvdFrame)
{
	if (pdfPPKM <= 0.0) { jf.err("Missing pdfPPKM-wjdownload.scaleChildToPage"); }
	vvdFrame[0][0] *= pdfPPKM;
	vvdFrame[0][1] *= pdfPPKM;
	vvdFrame[1][0] *= pdfPPKM;
	vvdFrame[1][1] *= pdfPPKM;
	for (int ii = 0; ii < vvdBorder.size(); ii++)
	{
		vvdBorder[ii][0] *= pdfPPKM;
		vvdBorder[ii][1] *= pdfPPKM;
	}
}
void WJDOWNLOAD::scaleParentToPage(vector<vector<double>>& parentBorder, vector<vector<double>>& parentFrameKM)
{
	// parentBorder begins in KM, leaves in pixels.
	double xRatio = (parentFrameKM[1][0] - parentFrameKM[0][0]) / (imgBLTR[1][0] - imgBLTR[0][0]);
	double yRatio = (parentFrameKM[1][1] - parentFrameKM[0][1]) / (imgBLTR[1][1] - imgBLTR[0][1]);
	double ratio = max(xRatio, yRatio);
	pdfPPKM = 1.0 / ratio;
	for (int ii = 0; ii < parentBorder.size(); ii++)
	{
		parentBorder[ii][0] *= pdfPPKM;
		parentBorder[ii][1] *= pdfPPKM;
	}
}
void WJDOWNLOAD::setUnit(string unit, vector<int> viIndex)
{
	sUnit = unit;
	displayData = viIndex;
	legendBarDouble = getLegendBarDouble(mapRegion, unit, viIndex.size());
	legendTickLines = getLegendTickLines(unit);
}
Wt::WString WJDOWNLOAD::stringToHTML(string& sLine)
{
	Wt::WString wsLine = "<p>" + Wt::WString::fromUTF8(sLine + "\n") + "</p>";
	return wsLine;
}
void WJDOWNLOAD::updateStackedPB(int index)
{
	// This function specifically re-creates the pushButton within the "download"
	// stacked widget, using the latest WJR struct. The pushButton must be recreated
	// (rather than relinked) due to the nature of (or a bug in) WT.
	Wt::WPushButton* pbDownload = (Wt::WPushButton*)stackedPB->widget(index);
	auto oldUnique = stackedPB->removeWidget(pbDownload);
	Wt::WString wsLabel = oldUnique->text();
	Wt::WCssDecorationStyle& style = oldUnique->decorationStyle();
	auto pbUnique = make_unique<Wt::WPushButton>();
	switch (index)
	{
	case 2:
		pbUnique->setLink(Wt::WLink(wjrPDFmap));
		break;
	case 3:
		pbUnique->setLink(Wt::WLink(wjrCSV));
		break;
	}	
	pbUnique->setText(wsLabel);
	pbUnique->setDecorationStyle(style);
	stackedPB->insertWidget(index, move(pbUnique));
	stackedPB->setCurrentIndex(index);
	stackedPB->setDisabled(0);
}
