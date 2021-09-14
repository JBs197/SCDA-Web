#pragma once
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WSpinBox.h>
#include <Wt/WSlider.h>
#include <Wt/WPanel.h>
#include <Wt/WPushButton.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLength.h>
#include <Wt/WTabWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WMenuItem.h>
#include <Wt/WImage.h>
#include <Wt/WEvent.h>
#include <Wt/WDialog.h>
#include <Wt/WJavaScript.h>
#include "SCDAserver.h"

using namespace std;
extern const string sroot;

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	string activeCata, activeCategory, activeColTopic, activeRowTopic, activeYear;
	string activeTableColTitle, activeTableRowTitle;
	string db_path = sroot + "\\SCDA.db";
	bool first = 1;
	JFUNC jf;
	bool jsEnabled = 0;
	JTREE jtRegion;
	unordered_map<string, int> mapNumVar;  // sCata -> number of variables (excluding col/row)
	unordered_map<int, string> mapTimeWord;  // word index -> word  (shown in init).
	unordered_map<string, Wt::WString> mapTooltip;  // widget -> tooltip
	unordered_map<string, string> mapUnit;  // ambiguous unit -> definitive unit
	bool mobile = 0;
	const int num_filters = 3;
	int numPreVariable = -1;  // Number of widgets in boxConfig prior to the "variable" panels.
	Wt::WString selectedRegion, selectedFolder;
	string sessionID, mapRegion;
	int tableWidth, boxTableWidth;
	vector<vector<string>> vvsDemographic;  // Form [forWhom index][forWhom, sCata0, sCata1, ...]
	vector<vector<string>> vvsParameter;  // Form [variable index][MID0, MID1, ..., variable title].
	Wt::WCssDecorationStyle wcssAttention, wcssDefault, wcssHighlighted;
	const Wt::WString wsAll = Wt::WString("All");
	const Wt::WString wsNoneSel = Wt::WString("[None selected]");

	WJCONFIG* wjConfig;
	WJTABLE* tableData;
	WTPAINT* wtMap;

	Wt::WColor colourSelectedStrong, colourSelectedWeak, colourWhite;
	Wt::WComboBox *cbColTopicTitle, *cbColTopicTable;
	Wt::WComboBox *cbColTopicSel, *cbRowTopicSel;
	Wt::WComboBox *cbRowTopicTitle, *cbRowTopicTable;
	Wt::WContainerWidget *boxConfig, *boxData, *boxDownload, *boxMap, *boxMapAll, *boxMapOption;
	Wt::WContainerWidget *boxTable, *boxTextLegend;
	Wt::WDialog *wdColFilter, *wdRowFilter;
	Wt::WImage *imgMap;
	Wt::WVBoxLayout* layoutConfig;
	Wt::WLineEdit* leTest;
	Wt::WPanel *panelCategory, *panelDemographic, *panelTopicCol, *panelTopicRow, *panelYear;
	Wt::WPopupMenu *popupUnit;
	Wt::WPushButton *pbSubsectionFilterCol, *pbSubsectionFilterRow, *pbDownloadPDF, *pbMobile, *pbUnit;
	Wt::WSelectionBox* sbList;
	SCDAserver& sRef;
	Wt::WStackedWidget* stackedTabData;
	Wt::WTabWidget* tabData;
	Wt::WText *textCata, *textTable, *textUnit;
	Wt::WTree *treeDialogCol, *treeDialogRow, *treeRegion;

	vector<WJCOMBO*> allCBJ;
	vector<Wt::WPanel*> allPanel, varPanel;
	vector<Wt::WPushButton*> allPB;

	void addVariable(vector<string>& vsVariable);
	void cbCategoryChanged();
	void cbColRowSelChanged();
	void cbColRowSelClicked();
	void cbColRowTitleClicked(string id);
	void cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem);
	void cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem);
	void cbRenew(Wt::WComboBox*& cb, string sTop, vector<vector<string>>& vvsItem);
	void cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem, string selItem);
	void cbVarTitleChanged(string id);
	void cbVarMIDChanged();
	void cbVarMIDClicked();
	void cleanUnit(string& unit);
	void connect();
	void dialogSubsectionFilterCol();
	void dialogSubsectionFilterColEnd();
	void dialogSubsectionFilterRow();
	void dialogSubsectionFilterRowEnd();
	void downloadMap();
	vector<string> getDemo();
	int getHeight();
	vector<string> getNextCBLayer(Wt::WComboBox*& wCB);
	Wt::WString getTextLegend(Wt::WPanel*& wPanel);
	string getUnit();
	vector<vector<string>> getVariable();
	int getWidth();
	void incomingPullSignal(const int& pullType);
	void incomingResetSignal(const int& resetType);
	void init();
	void initUI();
	string jsMakeFunctionTableScrollTo(WJTABLE*& boxTable);
	string jsMakeFunctionTableWidth(WJTABLE*& boxTable, string tableID);
	unique_ptr<Wt::WContainerWidget> makeBoxConfig(Wt::WContainerWidget*& wBox);
	unique_ptr<Wt::WContainerWidget> makeBoxData(Wt::WContainerWidget*& wBox);
	unique_ptr<WJTABLE> makeTableData(WJTABLE*& tableData);
	unique_ptr<Wt::WTree> makeTreeRegion(Wt::WTree*& wTree);
	void makeUI();
	void mapAreaClicked(int areaIndex);
	void mapAreaDoubleClicked(int areaIndex);
	void populateTextLegend(Wt::WContainerWidget*& boxTextLegend);
	void populateTree(JTREE& jt, Wt::WTreeNode*& node);
	void processDataEvent(const DataEvent& event);
	void processEventCatalogue(string sYear, string sCata);
	void processEventCategory(vector<string> vsCategory);
	void processEventDemographic(vector<vector<string>> vvsDemo);
	void processEventMap(vector<string> vsRegion, vector<vector<vector<double>>> vvvdArea, vector<vector<double>> vvdData);
	void processEventParameter(vector<vector<string>> vvsVariable, vector<vector<string>> vvsCata, vector<string> vsDIMIndex);
	void processEventTable(vector<vector<string>> vvsTable, vector<string> vsCol, vector<string> vsRow);
	void processEventTopic(vector<string> vsRowTopic, vector<string> vsColTopic);
	void processEventTree(JTREE jt);
	void removeVariable(int varIndex);
	void resetMap();
	void resetTable();
	void resetTopicSel();
	void resetTree();
	void resetVariables();
	void resetVariables(int plus);
	void setMap(int iRow, int iCol, string sRegion);
	void setTable(int geoCode, string sRegion);
	void tableCBUpdate(int iRow, int iCol);
	void toggleMobile();
	void treeClicked();
	void updateTextCata(int numCata);
	void updateUnit(string sUnit);
	void widgetMobile();

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver), jsInfo(this, "jsInfo", 1), jsTWidth(this, "jsTWidth", 1)
	{ 
		this->setId("SCDAwidget");
		init();
	}

	void displayCata(const Wt::WKeyEvent& wKey);
	void tableReceiveDouble(const double& width);
	void tableReceiveString(const string& sInfo);
	void virtualClick(const int& vcType);

	Wt::JSignal<string> jsInfo;
	Wt::JSignal<double> jsTWidth;
	
	Wt::WLength wlAuto = Wt::WLength::Auto;
	Wt::WLength len5p = Wt::WLength("5px");
	Wt::WLength len50p = Wt::WLength("50px");
	Wt::WLength len150p = Wt::WLength("150px");
	Wt::WLength len200p = Wt::WLength("200px");
	Wt::WLength len250p = Wt::WLength("250px");
	Wt::WLength len300p = Wt::WLength("300px");
	Wt::WLength len600p = Wt::WLength("600px");
	Wt::WLength len700p = Wt::WLength("700px");
	Wt::WLength len780p = Wt::WLength("780px");
	Wt::WLength len800p = Wt::WLength("800px");
};

