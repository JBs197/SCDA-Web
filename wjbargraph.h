#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WGridLayout.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/Chart/WCartesianChart.h>
#include <Wt/Chart/WDataSeries.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/WStandardItem.h>
#include <Wt/WPanel.h>
#include <Wt/WText.h>
#include "jfunc.h"

using namespace std;

struct WJDATASET
{
	unordered_set<string> setParameter;
	vector<string> vsData;
	vector<string> vsParameter;
};

class WJPARAMPANEL : public Wt::WPanel
{
	Wt::WGridLayout* gLayout = nullptr;
	int paramPerLine = 2;
	Wt::WContainerWidget* wBox = nullptr;
	Wt::WBorder wbColour;
	Wt::WColor wcGrey;
	Wt::WLength wlWidthColour, wlHeightColour;

public:
	WJPARAMPANEL(string sTitle) : Wt::WPanel() { init(sTitle); }
	~WJPARAMPANEL() {}

	void addEndspace();
	void addParameter(string sParameter, vector<Wt::WColor>& colours);
	void clear();
	void init(string sTitle);

};

class WJBARGRAPH : public Wt::WContainerWidget
{
	Wt::Chart::WCartesianChart *chart = nullptr;
	unordered_map<string, int> mapIndexParameter;
	unordered_map<string, int> mapIndexRegion;
	unordered_map<int, string> mapValueParameter;
	shared_ptr<Wt::WStandardItemModel> model = nullptr;
	WJPARAMPANEL *ppCommon = nullptr, *ppDiff = nullptr, *ppUnique = nullptr;
	vector<WJDATASET> vDataset;
	vector<string> vsRegion;
	vector<vector<int>> vviParameter;  // Form [indexParameter][count in Dataset0, count in Dataset1, ...]
	Wt::WLength wlChartHeight;

public:
	WJBARGRAPH(Wt::WLength wlHeight) : Wt::WContainerWidget(), wlChartHeight(wlHeight) {}
	WJBARGRAPH() : Wt::WContainerWidget() {}
	~WJBARGRAPH() {}

	string activeCata;
	JFUNC jf;
	string region = "", unit = "";

	void addDataset(vector<vector<string>>& vvsData, vector<string>& vsParameter);
	void configureChart();
	void display();
	vector<Wt::WColor> getSeriesColour();
	unique_ptr<Wt::Chart::WCartesianChart> makeChart();
	unique_ptr<WJPARAMPANEL> makeWJPP(WJPARAMPANEL*& wjpp, string sTitle);
	void parameterPopulation();
	void parameterSorting(vector<Wt::WColor>& seriesColour);
	void reset();
	void setModelValues();
	void widgetMobile(bool mobile);

	Wt::WLength wlAuto = Wt::WLength::Auto;
};

