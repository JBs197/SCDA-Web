#include "wtfunc.h"

void WTFUNC::areaClicked(int index)
{
	indexAreaSel = index;
	update();
}
void WTFUNC::drawMap(vector<vector<Wt::WPointF>>& Areas)
{
	areas = Areas;
	update();
}
void WTFUNC::err(string func)
{
	jf.err(func);
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
	painter.setPen(pen);
	
	if (areas.size() < 1) { return; }
	for (int ii = 0; ii < areas.size() - 1; ii++) 
	{                                  // Note that the parent region is done last.
		auto wpArea = make_unique<Wt::WPolygonArea>();
		wpArea->setPoints(areas[ii + 1]);
		function<void()> fn = bind(&WTFUNC::areaClicked, this, ii);
		wpArea->clicked().connect(fn);
		this->addArea(move(wpArea));
		Wt::WPainterPath wpPath = Wt::WPainterPath(areas[ii + 1][0]);
		for (int jj = 1; jj < areas[ii + 1].size(); jj++)
		{
			wpPath.lineTo(areas[ii + 1][jj]);
		}
		if (ii == indexAreaSel)
		{
			painter.save();
			painter.setPen(penSel);
			painter.drawPath(wpPath);
			painter.restore();
		}
		else { painter.drawPath(wpPath); }		
	}
	int index = areas.size() - 1;
	auto wpArea = make_unique<Wt::WPolygonArea>();
	wpArea->setPoints(areas[0]);
	function<void()> fn = bind(&WTFUNC::areaClicked, this, index);
	wpArea->clicked().connect(fn);
	this->addArea(move(wpArea));
	Wt::WPainterPath wpPath = Wt::WPainterPath(areas[0][0]);
	for (int jj = 1; jj < areas[0].size(); jj++)
	{
		wpPath.lineTo(areas[0][jj]);
	}
	painter.drawPath(wpPath);

}
