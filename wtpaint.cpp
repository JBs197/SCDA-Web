#include "wtpaint.h"

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
	parentFrameKM.assign(vvvdBorder[0].begin() + vvvdBorder[0].size() - 2, vvvdBorder[0].end());
	if (parentFrameKM.size() != 2) { jf.err("Missing frameKM-wtpaint.drawMap"); }
	vvvdBorder[0].resize(vvvdBorder[0].size() - 2);
	for (int ii = 0; ii < vvvdBorder[0].size(); ii++)
	{
		vvvdBorder[0][ii][0] -= parentFrameKM[0][0];
		vvvdBorder[0][ii][1] -= parentFrameKM[0][1];
	}
}
vector<Wt::WPolygonArea*> WTPAINT::drawMap(vector<vector<vector<double>>>& vvvdBorder, vector<string>& vsRegion, vector<double>& vdData)
{
	if (vvvdBorder.size() < 2 || vsRegion.size() < 2 || vdData.size() < 2) { jf.err("Less than two regions given-wtpaint.drawMap"); }
	vector<vector<double>> parentFrameKM, childFrameKM;
	displaceParentToWidget(vvvdBorder, parentFrameKM);
	vector<Wt::WPointF> borderParent = scaleParentToWidget(vvvdBorder, parentFrameKM);
	border.resize(vsRegion.size());  // Form [children, parent].
	area.resize(vsRegion.size());  // Form [children, parent].
	areaName.resize(vsRegion.size());  // Form [children, parent].
	areaData.resize(vsRegion.size());  // Form [children, parent].

	vector<double> dispParentTL;  // Child's TL widget pixel displacement from (0,0). 
	for (int ii = 1; ii < vsRegion.size(); ii++)
	{
		areaName[ii - 1] = vsRegion[ii];
		areaData[ii - 1] = vdData[ii];
		dispParentTL = getChildTL(vvvdBorder[ii], childFrameKM, parentFrameKM);
		scaleChildToWidget(vvvdBorder[ii], childFrameKM);
		border[ii - 1] = displaceChildToParent(vvvdBorder[ii], childFrameKM[0], dispParentTL);		
	}
	areaName[vsRegion.size() - 1] = vsRegion[0];
	areaData[vsRegion.size() - 1] = vdData[0];
	border[vsRegion.size() - 1] = borderParent;

	makeAreas();
	updateAreaColour();
	update();
	return area;
}
vector<double> WTPAINT::getChildTL(vector<vector<double>>& vvdBorder, vector<vector<double>>& childFrameKM, vector<vector<double>>& parentFrameKM)
{
	if (widgetPPKM <= 0.0) { jf.err("No widgetPPKM-wtpaint.getChildTL"); }
	childFrameKM.assign(vvdBorder.begin() + vvdBorder.size() - 2, vvdBorder.end());
	if (childFrameKM.size() != 2) { jf.err("Missing frameKM-wtpaint.getChildTL"); }
	vvdBorder.resize(vvdBorder.size() - 2);
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
	jf.toInt(TLBRd, TLBR);
	return TLBR;
}
vector<int> WTPAINT::getScaleValues(int numTicks)
{
	vector<int> ticks(numTicks);
	vector<int> indexMinMax = jf.minMax(areaData);
	double min = areaData[indexMinMax[0]], dTemp;
	if (min < 0.0) { min = 0.0; }
	double max = areaData[indexMinMax[1]];
	if (max < 0.0) { max = 0.0; }
	double bandWidth = (max - min) / (double)(numTicks - 1);
	for (int ii = 0; ii < numTicks; ii++)
	{
		dTemp = (double)ii * bandWidth;
		ticks[ii] = int(round(min + dTemp));
	}
	return ticks;
}
void WTPAINT::makeAreas()
{
	clearAreas();
	for (int ii = 0; ii < border.size(); ii++)
	{
		auto wpArea = make_unique<Wt::WPolygonArea>(border[ii]);
		area.push_back(wpArea.get());
		this->addArea(move(wpArea));
	}
	vector<Wt::WPointF> corners(4);
	corners[0] = Wt::WPointF(0.0, 0.0);
	corners[1] = Wt::WPointF(dWidth, 0.0);
	corners[2] = Wt::WPointF(dWidth, dHeight);
	corners[3] = Wt::WPointF(0.0, dHeight);
	auto bgArea = make_unique<Wt::WPolygonArea>(corners);
	area.push_back(bgArea.get());
	this->addArea(move(bgArea));
}
void WTPAINT::initColour()
{
	keyColour.resize(6);
	keyColour[0] = { 255, 0, 0 };  // Red
	keyColour[1] = { 255, 255, 0 };  // Yellow
	keyColour[2] = { 0, 255, 0 };  // Green
	keyColour[3] = { 0, 255, 255 };  // Teal
	keyColour[4] = { 0, 0, 255 };  // Blue
	keyColour[5] = { 127, 0, 255 };  // Violet
	extraColour = { 255, 0, 127 };  // Pink
}
void WTPAINT::paintEvent(Wt::WPaintDevice* paintDevice)
{
	Wt::WPainter painter(paintDevice);
	Wt::WLength penWidth(2.0, Wt::LengthUnit::Pixel);
	Wt::WPen pen(Wt::PenStyle::SolidLine);
	pen.setWidth(penWidth);
	painter.setPen(pen);
	if (area.size() < 1) { return; }
	paintRegionAll(painter);
	paintLegendBar(painter);
	resize(barTLBR[1][0] + 1.0, barTLBR[1][1] + 1.0);
}
void WTPAINT::paintLegendBar(Wt::WPainter& painter)
{
	int numColour = numColourBands + 1;
	if (keyColour.size() != numColour) { initColour(); }
	double width = barTLBR[1][0] - barTLBR[0][0];
	if (width <= 0.0) { jf.err("Invalid barTLBR width-wtpaint.paintLegendBar"); }
	double height = barTLBR[1][1] - barTLBR[0][1];
	if (height <= 0.0) { jf.err("Invalid barTLBR height-wtpaint.paintLegendBar"); }
	bool barVertical = 0;
	if (height > width) { barVertical = 1; }

	double dLen, wGradX0, wGradX1, wGradY0, wGradY1, tickX, tickY, dTemp, xCoord, yCoord;
	Wt::WRectF rectColour; 
	Wt::WGradient wGrad;
	Wt::WColor wColour;
	Wt::WString wsTemp;
	string temp;
	vector<int> scaleValues = getScaleValues(numColour);
	if (barVertical)
	{
		dLen = dHeight - (2.0 * barThickness);
		rectColour = Wt::WRectF(barTLBR[1][0] - (1.5 * barThickness), barThickness, barThickness, dLen);
		wGradX0 = 1.5 * barThickness;
		wGradX1 = wGradX0;
		wGradY0 = dLen + barThickness - 1.0;
		wGradY1 = barThickness + 1.0;
		tickX = barTLBR[1][0] - (1.5 * barThickness);
		xCoord = barTLBR[0][0] - 2.0;
	}
	else
	{
		dLen = dWidth - (2.0 * barThickness);
		rectColour = Wt::WRectF(barThickness, barTLBR[1][1] - (1.5 * barThickness), dLen, barThickness);
		wGradX0 = barThickness + 1.0; 
		wGradX1 = dLen + barThickness - 1.0; 
		wGradY0 = 1.5 * barThickness;
		wGradY1 = wGradY0;
		tickY = barTLBR[1][1] - (1.5 * barThickness);
		yCoord = barTLBR[0][1];
	}
	wGrad.setLinearGradient(wGradX0, wGradY0, wGradX1, wGradY1);
	double bandWidth = 1.0 / (double)(numColourBands);
	for (int ii = 0; ii < numColour; ii++)
	{
		dTemp = (double)ii * bandWidth;
		wColour.setRgb(keyColour[ii][0], keyColour[ii][1], keyColour[ii][2]);
		wGrad.addColorStop(dTemp, wColour);
		temp = to_string(scaleValues[ii]);
		wsTemp = Wt::WString::fromUTF8(temp);
		if (barVertical) 
		{ 
			tickY = wGradY0 - (dTemp * dLen) + 1.0; 
			painter.drawLine(tickX, tickY, tickX - (barThickness / 2.0), tickY);
			if (ii == 0) { yCoord = min(tickY - (barNumberHeight / 2.0), barTLBR[1][1] - barNumberHeight); }
			else if (ii == numColour - 1) { yCoord = max(tickY - (barNumberHeight / 2.0), 0.0); }
			else { yCoord = tickY - (barNumberHeight / 2.0); }
			painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Right, wsTemp);
		}
		else 
		{ 
			tickX = wGradX0 + (dTemp * dLen) - 1.0; 
			painter.drawLine(tickX, tickY, tickX, tickY - (barThickness / 2.0));
			if (ii == 0) 
			{ 
				xCoord = 0.0; 
				painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Left, wsTemp);
			}
			else if (ii == numColour - 1) 
			{ 
				xCoord = barTLBR[1][0] - barNumberWidth; 
				painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Right, wsTemp);
			}
			else 
			{ 
				xCoord = tickX - (barNumberWidth / 2.0);
				painter.drawText(xCoord, yCoord, barNumberWidth, barThickness, Wt::AlignmentFlag::Center, wsTemp);
			}
		}		
	}
	Wt::WBrush wBrush(wGrad);
	painter.setBrush(wBrush);
	painter.drawRect(rectColour);
}
void WTPAINT::paintRegionAll(Wt::WPainter& painter)
{
	Wt::WColor wColour;
	Wt::WBrush wBrush;
	painter.save();
	for (int ii = border.size() - 1; ii >= 0; ii--)
	{
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
void WTPAINT::scaleChildToWidget(vector<vector<double>>& vvdBorder, vector<vector<double>>& vvdFrame)
{
	if (widgetPPKM <= 0.0) { jf.err("Missing widgetPPKM-wtpaint.scaleChildToWidget"); }
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
vector<Wt::WPointF> WTPAINT::scaleParentToWidget(vector<vector<vector<double>>>& vvvdBorder, vector<vector<double>>& parentFrameKM)
{
	// Returns the parent region's border (in pixels), scaled to the widget.
	if (dHeight < 0.0 || dWidth < 0.0) { jf.err("No widget dimensions-wtpaint.scaleParentToWidget"); }
	vector<Wt::WPointF> parentBorder(vvvdBorder[0].size());
	barTLBR.clear();
	barTLBR.resize(2, vector<double>(2));
	imgTLBR.clear();
	imgTLBR.resize(2, vector<double>(2));
	imgTLBR[0] = { 0.0, 0.0 };
	double barSpaceHorizontal = barNumberWidth + (2.0 * barThickness);
	double barSpaceVertical = barNumberHeight + (2.0 * barThickness);
	double imgWidthKM = parentFrameKM[1][0] - parentFrameKM[0][0];
	double imgHeightKM = parentFrameKM[1][1] - parentFrameKM[0][1];
	double xRatio = imgWidthKM / (dWidth - barSpaceHorizontal);  // km per pixel
	double yRatio = imgHeightKM / (dHeight - barSpaceVertical);
	double ratio; 
	if (xRatio > yRatio)  // Legend bar will be horizontal.
	{
		xRatio = imgWidthKM / dWidth;
		ratio = max(xRatio, yRatio);
		imgTLBR[1][0] = imgWidthKM / ratio; 
		imgTLBR[1][1] = imgHeightKM / ratio;
		barTLBR[0][0] = 0.0;
		barTLBR[0][1] = min(dHeight - barSpaceVertical, imgTLBR[1][1] + 10.0);
		barTLBR[1][0] = dWidth;
		barTLBR[1][1] = min(dHeight, imgTLBR[1][1] + 10.0 + barSpaceVertical);
	}
	else  // Legend bar will be vertical.
	{
		yRatio = imgHeightKM / dHeight;
		ratio = max(xRatio, yRatio);
		imgTLBR[1][0] = imgWidthKM / ratio;
		imgTLBR[1][1] = imgHeightKM / ratio;
		barTLBR[0][0] = min(dWidth - barSpaceHorizontal, imgTLBR[1][0] + 10.0);
		barTLBR[0][1] = 0.0;
		barTLBR[1][0] = min(dWidth, imgTLBR[1][0] + 10.0 + barSpaceHorizontal);
		barTLBR[1][1] = dHeight;		
	}	
	widgetPPKM = 1.0 / ratio;
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
bool WTPAINT::testExcludeParent()
{
	areaDataChildren.assign(areaData.begin(), areaData.end() - 1);
	if (areaName.back() == "Canada") { return 1; }
	vector<int> minMaxIndex = jf.minMax(areaDataChildren);
	double percent = areaDataChildren[minMaxIndex[1]] / areaData[0];
	if (percent < defaultParentExclusionThreshold) { return 1; }
	return 0;
}
void WTPAINT::updateAreaColour()
{
	size_t numArea = areaName.size();
	if (numArea != areaData.size()) { jf.err("Area size mismatch-wtf.updateAreaColour"); }
	if (keyColour.size() != numColourBands + 1) { initColour(); }
	areaColour.resize(numArea, vector<int>(3));
	double percentage, dR, dG, dB, remains, dMin, dMax;
	int floor, indexEnd;
	vector<int> indexMinMax;
	if (testExcludeParent())
	{
		indexMinMax = jf.minMax(areaDataChildren);
		dMin = areaDataChildren[indexMinMax[0]];
		dMax = areaDataChildren[indexMinMax[1]];
		areaColour[areaColour.size() - 1] = { 0, 0, 0 };  // Parent.
		indexEnd = numArea - 1;
	}
	else
	{
		indexMinMax = jf.minMax(areaData);
		dMin = areaData[indexMinMax[0]];
		dMax = areaData[indexMinMax[1]];
		indexEnd = numArea;
	}
	
	if (dMin == dMax)  // Every data point is identical ! 
	{
		for (int ii = 0; ii < indexEnd; ii++)
		{
			areaColour[ii] = extraColour;
		}
		return;
	}
	for (int ii = 0; ii < indexEnd; ii++)
	{
		if (areaData[ii] == -1.0)  // Data is missing, so draw as grey.
		{
			areaColour[ii][0] = 180;
			areaColour[ii][1] = 180;
			areaColour[ii][2] = 180;
			continue;
		}
		percentage = (areaData[ii] - dMin) / (dMax - dMin);
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
