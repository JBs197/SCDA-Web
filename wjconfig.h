#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WComboBox.h>
#include <Wt/WPanel.h>
#include <Wt/WColor.h>
#include "jfunc.h"

using namespace std;

struct WJCOMBO : public Wt::WContainerWidget
{
	WJCOMBO() : Wt::WContainerWidget() { init(); }
	~WJCOMBO() {}

	unordered_map<string, int> mapMIDIndex, mapTitleIndex;  // sValue -> CB index
	Wt::WComboBox* wcbMID = nullptr;
	Wt::WComboBox* wcbTitle = nullptr;
	Wt::WPushButton* wpbDialog = nullptr;

	void highlight(Wt::WCssDecorationStyle& wcssTitle, Wt::WCssDecorationStyle& wcssMID);
	void init();
	void setMID(vector<string>& vsList);
	void setMID(vector<string>& vsList, string sActive);
	void setTitle(vector<string>& vsList);
	void setTitle(vector<string>& vsList, string sActive);

};

class WJCONFIG : public Wt::WContainerWidget
{
	JFUNC jf;
	unordered_map<string, int> mapVarIndex;  // unique varPanel id -> var index
	Wt::Signal<int> pullSignal_, resetSignal_;
	Wt::WBorder wbDotted, wbSolid;
	Wt::WColor wcSelectedStrong, wcSelectedWeak, wcWhite;
	Wt::WCssDecorationStyle wcssAttention, wcssDefault, wcssHighlighted;
	Wt::WPanel *wpCategory, *wpDemo, *wpTopicCol, *wpTopicRow, *wpYear;
	Wt::WVBoxLayout* vLayout = nullptr;

	vector<Wt::WPanel*> allPanel, varPanel;
	vector<WJCOMBO*> allWJC, varWJC;

public:
	WJCONFIG(vector<string> vsYear) : Wt::WContainerWidget() {
		this->setId("wjConfig");
		init(vsYear);
	}
	~WJCONFIG() {}

	WJCOMBO *wjcCategory, *wjcDemo, *wjcTopicCol, *wjcTopicRow, *wjcYear;

	Wt::Signal<int>& pullSignal() { return pullSignal_; }
	Wt::Signal<int>& resetSignal() { return resetSignal_; }

	void addDemographic(vector<vector<string>>& vvsDemo);
	void addDifferentiator(vector<string> vsDiff);
	void demographicChanged();
	void diffTitleChanged(string id);
	void highlight(Wt::WPanel*& wpHL, bool HL);
	void init(vector<string> vsYear);
	unique_ptr<Wt::WPanel> makePanelCategory();
	unique_ptr<Wt::WPanel> makePanelTopicCol();
	unique_ptr<Wt::WPanel> makePanelTopicRow();
	unique_ptr<Wt::WPanel> makePanelYear(vector<string> vsYear);
	void yearChanged() { pullSignal_.emit(0); }

	Wt::WLength wlAuto = Wt::WLength();

};

