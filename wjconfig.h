#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WComboBox.h>
#include <Wt/WPanel.h>
#include <Wt/WColor.h>
#include "jtree.h"

using namespace std;

struct WJPANEL : public Wt::WPanel
{
	WJPANEL() : Wt::WPanel() { init(); }
	~WJPANEL() {}

	unordered_map<string, int> mapMIDIndex, mapTitleIndex;  // sValue -> index within CB 
	Wt::WComboBox* cbMID = nullptr;
	Wt::WComboBox* cbTitle = nullptr;
	Wt::WColor wcSelectedWeak, wcWhite;
	Wt::WPushButton* pbDialog = nullptr;

	void clear();
	void highlight(int widgetIndex);
	void init();
	void unhighlight(int widgetIndex);
	void setCB(int cbIndex, vector<string>& vsList);
	void setCB(int cbIndex, vector<string>& vsList, int iActive);
	void setCB(int cbIndex, vector<string>& vsList, string sActive);

};

class WJCONFIG : public Wt::WContainerWidget
{
	unordered_map<string, int> mapVarIndex;  // unique varPanel id -> var index
	mutex m_prompt;
	Wt::Signal<int> pullSignal_, resetSignal_;
	Wt::Signal<int, int> tableSignal_;
	vector<string> vsPrompt;
	vector<vector<string>> vvsPrompt;
	Wt::WBorder wbDotted, wbSolid;
	Wt::WColor wcSelectedStrong, wcSelectedWeak, wcWhite;
	Wt::WCssDecorationStyle wcssAttention, wcssDefault, wcssHighlighted;
	Wt::WVBoxLayout* vLayout = nullptr;

public:
	WJCONFIG(vector<string> vsYear) : Wt::WContainerWidget() {
		this->setId("wjConfig");
		init(vsYear);
	}
	~WJCONFIG() {}

	vector<WJPANEL*> allWJP, varWJP;
	JFUNC jf;
	vector<vector<string>> vvsDemographic, vvsParameter;
	WJPANEL *wjpCategory, *wjpDemo, *wjpTopicCol, *wjpTopicRow, *wjpYear;
	Wt::WPushButton* pbMobile;
	Wt::WText* textCata;

	Wt::Signal<int>& pullSignal() { return pullSignal_; }
	Wt::Signal<int>& resetSignal() { return resetSignal_; }
	Wt::Signal<int, int>& tableSignal() { return tableSignal_; }

	void addDemographic(vector<vector<string>>& vvsDemo);
	void addDifferentiator(vector<string> vsDiff);
	void addVariable(vector<string>& vsVariable);
	void categoryChanged();
	void cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem);
	void cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem, string sActive);
	void demographicChanged();
	void diffTitleChanged(string id);
	vector<string> getDemo();
	vector<vector<string>> getDemoSplit();
	vector<vector<string>> getDemoSplit(vector<string>& vsDemo);
	void getPrompt(vector<string>& vsP);
	void getPrompt(vector<string>& vsP, vector<vector<string>>& vvsP);
	vector<Wt::WString> getTextLegend();
	vector<vector<string>> getVariable();
	void init(vector<string> vsYear);
	unique_ptr<Wt::WPanel> makePanelCategory();
	unique_ptr<Wt::WPanel> makePanelTopicCol();
	unique_ptr<Wt::WPanel> makePanelTopicRow();
	unique_ptr<Wt::WPanel> makePanelYear(vector<string> vsYear);
	void removeVariable(int varIndex);
	void resetTopicSel();
	void resetVariables();
	void resetVariables(int plus);
	void setPrompt(vector<string>& vsP);
	void setPrompt(vector<string>& vsP, vector<vector<string>>& vvsP);
	void topicSelChanged();
	void topicSelClicked();
	void topicTitleChanged(string id);
	void varMIDChanged();
	void varMIDClicked();
	void varTitleChanged(string id);
	void yearChanged() { pullSignal_.emit(0); }

	Wt::WLength wlAuto = Wt::WLength();
	Wt::WString wsNoneSel = "[None selected]";
};

