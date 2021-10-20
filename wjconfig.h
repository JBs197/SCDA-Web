#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WPushButton.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WComboBox.h>
#include <Wt/WPanel.h>
#include <Wt/WColor.h>
#include <Wt/WDialog.h>
#include <Wt/WPainter.h>
#include <Wt/WSvgImage.h>
#include "jtree.h"

using namespace std;
extern mutex m_config;

struct WJPANEL : public Wt::WPanel
{
	WJPANEL() : Wt::WPanel() { init(); }
	~WJPANEL() {}

	JFUNC jf;
	JTREE jtMID;
	int MIDchanged = -1;

	Wt::WContainerWidget *boxMID = nullptr;
	Wt::WComboBox *cbTitle = nullptr;
	unordered_map<int, int> mapFilterMID;  // JTREE index -> index within viFilter
	unordered_map<string, int> mapIndexTitle;  // sTitle -> index within cbTitle 
	Wt::WPushButton *pbMIDclosed = nullptr, *pbMIDopened = nullptr, *pbMID;
	string selMID = "";
	Wt::WStackedWidget *stackedPB = nullptr;
	Wt::WText *textMID = nullptr;
	Wt::WTree *treeDialog = nullptr;
	Wt::WTreeNode *treeNodeSel = nullptr;
	vector<int> viFilter;                   // JTREE indices of permitted items.
	vector<string> vsTitle, vsMID;
	Wt::WColor wcBorder, wcGrey, wcOffWhite, wcSelectedWeak, wcWhite;
	Wt::WBorder wbDefaultCB, wbThick;
	unique_ptr<Wt::WDialog> wdTree = nullptr;
	Wt::WLength wlAuto = Wt::WLength::Auto;

	void appendSelf(vector<string>& vsDIMtitle, vector<int>& viMID);
	void clear();
	void closeDialogs();
	void dialogMID();
	void dialogMIDEnd();
	void dialogMIDToggle(int stackedIndex);
	Wt::Signal<string>& dialogOpenSignal() { return dialogOpenSignal_; }
	Wt::Signal<>& filterSignal() { return filterSignal_; }
	int getIndexMID(int mode);
	vector<string> getTextLegend();
	void highlight(int widgetIndex);
	void init();
	void initStackedPB(Wt::WLink wlClosed, Wt::WLink wlOpened);
	void panelClicked() { jf.timerStart(); }
	void populateTree(JTREE& jt, Wt::WTreeNode*& node);
	void populateTree(JTREE& jt, Wt::WTreeNode*& node, int selTreeIndex);
	void resetMapIndex();
	void setCB(vector<string>& vsList);
	void setCB(vector<string>& vsList, int iActive);
	void setCB(vector<string>& vsList, string sActive);
	void setFilter(vector<int>& viFilter);
	void setIndexMID(int indexMID);
	void setTextMID(string sMID);
	void setTree(vector<string>& vsList);
	void setTree(vector<vector<string>>& vvsMID);
	void toggleMobile(bool mobile);
	Wt::Signal<>& topicSignal() { return topicSignal_; }
	void unhighlight(int widgetIndex);
	Wt::Signal<>& varSignal() { return varSignal_; }

private:
	Wt::Signal<string> dialogOpenSignal_;
	Wt::Signal<> filterSignal_;
	Wt::Signal<> topicSignal_;
	Wt::Signal<> varSignal_;
};

class WJCONFIG : public Wt::WContainerWidget
{
	Wt::Signal<int, int> headerSignal_;
	unordered_map<string, int> mapDiffIndex, mapVarIndex;  // sID -> var/diff index
	bool mobile = 0;
	Wt::Signal<int> pullSignal_, resetSignal_;
	string sPrompt;
	vector<int> viMID;
	Wt::WVBoxLayout* vLayout = nullptr;
	vector<string> vsPrompt, vsDIMtitle;
	vector<vector<string>> vvsCata, vvsPrompt;
	Wt::WBorder wbDefaultCB, wbDotted, wbSolid;
	Wt::WColor wcBlack, wcBorder, wcOffWhite, wcSelectedStrong, wcSelectedWeak, wcWhite;
	Wt::WCssDecorationStyle wcssAttention, wcssDefault, wcssHighlighted;
	unique_ptr<Wt::WDialog> wdFilter = nullptr;

public:
	WJCONFIG(vector<string> vsYear) : Wt::WContainerWidget() {
		this->setId("wjConfig");
		init(vsYear);
	}
	~WJCONFIG() {}

	vector<WJPANEL*> basicWJP, varWJP;
	vector<Wt::WPanel*> diffWP;
	JFUNC jf;
	Wt::WLink linkIconChevronDown = Wt::WLink(), linkIconChevronRight = Wt::WLink();
	vector<vector<string>> vvsDemographic;
	vector<vector<vector<string>>> vvvsParameter;
	WJPANEL *wjpCategory, *wjpDemo, *wjpTopicCol, *wjpTopicRow, *wjpYear;
	Wt::WPushButton* pbMobile;
	Wt::WText* textCata;

	Wt::Signal<int, int>& headerSignal() { return headerSignal_; }
	Wt::Signal<int>& pullSignal() { return pullSignal_; }
	Wt::Signal<int>& resetSignal() { return resetSignal_; }

	void addDemographic(vector<vector<string>>& vvsDemo);
	void addDifferentiator(vector<string> vsDiff);
	void addDifferentiator(vector<string> vsDiff, string sTitle);
	void addVariable(string& sTitle, vector<vector<string>>& vvsMID);
	void categoryChanged();
	void cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem);
	void cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem, string sActive);
	void demographicChanged();
	void dialogHighlander(const string& sObjectName);
	void diffChanged(string id);
	vector<string> getDemo();
	vector<vector<string>> getDemoSplit();
	vector<vector<string>> getDemoSplit(vector<string>& vsDemo);
	vector<vector<int>> getFilterTable();
	void getPrompt(vector<string>& vsP);
	void getPrompt(vector<string>& vsP, vector<vector<string>>& vvsP);
	void getPrompt(string& sP, vector<vector<string>>& vvsP1, vector<vector<string>>& vvsP2);
	void getPrompt(vector<string>& vsP1, vector<string>& vsP2, vector<int>& viP);
	vector<vector<string>> getTextLegend();
	vector<vector<string>> getTextLegend(vector<int>& viChanged);
	vector<vector<string>> getVariable();
	void getVariable(vector<string>& vsDIMtitle, vector<int>& viMID);
	void highlightPanel(Wt::WPanel*& wPanel, int widgetIndex);
	void init(vector<string> vsYear);
	unique_ptr<WJPANEL> makePanelCategory();
	unique_ptr<WJPANEL> makePanelTopicCol();
	unique_ptr<WJPANEL> makePanelTopicRow();
	unique_ptr<WJPANEL> makePanelVariable();
	unique_ptr<WJPANEL> makePanelYear(vector<string> vsYear);
	void removeDifferentiators();
	void removeVariable(int varIndex);
	void resetTopicMID();
	void resetVariables();
	void resetVariables(int plus);
	void setMobile(bool goMobile);
	void setPrompt(vector<string>& vsP);
	void setPrompt(vector<string>& vsP, vector<vector<string>>& vvsP);
	void setPrompt(string& sP, vector<vector<string>>& vvsC, vector<vector<string>>& vvsP);
	void setPrompt(vector<string>& vsP1, vector<string>& vsP2, vector<int>& viP);
	void toggleMobilePanel(Wt::WPanel*& wPanel, bool mobile);
	void topicSelChanged();
	void topicSelClicked();
	void topicTitleChanged(string id);
	void unhighlightPanel(Wt::WPanel*& wPanel, int widgetIndex);
	void varMIDChanged();
	void varMIDClicked();
	void widgetMobile(bool mobile);
	void yearChanged() { pullSignal_.emit(0); }

	Wt::WLength wlAuto = Wt::WLength();
	Wt::WString wsNoneSel = "[None selected]";
};

