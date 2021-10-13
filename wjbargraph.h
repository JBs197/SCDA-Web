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
#include <Wt/WPushButton.h>
#include <Wt/WPanel.h>
#include <Wt/WText.h>
#include "wjdrag.h"
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
	Wt::Signal<int> deleteSignal_;
	unordered_map<string, int> mapSeries;  // sID -> seriesIndex
	int paramPerLine = 2;
	const vector<Wt::WColor> seriesColour;
	Wt::WBorder wbColour;
	Wt::WColor wcGrey;
	Wt::WLength wlWidthColour, wlHeightColour;

public:
	WJPARAMPANEL(string sTitle, Wt::WLink linkTrash, vector<Wt::WColor> vColour) : Wt::WPanel(), seriesColour(vColour)
	{ 
		init(sTitle, linkTrash); 
	}
	~WJPARAMPANEL() {}

	set<int> setColourLayout, setTextWidget;

	void addEndspace();
	void addParameter(string sParameter, vector<int> vIndex);
	void clear();
	Wt::Signal<int>& deleteSignal() { return deleteSignal_; }
	void drawTrashRect(vector<int>& vColourIndex);
	void init(string sTitle, Wt::WLink linkIconTrash);
	void removeParameter(const string& sID);
};

class WJBARGRAPH : public Wt::WContainerWidget
{
	Wt::Chart::WCartesianChart *chart = nullptr;
	unordered_map<string, int> mapIndexParameter;
	unordered_map<string, int> mapIndexRegion;
	unordered_map<int, string> mapValueParameter;
	Wt::Signal<string> tipSignal_;
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
	Wt::WLink linkIconClose = Wt::WLink(), linkIconTrash = Wt::WLink();
	shared_ptr<Wt::WStandardItemModel> model = nullptr;
	WJPARAMPANEL *ppCommon = nullptr, *ppDiff = nullptr, *ppUnique = nullptr;
	string region = "", unit = "";

	void addDataset(vector<vector<string>>& vvsData, vector<vector<string>>& vvsParameter);
	void addTipWheel(int layoutIndex);
	void configureChart();
	void display();
	void getParameterAll(vector<vector<double>>& seriesColour, vector<vector<vector<int>>>& panelColourIndex, vector<vector<vector<string>>>& panelText);
	vector<double> getMinMaxY();
	vector<vector<string>> getModelValues();
	int getNumSeries() { return vDataset.size(); }
	vector<Wt::WColor> getSeriesColour();
	string italicize(vector<string>& vsParameter, int italicFreq);
	unique_ptr<Wt::Chart::WCartesianChart> makeChart();
	unique_ptr<WJPARAMPANEL> makeWJPP(WJPARAMPANEL*& wjpp, string sTitle, vector<Wt::WColor>& seriesColour);
	void parameterPopulation();
	void parameterSorting();
	int removeDataset(int seriesIndex);
	void removeTipWheel(int layoutIndex);
	void reset();
	void setModelValues();
	Wt::Signal<string>& tipSignal() { return tipSignal_; }
	void widgetMobile(bool mobile);

	Wt::WLength wlAuto = Wt::WLength::Auto;
};

