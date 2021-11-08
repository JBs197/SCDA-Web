#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "jfunc.h"

using namespace std;

class WJLEGEND : public Wt::WContainerWidget
{
	vector<vector<int>> colourList;
	JFUNC jf;
	vector<int> vColour;

public:
	WJLEGEND() { initColour(); }
	~WJLEGEND() {}

	vector<vector<string>> vvsParameter;

	void display(bool oddItalics);
	void initColour();
	string italicize(vector<string>& vsParameter, int italicFreq);
	string italicizeOdd(string& sParameter);
	void setColour(vector<int>& viChanged);
};

class WJMAP : public Wt::WContainerWidget
{
	unordered_map<string, Wt::WString> mapTooltip;  // sPrompt -> wsTooltip
	bool mobile = 0;
	Wt::Signal<string> tipSignal_;
	Wt::WColor wcSelectedWeak, wcWhite;

	void init();
	void initColour();
	void initMaps();

public:
	WJMAP() { init(); }
	~WJMAP() {}

	Wt::WContainerWidget *boxMap, *boxOption;
	JFUNC jf;
	Wt::WPushButton *pbPin, *pbPinReset, *pbUnit;
	Wt::WPopupMenu* popupUnit;
	Wt::WText* textUnit;
	WJLEGEND* wjlMap;
	Wt::WLength wlAuto;
	Wt::WLink linkIconClose = Wt::WLink();

	void addTipPin(int layoutIndex);
	void build();
	int getLegendBarDouble(vector<string>& vsRegion, string sUnit, int displayDataSize);
	int getLegendTickLines(string sUnit);
	unique_ptr<Wt::WContainerWidget> makeUnitPinBox();
	void removeTipPin(int layoutIndex);
	void resetMenu();
	Wt::Signal<string>& tipSignal() { return tipSignal_; }
	void updatePinButtons(vector<string> vsTooltip);
	void widgetMobile(bool mobile);
};

