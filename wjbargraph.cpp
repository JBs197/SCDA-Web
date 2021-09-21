#include "wjbargraph.h"

void WJPARAMPANEL::addEndspace()
{
	int numRow = gLayout->rowCount();
	gLayout->setRowStretch(numRow, 1);
}
void WJPARAMPANEL::addParameter(string sParameter, vector<Wt::WColor>& colours)
{
	Wt::WContainerWidget* wBox = (Wt::WContainerWidget*)this->centralWidget();
	Wt::WGridLayout* gLayout = (Wt::WGridLayout*)wBox->layout();
	int index = gLayout->rowCount();
	
	if (colours.size() < 4)
	{
		auto hLayout = make_unique<Wt::WHBoxLayout>();
		for (int ii = 0; ii < colours.size(); ii++)
		{
			auto colourRect = make_unique<Wt::WText>();
			colourRect->setMinimumSize(wlWidthColour, wlHeightColour);
			Wt::WCssDecorationStyle& style = colourRect->decorationStyle();
			style.setBackgroundColor(colours[ii]);
			style.setBorder(wbColour);
			hLayout->addWidget(move(colourRect));
		}
		gLayout->addLayout(move(hLayout), index, 0, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);
	}
	else
	{
		auto gcLayout = make_unique<Wt::WGridLayout>();
		int gcCol = 0;
		for (int ii = 0; ii < colours.size(); ii++)
		{
			auto colourRect = make_unique<Wt::WText>();
			colourRect->setMinimumSize(wlWidthColour, wlHeightColour);
			Wt::WCssDecorationStyle& style = colourRect->decorationStyle();
			style.setBackgroundColor(colours[ii]);
			style.setBorder(wbColour);
			if (ii % 2 == 0)
			{
				gcLayout->addWidget(move(colourRect), 0, gcCol, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);
			}
			else 
			{
				gcLayout->addWidget(move(colourRect), 1, gcCol, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);
				gcCol++;
			}
		}
		gLayout->addLayout(move(gcLayout), index, 0, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);
	}

	auto parameterText = make_unique<Wt::WText>(sParameter);
	parameterText->setWordWrap(0);
	if (index % 2 == 1)
	{
		Wt::WCssDecorationStyle& style = parameterText->decorationStyle();
		style.setBackgroundColor(wcGrey);
	}
	gLayout->addWidget(move(parameterText), index, 1, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Middle);
}
void WJPARAMPANEL::clear()
{
	Wt::WContainerWidget* cwTemp = titleBarWidget();
	if (cwTemp != nullptr) { cwTemp->clear(); }
	cwTemp = (Wt::WContainerWidget*)centralWidget();
	if (cwTemp != nullptr) { cwTemp->clear(); }
}
void WJPARAMPANEL::init(string sTitle)
{
	Wt::WString wsTemp = Wt::WString::fromUTF8(sTitle);
	setTitle(wsTemp);
	auto boxUnique = make_unique<Wt::WContainerWidget>();
	auto layoutUnique = make_unique<Wt::WGridLayout>();
	layoutUnique->setColumnStretch(1, 1);
	gLayout = boxUnique->setLayout(move(layoutUnique));
	wBox = setCentralWidget(move(boxUnique));

	wlWidthColour = Wt::WLength(40.0);
	wlHeightColour = Wt::WLength(25.0);
	wbColour = Wt::WBorder(Wt::BorderStyle::Solid, Wt::BorderWidth::Medium, Wt::WColor(Wt::StandardColor::Black));
	wcGrey = Wt::WColor(210, 210, 210);
}

void WJBARGRAPH::addDataset(vector<vector<string>>& vvsData, vector<string>& vsParam)
{
	// vvsData has form [x-axis category][category label, bar value0, bar value1, ...].	
	WJDATASET wjds;
	int numRegion = vvsData.size();
	wjds.vsData.resize(numRegion);
	if (vDataset.size() < 1)
	{
		vsRegion.resize(numRegion);
		mapIndexRegion.clear();
		for (int ii = 0; ii < numRegion; ii++)
		{
			vsRegion[ii] = vvsData[ii][0];
			mapIndexRegion.emplace(vvsData[ii][0], ii);
			wjds.vsData[ii] = vvsData[ii][1];
		}
	}
	else
	{
		int index;
		for (int ii = 0; ii < numRegion; ii++)
		{
			index = mapIndexRegion.at(vvsData[ii][0]);
			if (index != ii) { jf.err("Region-data mismatch-wjbargraph.addDataset"); }
			wjds.vsData[ii] = vvsData[ii][1];
		}
	}
	wjds.vsParameter = vsParam;
	for (int ii = 0; ii < vsParam.size(); ii++)
	{
		wjds.setParameter.emplace(vsParam[ii]);
	}
	vDataset.push_back(wjds);
}
void WJBARGRAPH::configureChart()
{
	if (chart == nullptr) { jf.err("No init-wjbargraph.configureChart"); }
	Wt::Chart::WAxis& yAxis = chart->axis(Wt::Chart::Axis::Y);
	yAxis.setLabelFormat("%.8g");
	yAxis.setTitle(unit);

	Wt::Chart::WAxis& xAxis = chart->axis(Wt::Chart::Axis::X);
	auto orientation = chart->orientation();
	if (orientation == Wt::Orientation::Vertical) 
	{ 
		xAxis.setLabelAngle(30.0); 
		chart->setPlotAreaPadding(120, Wt::Side::Bottom);
		chart->setPlotAreaPadding(60, Wt::Side::Left);
	}
	else 
	{ 
		xAxis.setLabelAngle(0.0);
		chart->setPlotAreaPadding(40, Wt::Side::Bottom);
		chart->setPlotAreaPadding(100, Wt::Side::Left);
	}
	chart->setPlotAreaPadding(40, Wt::Side::Top);
	chart->setPlotAreaPadding(5, Wt::Side::Right);
}
void WJBARGRAPH::display()
{
	if (chart != nullptr) { removeWidget(chart); }
	auto layout = make_unique<Wt::WVBoxLayout>();
	auto chartUnique = makeChart();
	chart = layout->addWidget(move(chartUnique));
	configureChart();
	vector<Wt::WColor> seriesColour = getSeriesColour();

	auto ppu = makeWJPP(ppUnique, "Unique Parameters");
	auto ppd = makeWJPP(ppDiff, "Differentiating Parameters");
	auto ppc = makeWJPP(ppCommon, "Common Parameters");
	ppUnique = layout->addWidget(move(ppu));
	ppDiff = layout->addWidget(move(ppd));
	ppCommon = layout->addWidget(move(ppc));

	parameterPopulation();
	parameterSorting(seriesColour);

	/*
	vector<Wt::WColor> colours;
	string sParameter;
	int count, countCommon = 0, countDiff = 0, countUnique = 0;
	int numSeries = seriesColour.size();
	for (int ii = 0; ii < vviParameter.size(); ii++)
	{
		count = 0;
		colours.clear();
		for (int jj = 0; jj < numSeries; jj++)
		{
			if (vviParameter[ii][jj] == 1)
			{
				count++;
				colours.push_back(seriesColour[jj]);
			}
		}
		sParameter = mapValueParameter.at(ii);
		if (count == 1) 
		{ 
			ppu->addParameter(sParameter, colours); 
			countUnique++;
		}
		else if (count == numSeries) 
		{ 
			ppc->addParameter(sParameter, colours); 
			countCommon++;
		}
		else 
		{ 
			ppd->addParameter(sParameter, colours); 
			countDiff++;
		}
	}
	*/

	this->setLayout(move(layout));
}
vector<Wt::WColor> WJBARGRAPH::getSeriesColour()
{
	if (chart == nullptr) { jf.err("No chart loaded-wjbargraph.getSeriesColour"); }
	int numCol = model->columnCount();
	vector<Wt::WColor> seriesColour(numCol - 1);
	for (int ii = 1; ii < numCol; ii++)
	{
		Wt::Chart::WDataSeries& series = chart->series(ii);
		Wt::WBrush brush = series.brush();
		const Wt::WColor& barColour = brush.color();
		seriesColour[ii - 1] = barColour;
	}
	return seriesColour;
}
unique_ptr<Wt::Chart::WCartesianChart> WJBARGRAPH::makeChart()
{
	auto modelTemp = make_shared<Wt::WStandardItemModel>(vsRegion.size(), vDataset.size() + 1);
	swap(modelTemp, model);
	setModelValues();
	auto chartTemp = make_unique<Wt::Chart::WCartesianChart>();
	chartTemp->setModel(model);
	chartTemp->setXSeriesColumn(0);
	chartTemp->setMinimumSize(wlAuto, wlChartHeight);
	int numCol = model->columnCount();
	for (int ii = 1; ii < numCol; ii++)
	{
		auto series = make_unique<Wt::Chart::WDataSeries>(ii, Wt::Chart::SeriesType::Bar);
		series->setShadow(Wt::WShadow(3, 3, Wt::WColor(0, 0, 0, 127), 3));
		chartTemp->addSeries(std::move(series));
	}
	return chartTemp;
}
unique_ptr<WJPARAMPANEL> WJBARGRAPH::makeWJPP(WJPARAMPANEL*& wjpp, string sTitle)
{
	if (wjpp != nullptr)
	{
		this->removeWidget(wjpp);
		wjpp = nullptr;
	}
	auto ppUnique = make_unique<WJPARAMPANEL>(sTitle);
	size_t pos1 = sTitle.find("Common");
	if (pos1 < sTitle.size()) { ppUnique->setObjectName("wjppCommon"); }
	else
	{
		pos1 = sTitle.find("Unique");
		if (pos1 < sTitle.size()) { ppUnique->setObjectName("wjppUnique"); }
		else
		{
			pos1 = sTitle.find("Diff");
			if (pos1 < sTitle.size()) { ppUnique->setObjectName("wjppDiff"); }
			else { jf.err("Cannot identify WJPARAMPANEL-wjbargraph-makeWJPP"); }
		}
	}
	return ppUnique;
}
void WJBARGRAPH::parameterPopulation()
{
	mapValueParameter.clear();
	mapIndexParameter.clear();
	vviParameter.clear();
	int numDataset = vDataset.size();
	int index;
	for (int ii = 0; ii < vDataset[0].vsParameter.size(); ii++)
	{
		for (int jj = 0; jj < numDataset; jj++)
		{
			if (mapIndexParameter.count(vDataset[jj].vsParameter[ii]))
			{
				index = mapIndexParameter.at(vDataset[jj].vsParameter[ii]);				
			}
			else
			{
				index = vviParameter.size();
				vviParameter.push_back(vector<int>(numDataset, 0));
				mapIndexParameter.emplace(vDataset[jj].vsParameter[ii], index);
				mapValueParameter.emplace(index, vDataset[jj].vsParameter[ii]);
			}
			vviParameter[index][jj]++;
		}
	}
}
void WJBARGRAPH::parameterSorting(vector<Wt::WColor>& seriesColour)
{
	if (seriesColour.size() != vviParameter[0].size()) { jf.err("Size mismatch-wjbargraph.parameterSorting"); }
	vector<Wt::WColor> colours;
	string sParameter;
	int count, countCommon = 0, countDiff = 0, countUnique = 0;
	int numSeries = seriesColour.size();
	for (int ii = 0; ii < vviParameter.size(); ii++)
	{
		count = 0;
		colours.clear();
		for (int jj = 0; jj < numSeries; jj++)
		{
			if (vviParameter[ii][jj] == 1)
			{
				count++;
				colours.push_back(seriesColour[jj]);
			}
		}
		sParameter = mapValueParameter.at(ii);
		if (count == 1) 
		{ 
			ppUnique->addParameter(sParameter, colours); 
			countUnique++;
		}
		else if (count == numSeries) 
		{ 
			ppCommon->addParameter(sParameter, colours); 
			countCommon++;
		}
		else 
		{ 
			ppDiff->addParameter(sParameter, colours);
			countDiff++;
		}
	}

	if (countUnique < 1) { ppUnique->setHidden(1); }
	else { ppUnique->addEndspace(); }
	if (countDiff < 1) { ppDiff->setHidden(1); }
	else { ppDiff->addEndspace(); }
	if (countCommon < 1) { ppCommon->setHidden(1); }
	else { ppCommon->addEndspace(); }
}
void WJBARGRAPH::reset()
{
	unit = "";
	activeCata.clear();
	vDataset.clear();
	if (chart != nullptr)
	{
		removeWidget(chart);
		chart = nullptr;
	}
	if (ppCommon != nullptr)
	{
		removeWidget(ppCommon);
		ppCommon = nullptr;
	}
	if (ppDiff != nullptr)
	{
		removeWidget(ppDiff);
		ppDiff = nullptr;
	}
	if (ppUnique != nullptr)
	{
		removeWidget(ppUnique);
		ppUnique = nullptr;
	}
	if (model != nullptr)
	{
		model.reset();
		model = nullptr;
	}
	
}
void WJBARGRAPH::setModelValues()
{
	for (int ii = 0; ii < vsRegion.size(); ii++)
	{
		model->setData(ii, 0, vsRegion[ii], Wt::ItemDataRole::Display);
	}
	for (int ii = 0; ii < vDataset.size(); ii++)
	{
		for (int jj = 0; jj < vDataset[ii].vsData.size(); jj++)
		{
			model->setData(jj, ii + 1, vDataset[ii].vsData[jj], Wt::ItemDataRole::Display);
		}
	}
}
