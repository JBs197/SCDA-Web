#pragma once
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WCssDecorationStyle.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WTree.h>
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
#include "jtime.h"
#include "jtree.h"

extern std::mutex m_config;

struct WJPANEL : public Wt::WPanel
{
	WJPANEL() : Wt::WPanel() { init(); }
	~WJPANEL() {}

	JTIME jtime;
	JTREE jtMID;
	int MIDchanged = -1;

	Wt::WContainerWidget *boxMID = nullptr;
	Wt::WComboBox *cbTitle = nullptr;
	std::unordered_map<int, int> mapFilterMID;  // JTREE index -> index within viFilter
	bool mobile = 0;
	std::unordered_map<std::string, int> mapIndexTitle;  // sTitle -> index within cbTitle 
	Wt::WPushButton *pbMIDclosed = nullptr, *pbMIDopened = nullptr, *pbMID;
	std::string selMID = "";
	Wt::WStackedWidget *stackedPB = nullptr;
	Wt::WText *textMID = nullptr;
	Wt::WTree* treeDialog = nullptr;
	Wt::WTreeNode* treeNodeSel = nullptr;
	std::vector<std::string> vsTitle, vsMID;
	Wt::WColor wcBorder, wcGrey, wcOffWhite, wcSelectedWeak, wcWhite;
	Wt::WBorder wbDefaultCB, wbThick;
	std::unique_ptr<Wt::WDialog> wdTree = nullptr;
	Wt::WLength wlAuto = Wt::WLength::Auto;

	void appendSelf(std::vector<std::string>& vsDIMtitle, std::vector<int>& viMID);
	void clear();
	void closeDialogs();
	void dialogMID();
	void dialogMIDEnd();
	void dialogMIDToggle(int stackedIndex);
	Wt::Signal<std::string>& dialogOpenSignal() { return dialogOpenSignal_; }
	Wt::Signal<>& filterSignal() { return filterSignal_; }
	int getIndexMID();
	std::vector<std::string> getTextLegend();
	void highlight(int widgetIndex);
	void init();
	void initStackedPB(Wt::WLink wlClosed, Wt::WLink wlOpened);
	void panelClicked() { jtime.timerStart(); }
	void populateTree(JTREE& jt, int parentID, Wt::WTreeNode*& parentNode, int selectedID = -1);
	void resetMapIndex();
	void setCB(std::vector<std::string>& vsList);
	void setCB(std::vector<std::string>& vsList, int iActive);
	void setCB(std::vector<std::string>& vsList, std::string sActive);
	void setIndexMID(int indexMID);
	void setTextMID(std::string sMID);
	void setTree(std::vector<std::string>& vsList);
	void setTree(std::vector<std::vector<std::string>>& vvsMID);
	void toggleMobile(bool mobile);
	Wt::Signal<>& topicSignal() { return topicSignal_; }
	void unhighlight(int widgetIndex);
	Wt::Signal<>& varSignal() { return varSignal_; }

private:
	Wt::Signal<std::string> dialogOpenSignal_;
	Wt::Signal<> filterSignal_;
	Wt::Signal<> topicSignal_;
	Wt::Signal<> varSignal_;

	void err(std::string message);
};

class WJCONFIG : public Wt::WContainerWidget
{
	Wt::Signal<int, int> headerSignal_;
	JTIME jtime;
	std::unordered_map<std::string, int> mapDiffIndex, mapVarIndex;  // sID -> var/diff index
	Wt::Signal<int> pullSignal_, resetSignal_;
	std::string sPrompt;
	std::vector<int> viMID;
	Wt::WVBoxLayout* vLayout = nullptr;
	std::vector<std::string> vsPrompt, vsDIMtitle;
	std::vector<std::vector<std::string>> vvsCata, vvsPrompt;
	Wt::WBorder wbDefaultCB, wbDotted, wbSolid;
	Wt::WColor wcBlack, wcBorder, wcOffWhite, wcSelectedStrong, wcSelectedWeak, wcWhite;
	Wt::WCssDecorationStyle wcssAttention, wcssDefault, wcssHighlighted;
	std::unique_ptr<Wt::WDialog> wdFilter = nullptr;

	void err(std::string message);

public:
	WJCONFIG(std::vector<std::string> vsYear) : Wt::WContainerWidget() {
		this->setId("wjConfig");
		init(vsYear);
	}
	~WJCONFIG() {}

	std::vector<WJPANEL*> basicWJP, varWJP;
	std::vector<Wt::WPanel*> diffWP;
	Wt::WLink linkIconChevronDown = Wt::WLink(), linkIconChevronRight = Wt::WLink();
	std::vector<std::vector<std::string>> vvsDemographic;
	std::vector<std::vector<std::vector<std::string>>> vvvsParameter;
	WJPANEL *wjpCategory, *wjpDemo, *wjpTopicCol, *wjpTopicRow, *wjpYear;
	Wt::WText* textCata;

	Wt::Signal<int, int>& headerSignal() { return headerSignal_; }
	Wt::Signal<int>& pullSignal() { return pullSignal_; }
	Wt::Signal<int>& resetSignal() { return resetSignal_; }

	void addDemographic(std::vector<std::vector<std::string>>& vvsDemo);
	void addDifferentiator(std::vector<std::string> vsDiff);
	void addDifferentiator(std::vector<std::string> vsDiff, std::string sTitle);
	void addVariable(std::string& sTitle, std::vector<std::vector<std::string>>& vvsMID);
	void categoryChanged();
	void cbRenew(Wt::WComboBox*& cb, std::vector<std::string>& vsItem);
	void cbRenew(Wt::WComboBox*& cb, std::vector<std::string>& vsItem, std::string sActive);
	void demographicChanged();
	void dialogHighlander(const std::string& sObjectName);
	void diffChanged(std::string id);
	std::vector<std::string> getDemo();
	std::vector<std::vector<std::string>> getDemoSplit();
	std::vector<std::vector<std::string>> getDemoSplit(std::vector<std::string>& vsDemo);
	void getPrompt(std::vector<std::string>& vsP);
	void getPrompt(std::vector<std::string>& vsP, std::vector<std::vector<std::string>>& vvsP);
	void getPrompt(std::string& sP, std::vector<std::vector<std::string>>& vvsP1, std::vector<std::vector<std::string>>& vvsP2);
	void getPrompt(std::vector<std::string>& vsP1, std::vector<std::string>& vsP2, std::vector<int>& viP);
	std::vector<std::vector<std::string>> getTextLegend();
	std::vector<std::vector<std::string>> getTextLegend(std::vector<int>& viChanged);
	std::vector<std::vector<std::string>> getVariable();
	void getVariable(std::vector<std::string>& vsDIMtitle, std::vector<int>& viMID);
	void highlightPanel(Wt::WPanel*& wPanel, int widgetIndex);
	void init(std::vector<std::string> vsYear);
	std::unique_ptr<WJPANEL> makePanelCategory();
	std::unique_ptr<WJPANEL> makePanelTopicCol();
	std::unique_ptr<WJPANEL> makePanelTopicRow();
	std::unique_ptr<WJPANEL> makePanelVariable();
	std::unique_ptr<WJPANEL> makePanelYear(std::vector<std::string> vsYear);
	void removeDifferentiators();
	void removeVariable(int varIndex);
	void resetTopicMID();
	void resetVariables();
	void resetVariables(int plus);
	void setPrompt(std::vector<std::string>& vsP);
	void setPrompt(std::vector<std::string>& vsP, std::vector<std::vector<std::string>>& vvsP);
	void setPrompt(std::string& sP, std::vector<std::vector<std::string>>& vvsC, std::vector<std::vector<std::string>>& vvsP);
	void setPrompt(std::vector<std::string>& vsP1, std::vector<std::string>& vsP2, std::vector<int>& viP);
	void toggleMobilePanel(Wt::WPanel*& wPanel, bool mobile);
	void topicSelChanged();
	void topicSelClicked();
	void topicTitleChanged(std::string id);
	void unhighlightPanel(Wt::WPanel*& wPanel, int widgetIndex);
	void varMIDChanged();
	void varMIDClicked();
	void yearChanged() { pullSignal_.emit(0); }

	Wt::WLength wlAuto = Wt::WLength();
	Wt::WString wsNoneSel = "[None selected]";
};

