#include "wjbargraph.h"

void WJPARAMPANEL::addEndspace()
{
	Wt::WContainerWidget* wBox = (Wt::WContainerWidget*)this->centralWidget();
	Wt::WGridLayout* gLayout = (Wt::WGridLayout*)wBox->layout();
	int numRow = gLayout->rowCount();
	auto dummy = make_unique<Wt::WText>("");
	gLayout->addWidget(move(dummy), numRow + 1, 1, Wt::AlignmentFlag::Bottom);
	gLayout->setRowStretch(numRow, 1);
	gLayout->setColumnStretch(1, 2);
}
void WJPARAMPANEL::addParameter(string sParameter, vector<int> vIndex)
{
	Wt::WContainerWidget* wBox = (Wt::WContainerWidget*)this->centralWidget();
	Wt::WGridLayout* gLayout = (Wt::WGridLayout*)wBox->layout();
	int indexRow = gLayout->rowCount();
	
	auto gcLayout = make_unique<Wt::WGridLayout>();
	if (vIndex.size() < 4)  // Display everything on a single line.
	{
		for (int ii = 0; ii < vIndex.size(); ii++)
		{
			auto wjRect = make_unique<WJPARAMRECT>(seriesColour[vIndex[ii]]);
			mapSeries.emplace(wjRect->id(), vIndex[ii]);
			gcLayout->addWidget(move(wjRect), 0, ii, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);
		}
		gLayout->addLayout(move(gcLayout), indexRow, 0, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);
	}
	else  // Display the parameter on two lines. 
	{
		int gcCol = 0;
		for (int ii = 0; ii < vIndex.size(); ii++)
		{
			auto wjRect = make_unique<WJPARAMRECT>(seriesColour[vIndex[ii]]);
			mapSeries.emplace(wjRect->id(), vIndex[ii]);
			if (ii % 2 == 0)
			{
				gcLayout->addWidget(move(wjRect), 0, gcCol, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);
			}
			else 
			{
				gcLayout->addWidget(move(wjRect), 1, gcCol, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);
				gcCol++;
			}
		}
		gLayout->addLayout(move(gcLayout), indexRow, 0, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);
	}

	auto parameterText = make_unique<Wt::WText>();
	parameterText->setTextFormat(Wt::TextFormat::Plain);
	parameterText->setWordWrap(0);
	Wt::WString wsTemp = Wt::WString::fromUTF8(sParameter);
	parameterText->setText(wsTemp);
	Wt::WCssDecorationStyle& style = parameterText->decorationStyle();
	style.font().setSize(Wt::FontSize::Large);
	gLayout->addWidget(move(parameterText), indexRow, 1, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Middle);
}
void WJPARAMPANEL::clear()
{
	Wt::WContainerWidget* cwTemp = titleBarWidget();
	if (cwTemp != nullptr) { cwTemp->clear(); }
	cwTemp = (Wt::WContainerWidget*)centralWidget();
	if (cwTemp != nullptr) { cwTemp->clear(); }
	mapSeries.clear();
}
void WJPARAMPANEL::init(string sTitle, Wt::WLink linkIconTrash)
{
	setTitleBar(1);
	Wt::WContainerWidget* cwTitle = titleBarWidget();
	cwTitle->setPadding(0.0);
	auto hLayout = make_unique<Wt::WHBoxLayout>();
	hLayout->setSpacing(40);

	Wt::WString wsTemp = Wt::WString::fromUTF8(sTitle);
	auto wText = make_unique<Wt::WText>(wsTemp);
	wText->decorationStyle().font().setSize(Wt::FontSize::Large);
	hLayout->addWidget(move(wText), 0, Wt::AlignmentFlag::Middle);

	auto wjTrash = make_unique<WJTRASH>(linkIconTrash);
	wjTrash->deleteSignal().connect(this, std::bind(&WJPARAMPANEL::removeParameter, this, std::placeholders::_1));
	hLayout->addWidget(move(wjTrash));

	hLayout->addStretch(1);
	cwTitle->setLayout(move(hLayout));

	auto boxUnique = make_unique<Wt::WContainerWidget>();
	boxUnique->setOverflow(Wt::Overflow::Auto);
	auto layoutUnique = make_unique<Wt::WGridLayout>();
	boxUnique->setLayout(move(layoutUnique));
	setCentralWidget(move(boxUnique));
	
	mapSeries.clear();
	wlWidthColour = Wt::WLength(40.0);
	wlHeightColour = Wt::WLength(25.0);
	wbColour = Wt::WBorder(Wt::BorderStyle::Solid, Wt::BorderWidth::Medium, Wt::WColor(Wt::StandardColor::Black));
	wcGrey = Wt::WColor(210, 210, 210);
}
void WJPARAMPANEL::removeParameter(const string& sID)
{
	// Does not actually remove anything - it informs the main SCDAwidget which 
	// data series is to be removed. 
	int seriesIndex = mapSeries.at(sID);
	deleteSignal_.emit(seriesIndex);
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
void WJBARGRAPH::addTipWheel(int layoutIndex)
{
	Wt::WFlags<Wt::Orientation> flags;
	Wt::WColor wcTip(255, 255, 180);
	auto box = make_unique<Wt::WContainerWidget>();
	box->decorationStyle().setBackgroundColor(wcTip);
	box->setPadding(0.0);
	auto tipLayout = make_unique<Wt::WHBoxLayout>();
	auto boxDummy = make_unique<Wt::WContainerWidget>();
	boxDummy->setPadding(0.0);
	auto text = make_unique<Wt::WText>();
	text->decorationStyle().font().setSize(Wt::FontSize::Small);
	text->decorationStyle().font().setWeight(Wt::FontWeight::Bold);
	Wt::WString wsTip = "Tip: Hold the Ctrl key while scrolling with your mouse wheel to zoom in and out of the bar graph.";
	text->setText(wsTip);
	boxDummy->addWidget(move(text));
	tipLayout->addWidget(move(boxDummy));
	tipLayout->addStretch(1);
	auto button = make_unique<Wt::WPushButton>();
	Wt::WCssDecorationStyle& buttonStyle = button->decorationStyle();
	buttonStyle.setBackgroundColor(Wt::StandardColor::White);
	buttonStyle.setBackgroundImage(linkIconClose, flags, Wt::Side::CenterX | Wt::Side::CenterY);
	button->clicked().connect(this, std::bind(&WJBARGRAPH::removeTipWheel, this, layoutIndex));
	tipLayout->addWidget(move(button));
	box->setLayout(move(tipLayout));
	Wt::WVBoxLayout* bigLayout = (Wt::WVBoxLayout*)this->layout();
	bigLayout->insertWidget(layoutIndex, move(box));
}
void WJBARGRAPH::configureChart()
{
	if (chart == nullptr) { jf.err("No init-wjbargraph.configureChart"); }
	chart->setAutoLayoutEnabled(1);
	map<Wt::WFlags<Wt::KeyboardModifier>, Wt::Chart::InteractiveAction> mapWheel;
	mapWheel.emplace(Wt::KeyboardModifier::None, Wt::Chart::InteractiveAction::PanMatching);
	mapWheel.emplace(Wt::KeyboardModifier::Control, Wt::Chart::InteractiveAction::ZoomX);
	chart->setWheelActions(mapWheel);

	Wt::Chart::WAxis& yAxis = chart->axis(Wt::Chart::Axis::Y);
	yAxis.setLabelFormat("%.8g");
	yAxis.setTitle(unit);

	Wt::Chart::WAxis& xAxis = chart->axis(Wt::Chart::Axis::X);
	auto orientation = chart->orientation();
	if (orientation == Wt::Orientation::Vertical)
	{
		xAxis.setLabelAngle(30.0);
	}
	else
	{
		xAxis.setLabelAngle(0.0);
	}
}
void WJBARGRAPH::display()
{
	if (chart != nullptr) { removeWidget(chart); }
	this->setOverflow(Wt::Overflow::Auto);
	auto layout = make_unique<Wt::WVBoxLayout>();
	auto chartUnique = makeChart();
	chart = layout->addWidget(move(chartUnique));
	configureChart();

	vector<Wt::WColor> seriesColour = getSeriesColour();
	auto ppu = makeWJPP(ppUnique, "Unique Parameters", seriesColour);
	auto ppd = makeWJPP(ppDiff, "Differentiating Parameters", seriesColour);
	auto ppc = makeWJPP(ppCommon, "Common Parameters", seriesColour);
	ppUnique = layout->addWidget(move(ppu));
	ppDiff = layout->addWidget(move(ppd));
	ppCommon = layout->addWidget(move(ppc));
	layout->addStretch(1);

	parameterPopulation();
	parameterSorting();

	this->setLayout(move(layout));
	int numRow = model->rowCount();
	if (numRow > 14) 
	{ 
		Wt::Chart::WAxis& wX = chart->axis(Wt::Chart::Axis::X);
		wX.setZoomRange(0.0, 14.0);
	}
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
	chartTemp->setPreferredMethod(Wt::RenderMethod::HtmlCanvas);
	chartTemp->setModel(model);
	chartTemp->setXSeriesColumn(0);
	chartTemp->setMinimumSize(wlAuto, wlChartHeight);
	chartTemp->setZoomEnabled(1);
	chartTemp->setPanEnabled(1);
	int numCol = model->columnCount();
	for (int ii = 1; ii < numCol; ii++)
	{
		auto series = make_unique<Wt::Chart::WDataSeries>(ii, Wt::Chart::SeriesType::Bar);
		series->setShadow(Wt::WShadow(3, 3, Wt::WColor(0, 0, 0, 127), 3));
		chartTemp->addSeries(std::move(series));
	}
	return chartTemp;
}
unique_ptr<WJPARAMPANEL> WJBARGRAPH::makeWJPP(WJPARAMPANEL*& wjpp, string sTitle, vector<Wt::WColor>& seriesColour)
{
	if (wjpp != nullptr)
	{
		this->removeWidget(wjpp);
		wjpp = nullptr;
	}
	auto ppUnique = make_unique<WJPARAMPANEL>(sTitle, linkIconTrash, seriesColour);
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
void WJBARGRAPH::parameterSorting()
{
	//if (seriesColour.size() != vviParameter[0].size()) { jf.err("Size mismatch-wjbargraph.parameterSorting"); }
	string sParameter;
	vector<int> vIndex;
	int count, countCommon = 0, countDiff = 0, countUnique = 0;
	const vector<unique_ptr<Wt::Chart::WDataSeries>>& vSeries = chart->series();
	int numSeries = vSeries.size();

	for (int ii = 0; ii < vviParameter.size(); ii++)
	{
		count = 0;
		vIndex.clear();
		for (int jj = 0; jj < numSeries; jj++)
		{
			if (vviParameter[ii][jj] == 1)
			{
				count++;
				vIndex.push_back(jj);
			}
		}
		sParameter = mapValueParameter.at(ii);
		if (count == 1) 
		{ 
			ppUnique->addParameter(sParameter, vIndex);
			countUnique++;
		}
		else if (count == numSeries) 
		{ 
			ppCommon->addParameter(sParameter, vIndex);
			countCommon++;
		}
		else 
		{ 
			ppDiff->addParameter(sParameter, vIndex);
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
int WJBARGRAPH::removeDataset(int seriesIndex)
{
	vDataset.erase(vDataset.begin() + seriesIndex);
	return vDataset.size();
}
void WJBARGRAPH::removeTipWheel(int layoutIndex)
{
	Wt::WVBoxLayout* bigLayout = (Wt::WVBoxLayout*)this->layout();
	Wt::WLayoutItem* layoutItem = bigLayout->itemAt(layoutIndex);
	bigLayout->removeItem(layoutItem);
	tipSignal_.emit("barGraphWheel");
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
	double dnum;
	size_t pos1, pos2;
	string temp, toolTip;
	for (int ii = 0; ii < vsRegion.size(); ii++)
	{
		model->setData(ii, 0, vsRegion[ii], Wt::ItemDataRole::Display);
	}
	for (int ii = 0; ii < vDataset.size(); ii++)
	{
		for (int jj = 0; jj < vDataset[ii].vsData.size(); jj++)
		{
			model->setData(jj, ii + 1, vDataset[ii].vsData[jj], Wt::ItemDataRole::Display);
			toolTip = vDataset[ii].vsData[jj];
			pos1 = toolTip.find('.');
			pos2 = toolTip.find_first_not_of('0', pos1 + 1);
			if (pos2 > toolTip.size()) { temp = toolTip.substr(0, pos1); }
			else
			{
				dnum = stod(toolTip);
				if (pos1 == 1 || unit == "$") 
				{ 
					dnum *= 100.0; 
					dnum = round(dnum);
					dnum /= 100.0;
					temp = to_string(dnum);
					pos1 = temp.find('.');
					temp.resize(pos1 + 3);
				}
				else 
				{ 
					dnum *= 10.0; 
					dnum = round(dnum);
					dnum /= 10.0;
					temp = to_string(dnum);
					pos1 = temp.find('.');
					temp.resize(pos1 + 2);
				}
			}
			toolTip = temp + " (" + unit + ")";
			model->setData(jj, ii + 1, toolTip, Wt::ItemDataRole::ToolTip);
		}
	}
}
void WJBARGRAPH::widgetMobile(bool mobile)
{

}
