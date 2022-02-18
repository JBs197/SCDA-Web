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
#include "jnumber.h"
#include "jstring.h"
#include "wjdrag.h"

struct WJDATASET
{
	std::unordered_set<std::string> setParameter;
	std::vector<std::string> vsData;
	std::vector<std::string> vsParameter;
};

class WJPARAMPANEL : public Wt::WPanel
{
	Wt::Signal<int> deleteSignal_;
	std::unordered_map<std::string, int> mapSeries;  // sID -> seriesIndex
	int paramPerLine = 2;
	const std::vector<Wt::WColor> seriesColour;
	Wt::WBorder wbColour;
	Wt::WColor wcGrey;
	Wt::WLength wlWidthColour, wlHeightColour;

	void err(std::string message);

public:
	WJPARAMPANEL(std::string sTitle, Wt::WLink linkTrash, std::vector<Wt::WColor> vColour) : Wt::WPanel(), seriesColour(vColour)
	{ 
		init(sTitle, linkTrash); 
	}
	~WJPARAMPANEL() {}

	std::set<int> setColourLayout, setTextWidget;

	void addEndspace();
	void addParameter(std::string sParameter, std::vector<int> vIndex);
	void clear();
	Wt::Signal<int>& deleteSignal() { return deleteSignal_; }
	void drawTrashRect(std::vector<int>& vColourIndex);
	void init(std::string sTitle, Wt::WLink linkIconTrash);
	void removeParameter(const std::string& sID);
};

class WJBARGRAPH : public Wt::WContainerWidget
{
	Wt::Chart::WCartesianChart *chart = nullptr;
	JNUMBER jnumber;
	JSTRING jstr;
	std::unordered_map<std::string, int> mapIndexParameter;
	std::unordered_map<std::string, int> mapIndexXName;
	std::unordered_map<int, std::string> mapValueParameter;
	Wt::Signal<std::string> tipSignal_;
	std::vector<WJDATASET> vDataset;
	std::vector<std::string> vXName; // List of x-axis bar groupings (region names, or table headers).
	std::vector<std::vector<int>> vviParameter;  // Form [indexParameter][count in Dataset0, count in Dataset1, ...]
	Wt::WLength wlChartHeight;

	void err(std::string message);

public:
	WJBARGRAPH(Wt::WLength wlHeight) : Wt::WContainerWidget(), wlChartHeight(wlHeight) {}
	WJBARGRAPH() : Wt::WContainerWidget() {}
	~WJBARGRAPH() {}

	std::string activeCata;
	int bgMode = -1;  // 0 = table row, 1 = table col, 2 = map.
	Wt::WLink linkIconClose = Wt::WLink(), linkIconTrash = Wt::WLink();
	std::map<unsigned, int> mapChecksumIndex;  // checksum -> dataset index
	std::shared_ptr<Wt::WStandardItemModel> model = nullptr;
	WJPARAMPANEL *ppCommon = nullptr, *ppDiff = nullptr, *ppUnique = nullptr;
	std::string region = "", unit = "";
	std::vector<unsigned> vChecksum;
	unsigned xChecksum = 0;

	void addDataset(std::vector<std::vector<std::string>>& vvsData, std::vector<std::vector<std::string>>& vvsParameter);
	void addTipWheel(int layoutIndex);
	void configureChart();
	void display();
	void getParameterAll(std::vector<std::vector<double>>& seriesColour, std::vector<std::vector<std::vector<int>>>& panelColourIndex, std::vector<std::vector<std::vector<std::string>>>& panelText);
	std::vector<double> getMinMaxY();
	std::vector<std::vector<std::string>> getModelValues();
	int getNumSeries() { return vDataset.size(); }
	std::vector<Wt::WColor> getSeriesColour();
	std::string italicize(std::vector<std::string>& vsParameter, int italicFreq);
	std::unique_ptr<Wt::Chart::WCartesianChart> makeChart();
	std::unique_ptr<WJPARAMPANEL> makeWJPP(WJPARAMPANEL*& wjpp, std::string sTitle, std::vector<Wt::WColor>& seriesColour);
	void parameterPopulation();
	void parameterSorting();
	int removeDataset(int seriesIndex);
	void removeTipWheel(int layoutIndex);
	void reset();
	void setModelValues();
	Wt::Signal<std::string>& tipSignal() { return tipSignal_; }
	void widgetMobile(bool mobile);

	Wt::WLength wlAuto = Wt::WLength::Auto;
};

