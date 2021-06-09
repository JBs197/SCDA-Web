#include "wtfunc.h"

void WTFUNC::areaClicked(int index)
{
	indexAreaPrevious = indexAreaSel;
	indexAreaSel = index;
	nameAreaSel = areaNames[index];	
	m_map.lock();
	commWidget.push_back(nameAreaSel);
	m_map.unlock();
	update(Wt::PaintFlag::Update);
}
void WTFUNC::clearAreas()
{
	auto listAreas = Wt::WPaintedWidget::areas();
	for (int ii = 0; ii < listAreas.size(); ii++)
	{
		Wt::WPaintedWidget::removeArea(listAreas[ii]);
	}
}
void WTFUNC::drawMap(vector<vector<Wt::WPointF>>& Areas, vector<string>& sidAreaNames)
{
	areas = Areas;  // Form [parent, children].
	areaNames.assign(sidAreaNames.begin() + 1, sidAreaNames.end()); // Form [parent, children].
	indexAreaSel = -1;
	indexAreaPrevious = -1;
	update();
}
void WTFUNC::err(string func)
{
	jf.err(func);
}
vector<vector<int>> WTFUNC::getFrame(vector<Wt::WPointF>& path)
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
void WTFUNC::initAreas(vector<int>& indexOrder)
{
	for (int ii = 0; ii < indexOrder.size(); ii++)
	{
		mapAreas.emplace(areaNames[indexOrder[ii]], indexOrder[ii]);
		auto wpArea = make_unique<Wt::WPolygonArea>();
		wpArea->setPoints(areas[indexOrder[ii]]);
		function<void()> fn = bind(&WTFUNC::areaClicked, this, indexOrder[ii]);
		wpArea->clicked().connect(fn);
		this->addArea(move(wpArea));
	}
}
void WTFUNC::init_proj_dir(string exec_dir)
{
	size_t pos1 = exec_dir.rfind('\\');  // Debug or release.
	pos1 = exec_dir.rfind('\\', pos1 - 1);  // Project folder.
	proj_dir = exec_dir.substr(0, pos1);
}
void WTFUNC::initSize(double w, double h)
{
	wlHeight = Wt::WLength(h);
	wlWidth = Wt::WLength(w);
	resize(wlWidth, wlHeight);
}
vector<Wt::WPointF> WTFUNC::makeWPPath(vector<vector<int>>& frameTLBR, vector<vector<int>>& border, vector<double>& windowDimPosition)
{
	int numPoints = border.size();
	vector<vector<double>> borderD(numPoints, vector<double>(2));
	for (int ii = 0; ii < numPoints; ii++)
	{
		borderD[ii][0] = (double)(border[ii][0] - frameTLBR[0][0]);
		borderD[ii][1] = (double)(border[ii][1] - frameTLBR[0][1]);
	}
	vector<double> mapDim(2);
	double xRatio, yRatio, ratio, myScale, xShift, yShift;
	if (windowDimPosition.size() == 2)  // Indicates the vector contains window 
	{                                   // dimensions (w,h).
		mapDim[0] = (double)(frameTLBR[1][0] - frameTLBR[0][0]);
		mapDim[1] = (double)(frameTLBR[1][1] - frameTLBR[0][1]);
		xRatio = windowDimPosition[0] / mapDim[0];
		yRatio = windowDimPosition[1] / mapDim[1];
		if (xRatio < yRatio) { ratio = xRatio; }
		else { ratio = yRatio; }
		for (int ii = 0; ii < numPoints; ii++)
		{
			borderD[ii][0] *= ratio;
			borderD[ii][1] *= ratio;
		}
		windowDimPosition = { ratio };
	}
	else if (windowDimPosition.size() == 3) // Indicates the vector contains position. 
	{                                       // Form [xShift, yShift, myRatio].
		//xShift = windowDimPosition[0] * windowDimPosition[3];
		//yShift = windowDimPosition[1] * windowDimPosition[4];
		for (int ii = 0; ii < numPoints; ii++)
		{
			borderD[ii][0] *= windowDimPosition[2];
			borderD[ii][1] *= windowDimPosition[2];
			borderD[ii][0] += windowDimPosition[0];
			borderD[ii][1] += windowDimPosition[1];
		}
	}
	else { jf.err("windowDimScaling-wtf.makeWPPath"); }

	vector<Wt::WPointF> area;
	area.resize(numPoints);
	for (int ii = 0; ii < numPoints; ii++)
	{
		area[ii] = Wt::WPointF(borderD[ii][0], borderD[ii][1]);
	}
	return area;
}
void WTFUNC::paintEvent(Wt::WPaintDevice* paintDevice)
{
	Wt::WPainter painter(paintDevice);
	Wt::WLength penWidth(3.0, Wt::LengthUnit::Pixel);
	Wt::WPen pen, penSel(Wt::StandardColor::Red);
	pen.setWidth(penWidth);
	penSel.setWidth(penWidth);
	if (areas.size() < 1) { return; }

	vector<int> indexOrder(areas.size());
	if (indexAreaSel < 0)  // Nothing selected, so parent goes last.
	{
		mapAreas.clear();
		clearAreas();
		for (int ii = 0; ii < areas.size() - 1; ii++)
		{
			indexOrder[ii] = ii + 1;
		}
		indexOrder[areas.size() - 1] = 0;
		painter.setPen(pen);
		initAreas(indexOrder);
		paintRegionAll(painter, indexOrder);
	}
	else if (indexAreaPrevious < 0)
	{
		painter.setPen(penSel);
		paintRegion(painter, indexAreaSel);
	}
	else
	{
		painter.setPen(pen);
		paintRegion(painter, indexAreaPrevious);
		painter.setPen(penSel);
		paintRegion(painter, indexAreaSel);
	}
}
void WTFUNC::paintRegion(Wt::WPainter& painter, int index)
{
	vector<vector<int>> TLBR = getFrame(areas[index]);
	cout << "TL: (" << to_string(TLBR[0][0]) << "," << to_string(TLBR[0][1]) << ")" << endl;
	cout << "BR: (" << to_string(TLBR[1][0]) << "," << to_string(TLBR[1][1]) << ")" << endl;
	Wt::WPainterPath wpPath = Wt::WPainterPath(areas[index][0]);
	for (int jj = 1; jj < areas[index].size(); jj++)
	{
		wpPath.lineTo(areas[index][jj]);
	}
	painter.drawPath(wpPath);
}
void WTFUNC::paintRegionAll(Wt::WPainter& painter, vector<int>& indexOrder)
{
	for (int ii = 0; ii < indexOrder.size(); ii++)
	{
		Wt::WPainterPath wpPath = Wt::WPainterPath(areas[indexOrder[ii]][0]);
		for (int jj = 1; jj < areas[indexOrder[ii]].size(); jj++)
		{
			wpPath.lineTo(areas[indexOrder[ii]][jj]);
		}
		painter.drawPath(wpPath);
	}
}
