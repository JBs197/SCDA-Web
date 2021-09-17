#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WComboBox.h>
#include <Wt/WPanel.h>
#include <Wt/WColor.h>
#include <Wt/WDialog.h>
#include "jtree.h"

using namespace std;

struct WJPANEL : public Wt::WPanel
{
	WJPANEL() : Wt::WPanel() { init(); }
	~WJPANEL() {}

	unordered_map<int, int> mapMIDIndent, mapTitleIndent;  // index -> indentation within hierarchy
	unordered_map<string, int> mapMIDIndex, mapTitleIndex;  // sValue -> index within CB 
	Wt::WComboBox* cbMID = nullptr;
	Wt::WComboBox* cbTitle = nullptr;
	Wt::WColor wcBorder, wcOffWhite, wcSelectedWeak, wcWhite;
	Wt::WBorder wbDefaultCB;
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
	JTREE jtCol, jtRow;
	unordered_map<string, int> mapDiffIndex, mapVarIndex;  // sID -> var/diff index
	mutex m_prompt;
	Wt::Signal<int, int> headerSignal_;
	Wt::Signal<> filterSignal_;
	Wt::Signal<int> pullSignal_, resetSignal_;
	string sFilterCol, sFilterRow;
	Wt::WTree *treeDialogCol = nullptr, *treeDialogRow = nullptr;
	vector<int> viFilterCol, viFilterRow;
	vector<string> vsPrompt;
	vector<vector<string>> vvsPrompt;
	Wt::WBorder wbDotted, wbSolid;
	Wt::WColor wcSelectedStrong, wcSelectedWeak, wcWhite;
	Wt::WCssDecorationStyle wcssAttention, wcssDefault, wcssHighlighted;
	unique_ptr<Wt::WDialog> wdFilter = nullptr;
	Wt::WVBoxLayout* vLayout = nullptr;

public:
	WJCONFIG(vector<string> vsYear) : Wt::WContainerWidget() {
		this->setId("wjConfig");
		init(vsYear);
	}
	~WJCONFIG() {}

	vector<WJPANEL*> basicWJP, diffWJP, varWJP;
	JFUNC jf;
	vector<vector<string>> vvsDemographic, vvsParameter;
	WJPANEL *wjpCategory, *wjpDemo, *wjpTopicCol, *wjpTopicRow, *wjpYear;
	Wt::WPushButton* pbMobile;
	Wt::WText* textCata;

	Wt::Signal<>& filterSignal() { return filterSignal_; }
	Wt::Signal<int, int>& headerSignal() { return headerSignal_; }
	Wt::Signal<int>& pullSignal() { return pullSignal_; }
	Wt::Signal<int>& resetSignal() { return resetSignal_; }

	void addDemographic(vector<vector<string>>& vvsDemo);
	void addDifferentiator(vector<string> vsDiff);
	void addVariable(vector<string>& vsVariable);
	void categoryChanged();
	void cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem);
	void cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem, string sActive);
	void demographicChanged();
	void dialogSubsectionFilterCol();
	void dialogSubsectionFilterColEnd();
	void dialogSubsectionFilterRow();
	void dialogSubsectionFilterRowEnd();
	void diffTitleChanged(string id);
	vector<string> getDemo();
	vector<vector<string>> getDemoSplit();
	vector<vector<string>> getDemoSplit(vector<string>& vsDemo);
	vector<vector<int>> getFilterTable();
	void getPrompt(vector<string>& vsP);
	void getPrompt(vector<string>& vsP, vector<vector<string>>& vvsP);
	vector<Wt::WString> getTextLegend();
	vector<vector<string>> getVariable();
	void init(vector<string> vsYear);
	void initJTree(vector<string>& vsRow, vector<string>& vsCol);
	unique_ptr<WJPANEL> makePanelCategory();
	unique_ptr<WJPANEL> makePanelTopicCol();
	unique_ptr<WJPANEL> makePanelTopicRow();
	unique_ptr<WJPANEL> makePanelYear(vector<string> vsYear);
	void populateTree(JTREE& jt, Wt::WTreeNode*& node);
	Wt::WTreeNode* populateTree(JTREE& jt, Wt::WTreeNode*& node, string sName);
	void removeDifferentiators();
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

