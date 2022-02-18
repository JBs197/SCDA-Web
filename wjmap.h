#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "jstring.h"

class WJLEGEND : public Wt::WContainerWidget
{
	std::vector<std::vector<int>> colourList;
	std::vector<int> vColour;

	void err(std::string message);

public:
	WJLEGEND() { initColour(); }
	~WJLEGEND() {}

	std::vector<std::vector<std::string>> vvsParameter;

	void display(bool oddItalics);
	void initColour();
	std::string italicize(std::vector<std::string>& vsParameter, int italicFreq);
	std::string italicizeOdd(std::string& sParameter);
	void setColour(std::vector<int>& viChanged);
};

class WJMAP : public Wt::WContainerWidget
{
	std::unordered_map<std::string, Wt::WString> mapTooltip;  // sPrompt -> wsTooltip
	bool mobile = 0;
	Wt::Signal<std::string> tipSignal_;
	Wt::WColor wcSelectedWeak, wcWhite;

	void err(std::string message);
	void init();
	void initColour();
	void initMaps();

public:
	WJMAP() { init(); }
	~WJMAP() {}

	Wt::WContainerWidget *boxMap, *boxOption;
	Wt::WPushButton *pbPin, *pbPinReset, *pbUnit;
	Wt::WPopupMenu* popupUnit;
	Wt::WText* textUnit;
	WJLEGEND* wjlMap;
	Wt::WLength wlAuto;
	Wt::WLink linkIconClose = Wt::WLink();

	void addTipPin(int layoutIndex);
	void build();
	int getLegendBarDouble(std::vector<std::string>& vsRegion, std::string sUnit, int displayDataSize);
	int getLegendTickLines(std::string sUnit);
	std::unique_ptr<Wt::WContainerWidget> makeUnitPinBox();
	void removeTipPin(int layoutIndex);
	void resetMenu();
	Wt::Signal<std::string>& tipSignal() { return tipSignal_; }
	void updatePinButtons(std::vector<std::string> vsTooltip);
	void widgetMobile(bool mobile);
};

