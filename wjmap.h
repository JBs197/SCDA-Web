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

	vector<string> vsParameter;

	void display(bool italicize);
	void initColour();
	string italicizeOdd(string& sParameter);
	void setColour(vector<int>& viChanged);
};

class WJMAP : public Wt::WContainerWidget
{
	bool mobile = 0;

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

	void build();
	int getLegendBarDouble(vector<string>& vsRegion, string sUnit, int displayDataSize);
	int getLegendTickLines(string sUnit);
	void init();
	void resetMenu();
	void updateUnit(string sUnit);
	void widgetMobile(bool mobile);
};

