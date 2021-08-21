#include "wtpaint.h"

void WTPAINT::areaClicked(int index)
{
	indexAreaPrevious = indexAreaSel;
	indexAreaSel = index;
	nameAreaSel = areaName[index];	
	update(Wt::PaintFlag::Update);
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
void WTPAINT::drawMap(vector<vector<vector<double>>>& vvvdBorder, vector<string>& vsRegion, vector<double>& vdData)
{
	indexAreaSel = -1;
	indexAreaPrevious = -1;
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
	double max = areaData[indexMinMax[1]];
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
		function<void()> fn = bind(&WTPAINT::areaClicked, this, ii);
		wpArea->clicked().connect(fn);
		area.push_back(wpArea.get());
		this->addArea(move(wpArea));
	}
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
	Wt::WBrush brush(Wt::BrushStyle::Solid);
	painter.setBrush(brush);
	if (area.size() < 1) { return; }

	if (indexAreaSel < 0)  // Nothing selected, so all regions are drawn.
	{
		paintRegionAll(painter);
		//paintLegendBarV(painter);
		//paintLegendBox(painter, 0);
	}
	else if (indexAreaPrevious < 0)
	{
		if (indexAreaSel != 0) { paintRegion(painter, indexAreaSel, 1.0); }
		//paintLegendBox(painter, indexAreaSel);
	}
	else
	{
		if (indexAreaPrevious != 0) { paintRegion(painter, indexAreaPrevious, colourDimPercent); }
		if (indexAreaSel != 0) { paintRegion(painter, indexAreaSel, 1.0); }
		//paintLegendBox(painter, indexAreaSel);
	}
}
void WTPAINT::paintLegendBarV(Wt::WPainter& painter)
{
	int numColour = numColourBands + 1;
	if (keyColour.size() != numColour) { initColour(); }
	double dLen = dHeight - (2.0 * barThickness);
	Wt::WRectF rectV(barThickness, barThickness, barThickness, dLen);
	Wt::WGradient wGrad;
	Wt::WColor wColour;
	Wt::WString wsTemp;
	string temp;
	vector<int> scaleValues = getScaleValues(numColour);
	double wGradX = 1.5 * barThickness;
	double wGradY0 = dLen + barThickness - 1.0;
	double wGradY1 = barThickness + 1.0;
	wGrad.setLinearGradient(wGradX, wGradY0, wGradX, wGradY1);
	double bandWidth = 1.0 / (double)(numColourBands);
	double tickX = 2.0 * barThickness, tickY, dTemp, xCoord, yCoord;
	for (int ii = 0; ii < numColour; ii++)
	{
		dTemp = (double)ii * bandWidth;
		wColour.setRgb(keyColour[ii][0], keyColour[ii][1], keyColour[ii][2]);
		wGrad.addColorStop(dTemp, wColour);
		tickY = wGradY0 - (dTemp * dLen) + 1.0;
		painter.drawLine(tickX, tickY, tickX + (barThickness / 2.0), tickY);
		temp = to_string(scaleValues[ii]);
		wsTemp = Wt::WString::fromUTF8(temp);
		xCoord = tickX + barThickness;
		if (ii == 0) { yCoord = tickY - 14.0; }
		else if (ii == numColour - 1) { yCoord = tickY; }
		else { yCoord = tickY - 7.0; }
		painter.drawText(xCoord, yCoord, 100.0, barThickness, Wt::AlignmentFlag::Left, wsTemp);
	}
	Wt::WBrush wBrush(wGrad);
	painter.setBrush(wBrush);
	painter.drawRect(rectV);
}
void WTPAINT::paintLegendBox(Wt::WPainter& painter, int index)
{
	string shortName, sBase, temp;
	vector<string> boxText;
	size_t pos1 = dataName[0].find("Total - "), indexBox = 0;
	if (pos1 < dataName[0].size())
	{
		boxText.resize(5);
		boxText[0] = "Showing";
		pos1 += 8;
		sBase = dataName[0].substr(pos1);
		for (int ii = 1; ii < dataName.size() - 1; ii++)
		{
			temp = dataName[ii];
			while (temp[0] == '+') { temp.erase(temp.begin()); }
			sBase += "->" + temp;
		}
		boxText[1] = sBase + " WITH";
		boxText[2] = dataName.back();
		indexBox = 3;
	}
	else
	{
		boxText.resize(dataName.size() + 3);
		boxText[0] = "Showing";
		for (int ii = 0; ii < dataName.size(); ii++)
		{
			boxText[ii + 1] = dataName[ii];
		}
		indexBox = dataName.size() + 1;
	}
	boxText[indexBox] = "for the region";

	double dHeight = 21.0 * (double)boxText.size();
	Wt::WRectF box(dWidth - legendBoxDim[0] - 15.0, 1.0, legendBoxDim[0], dHeight);
	Wt::WBrush wBrush(Wt::StandardColor::White);
	painter.setBrush(wBrush);
	painter.drawRect(box);
	pos1 = areaName[index].find(" or ");
	if (pos1 < areaName[index].size()) 
	{
		shortName = areaName[index].substr(0, pos1);
	}
	else { shortName = areaName[index]; }
	pos1 = shortName.find("(Canada)");
	if (pos1 < shortName.size()) { shortName.resize(pos1); }
	boxText[indexBox + 1] = shortName + " = " + to_string(int(areaData[index]));
	Wt::WString wsTemp; 
	double dTemp = box.y() - 15.0;
	for (int ii = 0; ii < boxText.size(); ii++)
	{
		wsTemp = Wt::WString::fromUTF8(boxText[ii]);
		dTemp += 20.0;
		box.setY(dTemp);
		painter.drawText(box, Wt::AlignmentFlag::Center, Wt::TextFlag::SingleLine, wsTemp);
	}
}
void WTPAINT::paintRegion(Wt::WPainter& painter, int index, double percent)
{
	Wt::WPainterPath wpPath = Wt::WPainterPath(border[index][0]);
	for (int jj = 1; jj < border[index].size(); jj++)
	{
		wpPath.lineTo(border[index][jj]);
	}
	wpPath.closeSubPath();
	Wt::WColor wColour(Wt::StandardColor::White);
	Wt::WBrush wBrush(wColour);
	painter.setBrush(wBrush);
	painter.drawPath(wpPath);
	setWColour(wColour, areaColour[index], percent);
	wBrush.setColor(wColour);
	painter.setBrush(wBrush);
	painter.drawPath(wpPath);
}
void WTPAINT::paintRegionAll(Wt::WPainter& painter)
{
	Wt::WColor wColour(Wt::StandardColor::White);
	Wt::WBrush wBrush(wColour);
	painter.setBrush(wBrush);
	for (int ii = border.size() - 1; ii >= 0; ii--)
	{
		Wt::WPainterPath wpPath = Wt::WPainterPath(border[ii][0]);
		for (int jj = 1; jj < border[ii].size(); jj++)
		{
			wpPath.lineTo(border[ii][jj]);
		}
		wpPath.closeSubPath();
		painter.drawPath(wpPath);
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
vector<Wt::WPointF> WTPAINT::scaleParentToWidget(vector<vector<vector<double>>>& vvvdBorder, vector<vector<double>>& vvdFrame)
{
	// Returns the parent region's border (in pixels), scaled to the widget.
	if (dHeight < 0.0 || dWidth < 0.0) { jf.err("No widget dimensions-wtpaint.scaleParentToWidget"); }
	vector<Wt::WPointF> parentBorder(vvvdBorder[0].size());
	double imgWidthKM = vvdFrame[1][0] - vvdFrame[0][0];
	double imgHeightKM = vvdFrame[1][1] - vvdFrame[0][1];
	double xRatio = imgWidthKM / dWidth;  // km per pixel
	double yRatio = imgHeightKM / dHeight;
	double ratio = max(xRatio, yRatio);
	for (int ii = 0; ii < vvvdBorder[0].size(); ii++)
	{
		parentBorder[ii].setX(vvvdBorder[0][ii][0] / ratio);
		parentBorder[ii].setY(vvvdBorder[0][ii][1] / ratio);
	}
	widgetPPKM = 1.0 / ratio;
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
	areaDataChildren.assign(areaData.begin() + 1, areaData.end());
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
	int floor, indexStart;
	vector<int> indexMinMax;
	if (numArea == 1)
	{
		//
	}
	else if (testExcludeParent())
	{
		indexMinMax = jf.minMax(areaDataChildren);
		dMin = areaDataChildren[indexMinMax[0]];
		dMax = areaDataChildren[indexMinMax[1]];
		areaColour[0] = extraColour;  // Parent.
		indexStart = 1;
	}
	else
	{
		indexMinMax = jf.minMax(areaData);
		dMin = areaData[indexMinMax[0]];
		dMax = areaData[indexMinMax[1]];
		indexStart = 0;
	}
	for (int ii = indexStart; ii < numArea; ii++)
	{
		if (areaData[ii] == -1.0)  // Data is missing, so draw as grey.
		{
			areaColour[ii][0] = 180;
			areaColour[ii][1] = 180;
			areaColour[ii][2] = 180;
			continue;
		}
		percentage = (areaData[ii] - dMin) / dMax;
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
