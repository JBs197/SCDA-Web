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
#include <Wt/WJavaScript.h>
#include "SCDAserver.h"

using namespace std;
extern const string sroot;

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	string activeCata, activeTableColTitle, activeTableRowTitle;
	string db_path = sroot + "\\SCDA.db";
	bool first = 1;
	JFUNC jf;
	bool jsEnabled = 0;
	JTREE jtRegion, jtWidget;
	unordered_map<string, int> mapNumVar;  // sCata -> number of variables (excluding col/row)
	unordered_map<int, string> mapTimeWord;  // word index -> word  (shown in init).
	unordered_map<string, Wt::WString> mapTooltip;  // widget -> tooltip
	unordered_map<string, string> mapUnit;  // ambiguous unit -> definitive unit
	bool mobile = 0;
	const int num_filters = 3;
	int screenWidth, screenHeight;         // Unit of pixels, measured from the client. 
	Wt::WString selectedRegion, selectedFolder;
	string sessionID, tableRegion;
	vector<string> tableCol, tableRow;    // Headers.
	vector<vector<string>> tableCore;
	int tableWidth, boxTableWidth;        // Used by JS functions.
	vector<vector<string>> vvsDemographic;  // Form [forWhom index][forWhom, sCata0, sCata1, ...]
	vector<vector<string>> vvsParameter;  // Form [variable index][MID0, MID1, ..., variable title].
	Wt::WCssDecorationStyle wcssAttention, wcssDefault, wcssHighlighted;
	const Wt::WString wsAll = Wt::WString("All");
	const Wt::WString wsNoneSel = Wt::WString("[None selected]");

	WJCONFIG* wjConfig = nullptr;
	WJTABLE* tableData = nullptr;
	WTPAINT* wtMap = nullptr;

	Wt::WColor wcGrey, wcSelectedStrong, wcSelectedWeak, wcWhite;
	Wt::WContainerWidget *boxConfig, *boxData, *boxDownload, *boxMap, *boxMapAll;
	Wt::WContainerWidget *boxMapOption, *boxTable, *boxTextLegend;
	Wt::WVBoxLayout* layoutConfig;
	Wt::WLineEdit* leTest;
	Wt::WPopupMenu *popupUnit;
	Wt::WPushButton *pbDownloadPDF, *pbUnit;
	Wt::WSelectionBox* sbList;
	SCDAserver& sRef;
	Wt::WStackedWidget* stackedTabData;
	Wt::WTabWidget* tabData;
	Wt::WText *textTable, *textUnit;
	Wt::WTree *treeDialogCol, *treeDialogRow, *treeRegion;

	vector<Wt::WPushButton*> allPB;

	void cleanUnit(string& unit);
	void connect();
	void downloadMap();
	int getHeight();
	vector<string> getNextCBLayer(Wt::WComboBox*& wCB);
	Wt::WString getTextLegend(Wt::WPanel*& wPanel);
	string getUnit();
	int getWidth();
	void incomingFilterSignal();
	void incomingHeaderSignal(const int& iRow, const int& iCol);
	void incomingPullSignal(const int& pullType);
	void incomingResetSignal(const int& resetType);
	void init();
	void initMaps();
	void initUI();
	string jsMakeFunctionTableScrollTo(WJTABLE*& boxTable);
	string jsMakeFunctionTableWidth(WJTABLE*& boxTable, string tableID);
	unique_ptr<Wt::WContainerWidget> makeBoxData();
	unique_ptr<Wt::WContainerWidget> makeBoxDownload();
	unique_ptr<Wt::WContainerWidget> makeBoxMap();
	void mapAreaClicked(int areaIndex);
	void populateTextLegend(Wt::WContainerWidget*& boxTextLegend);
	void populateTree(JTREE& jt, Wt::WTreeNode*& node);
	void processDataEvent(const DataEvent& event);
	void processEventCatalogue(string sYear, string sCata);
	void processEventCategory(vector<string> vsCategory);
	void processEventDemographic(vector<vector<string>> vvsDemo);
	void processEventMap(vector<string> vsRegion, vector<vector<vector<double>>> vvvdArea, vector<vector<double>> vvdData);
	void processEventParameter(vector<vector<string>> vvsVariable, vector<vector<string>> vvsCata, vector<string> vsDIMIndex);
	void processEventTable(vector<vector<string>>& vvsTable, vector<string>& vsCol, vector<string>& vsRow, string& sRegion);
	void processEventTopic(vector<string> vsRowTopic, vector<string> vsColTopic);
	void processEventTree(JTREE jt);
	void resetMap();
	void resetTable();
	void resetTree();
	void setMap(int iRow, int iCol, string sRegion);
	void setTable(int geoCode, string sRegion);
	void toggleMobile();
	void treeClicked();
	void treeClicked(int& geoCode, string& sRegion);
	void updateTextCata(int numCata);
	void updateUnit(string sUnit);

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver), jsInfo(this, "jsInfo", 1), jsTWidth(this, "jsTWidth", 1)
	{ 
		this->setId("SCDAwidget");
		init();
	}

	void displayCata(const Wt::WKeyEvent& wKey);
	void tableReceiveDouble(const double& width);
	void tableReceiveString(const string& sInfo);

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

