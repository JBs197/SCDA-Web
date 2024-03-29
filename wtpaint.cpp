#include "wtpaint.h"

using namespace std;

int WTPAINT::adjustScale(vector<vector<int>>& scaleValues, string& sUnit)
{
	// If scale values are very large, they will be reduced by changing the unit.
	// Returns the prefix: 0 = normal, 1 = thousand, 2 = million, 3 = billion, etc.
	int index = 0, prefix = -1;
	if (scaleValues[0][0] == 0) { index = 1; }
	int quotient = scaleValues[0][index];
	while (quotient > 0)
	{
		prefix++;
		quotient /= 1000;
	}
	
	switch (prefix)
	{
	case 0:  // No changes will be made.
		break;
	case 1:
		for (int ii = 0; ii < scaleValues.size(); ii++)
		{
			for (int jj = 0; jj < scaleValues[ii].size(); jj++)
			{
				scaleValues[ii][jj] /= 1000;
			}
		}
		sUnit = "thousand persons";
		break;
	case 2:
		for (int ii = 0; ii < scaleValues.size(); ii++)
		{
			for (int jj = 0; jj < scaleValues[ii].size(); jj++)
			{
				scaleValues[ii][jj] /= 1000000;
			}
		}
		sUnit = "million persons";
		break;
	case 3:
		for (int ii = 0; ii < scaleValues.size(); ii++)
		{
			for (int jj = 0; jj < scaleValues[ii].size(); jj++)
			{
				scaleValues[ii][jj] /= 1000000000;
			}
		}
		sUnit = "billion persons";
		break;
	}
	return prefix;
}
string WTPAINT::areaClicked(int index)
{
	if (index == areaName.size()) { return "bgArea"; }
	return areaName[index];
}
void WTPAINT::clearAreas()
{
	auto listAreas = Wt::WPaintedWidget::areas();
	for (int ii = 0; ii < listAreas.size(); ii++)
	{
		Wt::WPaintedWidget::removeArea(listAreas[ii]);
	}
	area.clear();
}
vector<string> WTPAINT::delinearizeTitle(string& linearTitle)
{
	vector<string> title;
	string temp;
	size_t pos1 = 0;
	size_t pos2 = linearTitle.find('@');
	while (pos2 < linearTitle.size())
	{
		temp = linearTitle.substr(pos1, pos2 - pos1);
		title.push_back(temp);
		pos1 = pos2 + 1;
		pos2 = linearTitle.find('@', pos1);
	}
	temp = linearTitle.substr(pos1);
	title.push_back(temp);
	return title;
}
vector<Wt::WPointF> WTPAINT::displaceChildToParent(vector<vector<double>>& vvdBorder, vector<double>& childTL, vector<double> dispParentTL)
{
	vector<Wt::WPointF> borderChild(vvdBorder.size());
	vector<double> vdShift(2);
	vdShift[0] = dispParentTL[0] - childTL[0];
	vdShift[1] = dispParentTL[1] - childTL[1];
	for (int ii = 0; ii < vvdBorder.size(); ii++)
	{
		borderChild[ii].setX(vvdBorder[ii][0] + vdShift[0]);
		borderChild[ii].setY(vvdBorder[ii][1] + vdShift[1]);
	}
	return borderChild;
}
void WTPAINT::displaceParentToWidget(vector<vector<vector<double>>>& vvvdBorder, vector<vector<double>>& parentFrameKM)
{
	for (int ii = 0; ii < vvvdBorder[0].size(); ii++)
	{
		vvvdBorder[0][ii][0] -= parentFrameKM[0][0];
		vvvdBorder[0][ii][1] -= parentFrameKM[0][1];
	}
}
vector<Wt::WPolygonArea*> WTPAINT::drawMap(vector<string>& vsRegion, vector<vector<vector<double>>>& vvvdFrame, vector<vector<vector<double>>>& vvvdBorder)
{
	if (vvvdBorder.size() < 2 || vsRegion.size() < 2) { err("Less than two regions given-wtpaint.drawMap"); }
	if (legendBarDouble < 0 || legendTickLines < 1) { err("No legend initialization-wtpaint.drawMap"); }
	int indexScale = jsb.getActiveIndex();
	if (indexScale < 0) { err("jscalebar missing init-wtpaint.drawMap"); }

	vector<vector<double>> parentFrameKM = vvvdFrame[0];
	scaleImgBar(parentFrameKM);

	border.resize(vsRegion.size());  // Form [parent, children].
	displaceParentToWidget(vvvdBorder, parentFrameKM); 
	border[0] = scaleParentToWidget(vvvdBorder, parentFrameKM);
	area.resize(vsRegion.size());  // Form [parent, children].
	areaName = vsRegion;  // Form [parent, children].

	vector<vector<double>> childFrameKM;
	vector<double> dispParentTL;  // Child's TL widget pixel displacement from (0,0). 
	for (int ii = 1; ii < areaName.size(); ii++)
	{
		if (areaName[ii].back() == '!')
		{
			selIndex = ii;
			areaName[ii].pop_back();
		}
		childFrameKM = vvvdFrame[ii];
		dispParentTL = getChildTL(vvvdBorder[ii], childFrameKM, parentFrameKM);
		scaleChildToWidget(vvvdBorder[ii], childFrameKM);
		border[ii] = displaceChildToParent(vvvdBorder[ii], childFrameKM[0], dispParentTL);		
	}

	initColour();
	makeAreas();
	updateAreaColour();
	update();
	return area;
}
void WTPAINT::err(string message)
{
	string errorMessage = "WTPAINT error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
vector<double> WTPAINT::getChildTL(vector<vector<double>>& vvdBorder, vector<vector<double>>& childFrameKM, vector<vector<double>>& parentFrameKM)
{
	if (widgetPPKM <= 0.0) { err("No widgetPPKM-wtpaint.getChildTL"); }
	if (childFrameKM.size() != 2) { err("Missing frameKM-wtpaint.getChildTL"); }
	vector<double> vdTL(2);
	vdTL[0] = childFrameKM[0][0] - parentFrameKM[0][0];
	vdTL[0] *= widgetPPKM;
	vdTL[1] = childFrameKM[0][1] - parentFrameKM[0][1];
	vdTL[1] *= widgetPPKM;
	return vdTL;
}
vector<vector<int>> WTPAINT::getFrame(vector<Wt::WPointF>& path)
{
	vector<vector<double>> TLBRd(2, vector<double>(2));
	TLBRd[0][0] = path[0].x();
	TLBRd[1][0] = path[0].x();
	TLBRd[0][1] = path[0].y();
	TLBRd[1][1] = path[0].y();
	for (int ii = 1; ii < path.size(); ii++)
	{
		if (path[ii].x() < TLBRd[0][0]) { TLBRd[0][0] = path[ii].x(); }
		else if (path[ii].x() > TLBRd[1][0]) { TLBRd[1][0] = path[ii].x(); }
		if (path[ii].y() < TLBRd[0][1]) { TLBRd[0][1] = path[ii].y(); }
		else if (path[ii].y() > TLBRd[1][1]) { TLBRd[1][1] = path[ii].y(); }
	}
	vector<vector<int>> TLBR;
	jnumber.toInt(TLBRd, TLBR);
	return TLBR;
}
vector<double> WTPAINT::getDimensions()
{
	vector<double> vdDim = { barTLBR[1][0], barTLBR[1][1] };
	return vdDim;
}
vector<vector<string>> WTPAINT::getGraphData()
{
	// Return form [region index][sRegion, sData]
	vector<vector<string>> vvsData(areaName.size(), vector<string>(2));
	double dNum;
	for (int ii = 0; ii < vvsData.size(); ii++)
	{
		vvsData[ii][0] = areaName[ii];
		dNum = jsb.getDatasetValue(-1, ii);
		vvsData[ii][1] = to_string(dNum);
	}
	return vvsData;
}
int WTPAINT::getLegendBarDouble(vector<string>& vsRegion, string sUnit, int displayDataSize)
{
	if (vsRegion.size() > 2 && sUnit == "# of persons" && displayDataSize == 1) { return 1; }
	else if (vsRegion[0] == "Canada") { return 2; }
	else { return 0; }
}
int WTPAINT::getLegendTickLines(string sUnit)
{
	if (sUnit == "$" || sUnit == "%") { return 1; }
	else { return 2; }
}
void WTPAINT::initColour()
{
	int numColour = 6;
	int depthColour = 3;
	keyColour.resize(numColour);
	keyColour[0] = { 255, 0, 0 };  // Red
	keyColour[1] = { 255, 255, 0 };  // Yellow
	keyColour[2] = { 0, 255, 0 };  // Green
	keyColour[3] = { 0, 255, 255 };  // Teal
	keyColour[4] = { 0, 0, 255 };  // Blue
	keyColour[5] = { 127, 0, 255 };  // Violet
	extraColour = { 255, 0, 127 };  // Pink
	unknownColour = { 180, 180, 180 };  // Grey

	keyColourDouble.resize(numColour, vector<double>(depthColour));
	for (int ii = 0; ii < numColour; ii++)
	{
		for (int jj = 0; jj < depthColour; jj++)
		{
			keyColourDouble[ii][jj] = (double)keyColour[ii][jj] / 255.0;
		}
	}
	extraColourDouble.resize(depthColour);
	unknownColourDouble.resize(depthColour);
	for (int ii = 0; ii < depthColour; ii++)
	{
		extraColourDouble[ii] = (double)extraColour[ii] / 255.0;
		unknownColourDouble[ii] = (double)unknownColour[ii] / 255.0;
	}

	mapTooltip.emplace("grey", "A grey region indicates missing source data.");
	mapTooltip.emplace("pink", "A pink region indicates that all regions are displaying the same data value.");
	mapTooltip.emplace("grandparent", "Clicking outside the parent region's border will load that region's parent, if it is available");
}
void WTPAINT::paintEvent(Wt::WPaintDevice* paintDevice)
{
	Wt::WPainter painter(paintDevice);
	Wt::WLength penWidth(2.0, Wt::LengthUnit::Pixel);
	Wt::WPen pen(Wt::PenStyle::SolidLine);
	pen.setWidth(penWidth);
	painter.setPen(pen);
	if (area.size() < 1) { return; }
	painter.save();
	paintLegendBar(painter);
	painter.restore();
	updateAreaColour();
	paintRegionAll(painter);
	resize(barTLBR[1][0] + 1.0, barTLBR[1][1] + 1.0);
}
void WTPAINT::paintLegendBar(Wt::WPainter& painter)
{
	if (legendBarDouble < 0) { err("Missing legendBarDouble-wtpaint.paintLegendBar"); }
	int numColour = numColourBands + 1;
	if (keyColour.size() != numColour) { initColour(); }
	double width = barTLBR[1][0] - barTLBR[0][0];
	if (width <= 0.0) { err("Invalid barTLBR width-wtpaint.paintLegendBar"); }
	double height = barTLBR[1][1] - barTLBR[0][1];
	if (height <= 0.0) { err("Invalid barTLBR height-wtpaint.paintLegendBar"); }
	bool barVertical = 0;
	if (height > width) { barVertical = 1; }
	bool gameOn = 1;
	string sUnit = jsb.getUnit(-1);
	int decimalPlaces = jsb.getDecimalPlaces(-1);

	double dLen, dLenParent, wGradX0, wGradX1, wGradY0, wGradY1, tickX, tickY, tickXP, tickYP;
	double dTemp, wGradX0P, wGradX1P, wGradY0P, wGradY1P, xCoord, yCoord, coordMin, coordMax;
	Wt::WRectF rectColour, rectColourParent, rectWhiteParent, rectDot;
	Wt::WGradient wGrad, wGradParent;
	Wt::WColor wColour;
	Wt::WString wsName, wsValue, wsUnit, wsTemp;
	vector<string> vsList, vsListCentered;
	string temp, displayUnit, suffix;
	int widthDiff;

	vector<vector<double>> scaleValues(1, vector<double>());  // Form [child bar, parent bar][ticks].
	switch (legendBarDouble)
	{
	case 0:
		scaleValues[0] = jsb.getTickValues(-1, numColour);  
		break;
	case 1:
		scaleValues.resize(2, vector<double>());
		scaleValues[0] = jsb.getTickValues(-1, numColour, { 0 }); 
		scaleValues[1].resize(4);
		scaleValues[1][0] = scaleValues[0][0];
		scaleValues[1][1] = scaleValues[0][scaleValues[0].size() - 1];
		dTemp = jsb.getDatasetValue(-1, 0);  // Parent value.
		if (dTemp >= scaleValues[1][1]) {
			scaleValues[1][2] = dTemp;
		}
		else {
			scaleValues[1][2] = scaleValues[1][1];
			scaleValues[1][1] = dTemp;
		}
		scaleValues[1][3] = jnumber.roundingCeil(scaleValues[1][2]);
		break;
	case 2:
		scaleValues[0] = jsb.getTickValues(-1, numColour);
		break;
	}
	legendMin = scaleValues[0][0];
	legendMax = scaleValues[0].back();

	if (legendTickLines == 1)  {  suffix = " (" + sUnit + ")"; }
	else
	{
		if (sUnit[0] != '%') { displayUnit = "(" + sUnit + ")"; }  // # of persons
		else 
		{ 
			suffix = " %";
			displayUnit = "(of population)"; 
		}
	}

	if (barVertical)
	{
		dLen = barTLBR[1][1] - (2.0 * barThickness);
		rectColour = Wt::WRectF(barTLBR[0][0] + barNumberWidth + (0.5 * barThickness), barThickness, barThickness, dLen);
		
		wGradX0 = 1.5 * barThickness;
		wGradX1 = wGradX0;
		wGradY0 = dLen + barThickness - 1.0;
		wGradY1 = barThickness + 1.0;
		tickX = barTLBR[0][0] + barNumberWidth;
		xCoord = barTLBR[0][0] - 2.0;

		if (legendBarDouble == 1)
		{
			dTemp = (double)(scaleValues[1][1] - scaleValues[1][0]) / (double)(scaleValues[1][3] - scaleValues[1][0]);
			dLenParent = dTemp * dLen;
			rectColourParent = Wt::WRectF(barTLBR[1][0] - barNumberWidth - (2.0 * barThickness), barThickness + dLen - dLenParent, barThickness, dLenParent);
			rectWhiteParent = Wt::WRectF(barTLBR[1][0] - barNumberWidth - (2.0 * barThickness), barThickness, barThickness, dLen - dLenParent);

			wGradX0P = 1.5 * barThickness;
			wGradX1P = wGradX0P;
			wGradY0P = dLen + barThickness - 1.0;
			wGradY1P = wGradY0P - dLenParent;
			tickXP = barTLBR[1][0] - barNumberWidth;
		}
	}
	else
	{
		dLen = barTLBR[1][0] - (2.0 * barThickness);
		rectColour = Wt::WRectF(barThickness, barTLBR[0][1] + ((double)legendTickLines * barNumberHeight) + (0.5 * barThickness), dLen, barThickness);

		wGradX0 = barThickness + 1.0;
		wGradX1 = dLen + barThickness - 1.0;
		wGradY0 = 1.5 * barThickness;
		wGradY1 = wGradY0;
		tickY = barTLBR[0][1] + ((double)legendTickLines * barNumberHeight);

		if (legendBarDouble == 1)
		{
			dTemp = (double)(scaleValues[1][1] - scaleValues[1][0]) / (double)(scaleValues[1][3] - scaleValues[1][0]);
			dLenParent = dTemp * dLen;
			rectColourParent = Wt::WRectF(barThickness, barTLBR[1][1] - (2.0 * barNumberHeight) - (2.0 * barThickness), dLenParent, barThickness);
			rectWhiteParent = Wt::WRectF(barThickness + dLenParent, barTLBR[1][1] - (2.0 * barNumberHeight) - (2.0 * barThickness), dLen - dLenParent, barThickness);

			wGradX0P = barThickness + 1.0;
			wGradX1P = wGradX0P + dLenParent - 1.0;
			wGradY0P = 1.5 * barThickness;
			wGradY1P = wGradY0P;
			tickYP = barTLBR[1][1] - (2.0 * barNumberHeight) - (0.5 * barThickness);
		}
	}

	wGrad.setLinearGradient(wGradX0, wGradY0, wGradX1, wGradY1);
	vector<string> vsVal = jnumber.doubleToCommaString(scaleValues[0], 1);

	double bandWidth = 1.0 / (double)(numColourBands);
	for (int ii = 0; ii < numColour; ii++)  // Child bar only.
	{
		dTemp = (double)ii * bandWidth;
		wColour.setRgb(keyColour[ii][0], keyColour[ii][1], keyColour[ii][2]);
		wGrad.addColorStop(dTemp, wColour);
		if (barVertical)
		{
			tickY = wGradY0 - (dTemp * dLen) + 1.0;
			painter.drawLine(tickX, tickY, tickX + (barThickness / 2.0), tickY);
			temp = vsVal[ii] + suffix;
			wsValue = Wt::WString::fromUTF8(temp);
			if (legendTickLines > 1)
			{
				wsUnit = Wt::WString::fromUTF8(displayUnit);
				widthDiff = abs((int)displayUnit.size() - (int)temp.size());
				widthDiff /= 2;
				if (temp.size() < displayUnit.size())
				{
					for (int jj = 0; jj < widthDiff; jj++)
					{
						wsValue += " ";
					}
				}
				else
				{
					for (int jj = 0; jj < widthDiff; jj++)
					{
						wsUnit += " ";
					}
				}
			}

			if (ii == 0) { yCoord = min(tickY - (0.5 * (double)legendTickLines * barNumberHeight), barTLBR[1][1] - ((double)legendTickLines * barNumberHeight)); }
			else if (ii == numColour - 1) { yCoord = max(tickY - (0.5 * (double)legendTickLines * barNumberHeight), 0.0); }
			else { yCoord = tickY - (0.5 * (double)legendTickLines * barNumberHeight); }
			painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Right, wsValue);
			if (legendTickLines > 1)
			{
				yCoord += barNumberHeight;
				painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Right, wsUnit);
			}
		}
		else
		{
			tickX = wGradX0 + (dTemp * dLen) - 1.0;
			painter.drawLine(tickX, tickY, tickX, tickY + (barThickness / 2.0));
			temp = vsVal[ii] + suffix;
			wsValue = Wt::WString::fromUTF8(temp);
			if (legendTickLines > 1)
			{
				wsUnit = Wt::WString::fromUTF8(displayUnit);
				widthDiff = abs((int)displayUnit.size() - (int)temp.size());
				widthDiff /= 2;
				wsTemp = "";
				for (int jj = 0; jj < widthDiff; jj++)
				{
					wsTemp += " ";
				}
				if (ii == 0)
				{
					if (temp.size() < displayUnit.size()) { wsValue = wsTemp + wsValue; }
					else { wsUnit = wsTemp + wsUnit; }
				}
				else if (ii == numColour - 1)
				{
					if (temp.size() < displayUnit.size()) { wsValue += wsTemp; }
					else { wsUnit += wsTemp; }
				}
			}

			if (ii == 0)
			{
				xCoord = 0.0;
				yCoord = barTLBR[0][1];
				painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Left, wsValue);
				if (legendTickLines > 1)
				{
					yCoord += barNumberHeight;
					painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Left, wsUnit);
				}
			}
			else if (ii == numColour - 1)
			{
				xCoord = barTLBR[1][0] - barNumberWidth;
				yCoord = barTLBR[0][1];
				painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Right, wsValue);
				if (legendTickLines > 1)
				{
					yCoord += barNumberHeight;
					painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Right, wsUnit);
				}
			}
			else
			{
				xCoord = tickX - (barNumberWidth / 2.0);
				yCoord = barTLBR[0][1];
				painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Center, wsValue);
				if (legendTickLines > 1)
				{
					yCoord += barNumberHeight;
					painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Center, wsUnit);
				}
			}
		}
	}
	
	if (legendBarDouble == 1)
	{
		double colourFraction;
		wGradParent.setLinearGradient(wGradX0P, wGradY0P, wGradX1P, wGradY1P);
		for (int ii = 0; ii < keyColour.size(); ii++)
		{
			colourFraction = (double)ii / (double)(keyColour.size() - 1);
			wColour.setRgb(keyColour[ii][0], keyColour[ii][1], keyColour[ii][2]);
			wGradParent.addColorStop(colourFraction, wColour);
		}

		int coord1;  // Representing the problematic dimension of ii = 1.
		vsVal = jnumber.doubleToCommaString(scaleValues[1], decimalPlaces);
		vsVal[2] = areaName[0];
		for (int ii = 0; ii < 4; ii++)  // For parent bar only.
		{
			dTemp = (double)(scaleValues[1][ii] - scaleValues[1][0]) / (double)(scaleValues[1][3] - scaleValues[1][0]);					
			temp = vsVal[ii] + suffix;
			wsValue = Wt::WString::fromUTF8(temp);
			if (legendTickLines > 1)
			{
				wsUnit = Wt::WString::fromUTF8(displayUnit);
				widthDiff = abs((int)displayUnit.size() - (int)temp.size());
				widthDiff /= 2;
				wsTemp = "";
				for (int jj = 0; jj < widthDiff; jj++)
				{
					wsTemp += " ";
				}
			}

			if (barVertical)
			{
				if (legendTickLines > 1)
				{
					if (temp.size() < displayUnit.size())
					{
						wsValue = wsTemp + wsValue;
					}
					else
					{
						wsUnit = wsTemp + wsUnit;
					}
				}

				if (ii == 0)
				{
					tickYP = wGradY0P - (dTemp * dLen) + 1.0;
					painter.drawLine(tickXP, tickYP, tickXP - (1.0 * barThickness), tickYP);
					xCoord = barTLBR[1][0] - barNumberWidth;
					yCoord = min(tickYP - barNumberHeight, barTLBR[1][1] - (2.0 * barNumberHeight));
					painter.drawText(xCoord, yCoord, barNumberWidth, barNumberHeight, Wt::AlignmentFlag::Left, wsValue);
					if (legendTickLines > 1)
					{
						yCoord += barNumberHeight;
						painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Left, wsUnit);
					}
				}
				else if (ii == 1)
				{
					tickYP = wGradY0P - (dTemp * dLen) + 1.0;
					coord1 = tickYP - barNumberHeight;
					coordMax = wGradY0P - (3.0 * barNumberHeight);
					if (coord1 > coordMax) { coord1 = coordMax; }
					coordMin = 4.0 * barNumberHeight;
					if (coord1 < coordMin) { coord1 = coordMin; }

					xCoord = barTLBR[1][0] - barNumberWidth;
					painter.drawText(xCoord, coord1, barNumberWidth, barNumberHeight, Wt::AlignmentFlag::Left, wsValue);
					if (legendTickLines > 1)
					{
						coord1 += barNumberHeight;
						painter.drawText(xCoord, coord1, barNumberWidth, barThickness, Wt::AlignmentFlag::Left, wsUnit);
					}

					if (coord1 == tickYP - barNumberHeight)
					{
						painter.drawLine(tickXP, tickYP, tickXP - (1.0 * barThickness), tickYP);
					}
					else
					{
						painter.drawLine(tickXP, coord1 + (0.5 * barNumberHeight), tickXP - (0.5 * barThickness), coord1 + (0.5 * barNumberHeight));
						painter.drawLine(tickXP - (0.5 * barThickness), coord1 + (0.5 * barNumberHeight), tickXP - (0.5 * barThickness), tickYP);
						painter.drawLine(tickXP - (0.5 * barThickness), tickYP, tickXP - (1.0 * barThickness), tickYP);
					}
				}
				else if (ii == 2)
				{
					tickYP = wGradY0P - (dTemp * dLen) + 1.0;
					yCoord = tickYP - barNumberHeight;
					coordMax = coord1 - (2.0 * barNumberHeight);
					if (yCoord > coordMax) { yCoord = coordMax; }
					coordMin = 2.0 * barNumberHeight;
					if (yCoord < coordMin) { yCoord = coordMin; }

					xCoord = barTLBR[1][0] - barNumberWidth;
					painter.drawText(xCoord, yCoord, barNumberWidth, barNumberHeight, Wt::AlignmentFlag::Left, wsValue);
					if (legendTickLines > 1)
					{
						yCoord += barNumberHeight;
						painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Left, wsUnit);
					}

					if (yCoord == tickYP)
					{
						painter.drawLine(tickXP, tickYP, tickXP - (1.0 * barThickness), tickYP);
					}
					else
					{
						painter.drawLine(tickXP, yCoord, tickXP - (0.5 * barThickness), yCoord);
						painter.drawLine(tickXP - (0.5 * barThickness), yCoord, tickXP - (0.5 * barThickness), tickYP);
						painter.drawLine(tickXP - (0.5 * barThickness), tickYP, tickXP - (1.0 * barThickness), tickYP);
					}

					double dRadius = (barThickness - 6.0) / 2.0;
					rectDot = Wt::WRectF(tickXP - (1.5 * barThickness) - dRadius, tickYP - dRadius, 2.0 * dRadius, 2.0 * dRadius);
				}
				else if (ii == 3)
				{
					tickYP = wGradY0P - (dTemp * dLen) + 1.0;
					painter.drawLine(tickXP, tickYP, tickXP - (1.0 * barThickness), tickYP);
					xCoord = barTLBR[1][0] - barNumberWidth;
					yCoord = max(tickY - barNumberHeight, 0.0);
					painter.drawText(xCoord, yCoord, barNumberWidth, barNumberHeight, Wt::AlignmentFlag::Left, wsValue);
					if (legendTickLines > 1)
					{
						yCoord += barNumberHeight;
						painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Left, wsUnit);
					}
				}
			}
			else
			{
				if (ii == 0)
				{
					if (legendTickLines > 1)
					{
						if (temp.size() < displayUnit.size())
						{
							wsValue = wsTemp + wsValue;
						}
						else
						{
							wsUnit = wsTemp + wsUnit;
						}
					}
					xCoord = 0.0;
					yCoord = barTLBR[1][1] - (2.0 * barNumberHeight);
					painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Left, wsValue);
					if (legendTickLines > 1)
					{
						yCoord += barNumberHeight;
						painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Left, wsUnit);
					}

					tickXP = barThickness + (dTemp * dLen);
					painter.drawLine(tickXP, tickYP, tickXP, tickYP - (0.75 * barThickness));
				}
				else if (ii == 1)
				{
					tickXP = barThickness + (dTemp * dLen);
					coord1 = tickXP - (barNumberWidth / 2.0);
					coordMin = barNumberWidth;  // To avoid overlap with first or last entry.
					if (coord1 < coordMin) { coord1 = coordMin; }
					coordMax = barTLBR[1][0] - (3.0 * barNumberWidth);
					if (coord1 > coordMax) { coord1 = coordMax; }

					yCoord = barTLBR[1][1] - (2.0 * barNumberHeight);
					painter.drawText(coord1, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Center, wsValue);
					if (legendTickLines > 1)
					{
						yCoord += barNumberHeight;
						painter.drawText(coord1, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Center, wsUnit);
					}

					if (coord1 == tickXP - (barNumberWidth / 2.0))
					{
						painter.drawLine(tickXP, tickYP, tickXP, tickYP - (0.75 * barThickness));
					}
					else
					{
						painter.drawLine(tickXP, tickYP, tickXP, tickYP - (0.5 * barThickness));
						painter.drawLine(tickXP, tickYP, coord1 + (0.5 * barNumberWidth), tickYP);
						painter.drawLine(coord1 + (0.5 * barNumberWidth), tickYP, coord1 + (0.5 * barNumberWidth), tickYP + (0.4 * barThickness));
					}
				}
				else if (ii == 2)
				{
					tickXP = barThickness + (dTemp * dLen);
					xCoord = tickXP - (barNumberWidth / 2.0);
					coordMin = coord1 + barNumberWidth;  // To avoid overlap with other entries.
					if (xCoord < coordMin) { xCoord = coordMin; }
					coordMax = barTLBR[1][0] - (2.0 * barNumberWidth);
					if (xCoord > coordMax) { xCoord = coordMax; }

					yCoord = barTLBR[1][1] - (2.0 * barNumberHeight);
					painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Center, wsValue);
					if (legendTickLines > 1)
					{
						yCoord += barNumberHeight;
						painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Center, wsUnit);
					}

					if (xCoord == tickXP - (barNumberWidth / 2.0))
					{
						painter.drawLine(tickXP, tickYP, tickXP, tickYP - (0.75 * barThickness));
					}
					else
					{
						painter.drawLine(tickXP, tickYP, tickXP, tickYP - (0.5 * barThickness));
						painter.drawLine(tickXP, tickYP, xCoord + (0.5 * barNumberWidth), tickYP);
						painter.drawLine(xCoord + (0.5 * barNumberWidth), tickYP, xCoord + (0.5 * barNumberWidth), tickYP + (0.4 * barThickness));
					}

					double dRadius = (barThickness - 6.0) / 2.0;
					rectDot = Wt::WRectF(tickXP - dRadius, tickYP - (1.0 * barThickness) - dRadius, 2.0 * dRadius, 2.0 * dRadius);
				}
				else if (ii == 3)
				{
					if (legendTickLines > 1)
					{
						if (temp.size() < displayUnit.size())
						{
							wsValue += wsTemp;
						}
						else
						{
							wsUnit += wsTemp;
						}
					}
					xCoord = barTLBR[1][0] - barNumberWidth;
					yCoord = barTLBR[1][1] - (2.0 * barNumberHeight);
					painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Right, wsValue);
					if (legendTickLines > 1)
					{
						yCoord += barNumberHeight;
						painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Right, wsUnit);
					}

					tickXP = barThickness + (dTemp * dLen);
					painter.drawLine(tickXP, tickYP, tickXP, tickYP - (0.75 * barThickness));
				}
			}
		}
	}
	else if (legendBarDouble == 2)
	{
		double parentValue = jsb.getDatasetValue(-1, 0);
		dTemp = (parentValue - (double)scaleValues[0][0]) / (double)(scaleValues[0][scaleValues[0].size() - 1] - scaleValues[0][0]);	
		if (parentValue < 0.0) { gameOn = 0; }
		vsList = { areaName[0] };

		if (parentValue > 101.0)
		{
			int iParentValue = int(round(parentValue));
			temp = jnumber.intToCommaString(iParentValue) + suffix;
		}
		else { temp = jnumber.doubleToCommaString(parentValue, 1) + suffix; }
		vsList.push_back(temp);

		if (legendTickLines != 1)
		{
			vsList.push_back(displayUnit);
		}

		if (barVertical && gameOn)
		{
			vsListCentered = jstr.horizontalCentering(vsList);

			tickXP = barTLBR[0][0] + barNumberWidth + (1.5 * barThickness);
			tickYP = wGradY0 - (dTemp * dLen) + 1.0;
			painter.drawLine(tickXP, tickYP, tickXP + (0.5 * barThickness), tickYP);

			xCoord = tickXP + (0.5 * barThickness) + 2.0;
			yCoord = tickYP - (0.5 * (double)legendTickLines * barNumberHeight);
			for (int ii = 0; ii < vsListCentered.size(); ii++)
			{
				wsTemp = Wt::WString::fromUTF8(vsListCentered[ii]);
				painter.drawText(xCoord, yCoord, barNumberWidth, barNumberHeight, Wt::AlignmentFlag::Left, wsTemp);
				yCoord += barNumberHeight;
			}

			double dRadius = (barThickness - 6.0) / 2.0;
			rectDot = Wt::WRectF(tickXP - (0.5 * barThickness) - dRadius, tickYP - dRadius, 2.0 * dRadius, 2.0 * dRadius);
		}
		else if (gameOn)
		{
			tickXP = wGradX0 + (dTemp * dLen);
			tickYP = barTLBR[0][1] + ((double)legendTickLines * barNumberHeight) + (1.5 * barThickness);
			painter.drawLine(tickXP, tickYP, tickXP, tickYP + (0.5 * barThickness));

			xCoord = tickXP - (0.5 * barNumberWidth);
			yCoord = tickYP + (0.5 * barThickness) + 1.0;

			coordMin = 0.0;  // To avoid the widget's edges.
			if (xCoord < coordMin) { xCoord = coordMin; }
			coordMax = barTLBR[1][0] - barNumberWidth;
			if (xCoord > coordMax) { xCoord = coordMax; }
			for (int ii = 0; ii < vsList.size(); ii++)
			{
				wsTemp = Wt::WString::fromUTF8(vsList[ii]);
				painter.drawText(xCoord, yCoord, barNumberWidth, barNumberHeight, Wt::AlignmentFlag::Center, wsTemp);
				yCoord += barNumberHeight;
			}

			double dRadius = (barThickness - 6.0) / 2.0;
			rectDot = Wt::WRectF(tickXP - dRadius, tickYP - (0.5 * barThickness) - dRadius, 2.0 * dRadius, 2.0 * dRadius);
		}
	}

	painter.save();
	Wt::WBrush wBrush(wGrad);
	painter.setBrush(wBrush);
	painter.drawRect(rectColour);
	painter.restore();

	if (legendBarDouble == 1)
	{
		Wt::WBrush wBrushParent(wGradParent);
		Wt::WBrush wBrushWhite(Wt::StandardColor::White);
		Wt::WBrush wBrushBlack(Wt::StandardColor::Black);
		painter.setBrush(wBrushParent);
		painter.drawRect(rectColourParent);
		painter.restore();
		painter.setBrush(wBrushWhite);
		painter.drawRect(rectWhiteParent);
		painter.restore();
		painter.setBrush(wBrushBlack);
		painter.drawEllipse(rectDot);
		painter.restore();
	}
	else if (legendBarDouble == 2)
	{
		if (!gameOn) { return; }
		Wt::WBrush wBrushBlack(Wt::StandardColor::Black);
		painter.setBrush(wBrushBlack);
		painter.drawEllipse(rectDot);
		painter.restore();
	}
}
void WTPAINT::paintRegionAll(Wt::WPainter& painter)
{
	Wt::WColor wColour;
	Wt::WBrush wBrush;
	Wt::WPen wPen;
	for (int ii = 0; ii < border.size(); ii++)
	{
		painter.save();
		if (areaName[ii] == "Canada")  // Special case wherein parent is completely obscured.
		{
			wColour = Wt::WColor(0, 0, 0);
			wBrush = Wt::WBrush(Wt::BrushStyle::Solid);
			wBrush.setColor(wColour);
			painter.setBrush(wBrush);
			Wt::WPainterPath wpPath = Wt::WPainterPath(border[ii][0]);
			for (int jj = 1; jj < border[ii].size(); jj++)
			{
				wpPath.lineTo(border[ii][jj]);
			}
			wpPath.closeSubPath();
			painter.drawPath(wpPath);
			painter.restore();			
			continue;
		}
		if (ii == selIndex)  // This non-parent region was selected by the user. 
		{
			wPen = painter.pen();
			if (areaName[0] == "Canada")
			{
				Wt::WLength penWidth(3.0, Wt::LengthUnit::Pixel);
				wPen.setWidth(penWidth);
				wColour = Wt::WColor((areaColour[ii][0] + 128) % 256, (areaColour[ii][1] + 128) % 256, (areaColour[ii][2] + 128) % 256);
				wPen.setColor(wColour);
			}
			wPen.setStyle(Wt::PenStyle::DotLine);
			painter.setPen(wPen);
		}

		wColour = Wt::WColor(areaColour[ii][0], areaColour[ii][1], areaColour[ii][2]);
		wBrush = Wt::WBrush(Wt::BrushStyle::Solid);
		wBrush.setColor(wColour);
		painter.setBrush(wBrush);
		Wt::WPainterPath wpPath = Wt::WPainterPath(border[ii][0]);
		for (int jj = 1; jj < border[ii].size(); jj++)
		{
			wpPath.lineTo(border[ii][jj]);
		}
		wpPath.closeSubPath();
		painter.drawPath(wpPath);
		painter.restore();
	}
}
void WTPAINT::makeAreas()
{
	Wt::WString wTemp;
	string suffix, value;
	double dAreaValue;
	int iAreaValue;
	clearAreas();
	string sUnit = jsb.getUnit(-1);
	int index = jsb.getActiveIndex();
	vector<unique_ptr<Wt::WPolygonArea>> vArea(border.size());
	for (int ii = 0; ii < border.size(); ii++)
	{
		value = areaName[ii] + "\n";
		dAreaValue = jsb.vDataset[index][ii];
		if (dAreaValue > 101.0)
		{
			iAreaValue = int(round(dAreaValue));
			value += jnumber.intToCommaString(iAreaValue);
		}
		else { value += jnumber.doubleToCommaString(dAreaValue, 1); }
		if (legendTickLines == 1) { suffix = " (" + sUnit + ")"; }
		else
		{
			if (sUnit == "% of population") { 
				suffix = " %\n(of population)";
			}
			else { 
				suffix = "\n(" + sUnit + ")";
			}
		}
		wTemp = Wt::WString::fromUTF8(value + suffix);

		vArea[ii] = make_unique<Wt::WPolygonArea>(border[ii]);
		vArea[ii]->setToolTip(wTemp);
		area.push_back(vArea[ii].get());
	}
	for (int ii = border.size() - 1; ii >= 0; ii--)
	{
		this->addArea(move(vArea[ii]));  // Parent must go last, as the lower index area receives priority when areas overlap.
	}
	vector<Wt::WPointF> corners(4);
	corners[0] = Wt::WPointF(0.0, 0.0);
	corners[1] = Wt::WPointF(dWidth, 0.0);
	corners[2] = Wt::WPointF(dWidth, dHeight);
	corners[3] = Wt::WPointF(0.0, dHeight);
	auto bgArea = make_unique<Wt::WPolygonArea>(corners);
	wTemp = mapTooltip.at("grandparent");
	bgArea->setToolTip(wTemp);
	area.push_back(bgArea.get());
	this->addArea(move(bgArea));
}
void WTPAINT::scaleChildToWidget(vector<vector<double>>& vvdBorder, vector<vector<double>>& vvdFrame)
{
	if (widgetPPKM <= 0.0) { err("Missing widgetPPKM-wtpaint.scaleChildToWidget"); }
	vvdFrame[0][0] *= widgetPPKM;
	vvdFrame[0][1] *= widgetPPKM;
	vvdFrame[1][0] *= widgetPPKM;
	vvdFrame[1][1] *= widgetPPKM;
	for (int ii = 0; ii < vvdBorder.size(); ii++)
	{
		vvdBorder[ii][0] *= widgetPPKM;
		vvdBorder[ii][1] *= widgetPPKM;
	}
}
void WTPAINT::scaleImgBar()
{
	// Creates new values for barTLBR and imgTLBR.
	if (dHeight < 0.0 || dWidth < 0.0) { err("No widget dimensions-wtpaint.scaleImgBar"); }
	if (legendBarDouble < 0 || legendTickLines < 0) { err("Missing legend init-scaleImgBar"); }
	if (parentAspectRatio < 0.0) { err("Missing parentAspectRatio-scaleImgBar"); }

	barTLBR.clear();
	barTLBR.resize(2, vector<double>(2));
	imgTLBR.clear();
	imgTLBR.resize(2, vector<double>(2));
	imgTLBR[0] = { 0.0, 0.0 };

	// Note that these pixel spaces assume the worst case scenario (double legend bars).
	double barSpaceVertical = (2.0 * (double)legendTickLines * barNumberHeight) + (4.0 * barThickness);
	double barSpaceHorizontal = (2.0 * barNumberWidth) + (4.0 * barThickness);
	
	double spaceARifVertical = (dWidth - barSpaceHorizontal) / dHeight;
	double spaceARifHorizontal = dWidth / (dHeight - barSpaceVertical);
	if (abs(spaceARifVertical - parentAspectRatio) < abs(spaceARifHorizontal - parentAspectRatio))
	{
		// The legend bar will be vertical.
		if (spaceARifVertical < parentAspectRatio)
		{
			// The limiting factor will be the parent image's width.
			imgTLBR[1][0] = dWidth - barSpaceHorizontal - 1.0;
			imgTLBR[1][1] = imgTLBR[1][0] / parentAspectRatio;
			barTLBR[0][0] = dWidth - barSpaceHorizontal;
			barTLBR[0][1] = 0.0;
			barTLBR[1][0] = dWidth;
			barTLBR[1][1] = imgTLBR[1][1];
		}
		else
		{
			// The limiting factor will be the parent image's height.
			imgTLBR[1][1] = dHeight;
			imgTLBR[1][0] = dHeight * parentAspectRatio;
			barTLBR[0][0] = imgTLBR[1][0] + 1.0;
			barTLBR[0][1] = 0.0;
			barTLBR[1][0] = barTLBR[0][0] + barSpaceHorizontal;
			barTLBR[1][1] = dHeight;
		}
	}
	else
	{
		// The legend bar will be horizontal.
		if (spaceARifHorizontal < parentAspectRatio)
		{
			// The limiting factor will be the parent image's width.
			imgTLBR[1][0] = dWidth;
			imgTLBR[1][1] = dWidth / parentAspectRatio;
			barTLBR[0][0] = 0.0;
			barTLBR[0][1] = imgTLBR[1][1] + 1.0;
			barTLBR[1][0] = dWidth;
			barTLBR[1][1] = barTLBR[0][1] + barSpaceVertical;
		}
		else
		{
			// The limiting factor will be the parent image's height.
			imgTLBR[1][1] = dHeight - barSpaceVertical - 1.0;
			imgTLBR[1][0] = imgTLBR[1][1] * parentAspectRatio;
			barTLBR[0][0] = 0.0;
			barTLBR[0][1] = dHeight - barSpaceVertical;
			barTLBR[1][0] = imgTLBR[1][0];
			barTLBR[1][1] = dHeight;
		}
	}
}
void WTPAINT::scaleImgBar(vector<vector<double>>& parentFrameKM)
{
	// This variant is meant for a new widget that is drawing a map for the first time.
	double imgWidthKM = parentFrameKM[1][0] - parentFrameKM[0][0];
	double imgHeightKM = parentFrameKM[1][1] - parentFrameKM[0][1];
	parentAspectRatio = imgWidthKM / imgHeightKM;
	scaleImgBar();
}
vector<Wt::WPointF> WTPAINT::scaleParentToWidget(vector<vector<vector<double>>>& vvvdBorder, vector<vector<double>>& parentFrameKM)
{
	// Returns the parent region's border (in pixels), scaled to the widget.
	if (imgTLBR.size() < 2) { err("imgTLBR not initialized-wtpaint.scaleParentToWidget"); }

	double xRatio = (parentFrameKM[1][0] - parentFrameKM[0][0]) / imgTLBR[1][0];
	double yRatio = (parentFrameKM[1][1] - parentFrameKM[0][1]) / imgTLBR[1][1];
	double ratio = max(xRatio, yRatio);
	widgetPPKM = 1.0 / ratio;
	vector<Wt::WPointF> parentBorder(vvvdBorder[0].size());
	for (int ii = 0; ii < vvvdBorder[0].size(); ii++)
	{
		parentBorder[ii].setX(vvvdBorder[0][ii][0] / ratio);
		parentBorder[ii].setY(vvvdBorder[0][ii][1] / ratio);
	}
	return parentBorder;
}
void WTPAINT::setDimensions(int iHeight, int iWidth)
{
	dWidth = (double)iWidth;
	dHeight = (double)iHeight;
	resize(dWidth, dHeight);
}
void WTPAINT::setWColour(Wt::WColor& wColour, vector<int> rgb, double percent)
{
	double dTemp = 255.0 * percent;
	wColour.setRgb(rgb[0], rgb[1], rgb[2], int(dTemp));
}
void WTPAINT::updateAreaColour()
{
	if (keyColour.size() != numColourBands + 1) { initColour(); }
	if (legendMin < 0.0 || legendMax < 0.0) { 
		pair<int, int> minMax;
		jnumber.minMaxIndex(minMax, jsb.vDataset[jsb.activeIndex]);
		legendMin = jsb.vDataset[jsb.activeIndex][get<0>(minMax)];
		legendMax = jsb.vDataset[jsb.activeIndex][get<1>(minMax)];
	}
	size_t numArea = areaName.size();
	areaColour.resize(numArea, vector<int>(3));

	double percentage, dR, dG, dB, remains, dMin, dMax, dVal;
	int floor, indexBegin;

	if (legendBarDouble == 1)
	{
		areaColour[0] = { 255, 255, 255 };  // Parent.
		indexBegin = 1;
	}
	else { indexBegin = 0; }
	
	Wt::WString wTemp;
	if (legendMin == legendMax)  // Every data point is identical ! 
	{
		dVal = jsb.getDatasetValue(-1, 0);
		if (dVal == -1.0)  // Data is missing, so draw as grey.
		{
			for (int ii = indexBegin; ii < numArea; ii++)
			{
				areaColour[ii] = unknownColour;
				wTemp = mapTooltip.at("grey");
				area[ii]->setToolTip(wTemp);
			}
		}
		else  // There is data, but it's all the same...
		{
			for (int ii = indexBegin; ii < numArea; ii++)
			{
				areaColour[ii] = extraColour;
				wTemp = mapTooltip.at("pink");
				area[ii]->setToolTip(wTemp);
			}
		}
		return;
	}
	for (int ii = indexBegin; ii < numArea; ii++)
	{
		dVal = jsb.getDatasetValue(-1, ii);
		if (dVal == -1.0)  // Data is missing, so draw as grey.
		{
			areaColour[ii] = unknownColour;
			wTemp = mapTooltip.at("grey");
			area[ii]->setToolTip(wTemp);
			continue;
		}
		percentage = (dVal - legendMin) / (legendMax - legendMin);
		if (percentage > 0.9999) { percentage = 0.9999; }
		percentage *= (double)numColourBands;
		floor = int(percentage);
		remains = percentage - (double)floor;
		dR = (keyColour[floor + 1][0] - keyColour[floor][0]) * remains;
		dG = (keyColour[floor + 1][1] - keyColour[floor][1]) * remains;
		dB = (keyColour[floor + 1][2] - keyColour[floor][2]) * remains;
		areaColour[ii][0] = keyColour[floor][0] + (int)dR;
		areaColour[ii][1] = keyColour[floor][1] + (int)dG;
		areaColour[ii][2] = keyColour[floor][2] + (int)dB;
	}
}
void WTPAINT::updateDisplay(int index)
{
	if (index >= jsb.vDataset.size() || index < 0) { err("Invalid index-wtpaint.updateDisplay"); }
	jsb.activeIndex = index;
	string unit = jsb.vUnit[index];

	scaleImgBar();
	updateAreaColour();
	update();
}
