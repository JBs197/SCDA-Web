#include "wtfunc.h"

void WTFUNC::drawMap(Wt::WPainterPath& wpp)
{
	wpPath = wpp;
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
void WTFUNC::makeWPPath(vector<vector<int>>& frameTLBR, vector<vector<int>>& border, vector<double>& windowDim, Wt::WPainterPath& wpPath)
{
	int numPoints = border.size();
	vector<vector<double>> borderD(numPoints, vector<double>(2));
	for (int ii = 0; ii < numPoints; ii++)
	{
		borderD[ii][0] = (double)(border[ii][0] - frameTLBR[0][0]);
		borderD[ii][1] = (double)(border[ii][1] - frameTLBR[0][1]);
	}
	vector<double> mapDim(2);
	mapDim[0] = (double)(frameTLBR[1][0] - frameTLBR[0][0]);
	mapDim[1] = (double)(frameTLBR[1][1] - frameTLBR[0][1]);
	double xRatio, yRatio, ratio;
	xRatio = windowDim[0] / mapDim[0];
	yRatio = windowDim[1] / mapDim[1];
	if (xRatio < yRatio) { ratio = xRatio; }
	else { ratio = yRatio; }
	for (int ii = 0; ii < numPoints; ii++)
	{
		borderD[ii][0] *= ratio;
		borderD[ii][1] *= ratio;
	}
	wpPath.moveTo(borderD[0][0], borderD[0][1]);
	for (int ii = 1; ii < numPoints; ii++)
	{
		wpPath.lineTo(borderD[ii][0], borderD[ii][1]);
	}
}
void WTFUNC::paintEvent(Wt::WPaintDevice* paintDevice)
{
	Wt::WPainter painter(paintDevice);
	Wt::WLength penWidth(3.0, Wt::LengthUnit::Pixel);
	Wt::WPen pen;
	pen.setWidth(penWidth);
	painter.setPen(pen);
	painter.drawPath(wpPath);
}
