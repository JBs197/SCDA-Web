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
#include <Wt/WToolBar.h>
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
	bool filtersEnabled = 0;
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
	unordered_set<string> setTip;         // List of helpful tips which have been dismissed by the user.
	vector<string> tableCol, tableRow;    // Headers.
	vector<vector<string>> tableCore;
	int tableWidth, boxTableWidth;        // Used by JS functions.
	vector<vector<string>> vvsDemographic;  // Form [forWhom index][forWhom, sCata0, sCata1, ...]
	vector<vector<string>> vvsParameter;  // Form [variable index][MID0, MID1, ..., variable title].
	Wt::WCssDecorationStyle wcssAttention, wcssDefault, wcssHighlighted;
	const Wt::WString wsAll = Wt::WString("All");
	const Wt::WString wsNoneSel = Wt::WString("[None selected]");

	WJBARGRAPH* wjBarGraph = nullptr;
	WJCONFIG* wjConfig = nullptr;
	WJDOWNLOAD* wjDownload = nullptr;
	WJTABLE* tableData = nullptr;
	WTPAINT* wtMap = nullptr;

	Wt::WColor wcGrey, wcSelectedStrong, wcSelectedWeak, wcWhite;
	Wt::WContainerWidget *boxBarGraph, *boxData, *boxDownload, *boxMap;
	Wt::WContainerWidget *boxMapAll, *boxMapOption, *boxTable, *boxTextLegend;
	Wt::WVBoxLayout* layoutConfig;
	Wt::WLineEdit* leTest;
	Wt::WPopupMenu *popupUnit;
	Wt::WPushButton *pbDownloadPDF, *pbFilterCol, *pbFilterRow, *pbPin, *pbPinReset, *pbUnit;
	Wt::WSelectionBox* sbList;
	SCDAserver& sRef;
	Wt::WStackedWidget* stackedTabData;
	Wt::WTabWidget* tabData;
	Wt::WText *textTable, *textUnit;
	Wt::WTree *treeDialog, *treeRegion;
	Wt::WTreeNode *treeNodeSel;

	vector<Wt::WPushButton*> allPB;

	void cleanUnit(string& unit);
	void connect();
	void downloadMap();
	int getHeight();
	vector<string> getMapParameterList();
	string getUnit();
	int getWidth();
	void incomingFilterSignal();
	void incomingHeaderSignal(const int& iRow, const int& iCol);
	void incomingPullSignal(const int& pullType);
	void incomingResetSignal(const int& resetType);
	void incomingVarSignal();
	void init();
	void initMaps();
	void initUI();
	string jsMakeFunctionTableScrollTo(WJTABLE*& boxTable);
	string jsMakeFunctionTableWidth(WJTABLE*& boxTable, string tableID);
	unique_ptr<Wt::WContainerWidget> makeBoxData();
	unique_ptr<Wt::WContainerWidget> makeBoxMap();
	unique_ptr<Wt::WContainerWidget> makeBoxTable();
	void mapAreaClicked(int areaIndex);
	void populateTextLegend(Wt::WContainerWidget*& boxTextLegend);
	void populateTree(JTREE& jt, Wt::WTreeNode*& node);
	void processDataEvent(const DataEvent& event);
	void processEventCatalogue(string sYear, string sCata);
	void processEventCategory(vector<string> vsCategory);
	void processEventDemographic(vector<vector<string>> vvsDemo);
	void processEventDifferentiation(vector<string> vsDiff, string sTitle);
	void processEventMap(vector<string> vsRegion, vector<vector<vector<double>>> vvvdArea, vector<vector<double>> vvdData);
	void processEventParameter(vector<vector<string>> vvsVariable, vector<vector<string>> vvsCata, vector<string> vsDIMIndex);
	void processEventTable(vector<vector<string>>& vvsTable, vector<string>& vsCol, vector<string>& vsRow, string& sRegion);
	void processEventTopic(vector<string> vsRowTopic, vector<string> vsColTopic);
	void processEventTree(JTREE jt);
	void resetBarGraph();
	void resetDownload();
	void resetMap();
	void resetTabAll();
	void resetTable();
	void resetTree();
	void seriesAddToGraph();
	void seriesRemoveFromGraph(const int& seriesIndex);
	void setMap(int iRow, int iCol, string sRegion);
	void setTable(int geoCode, string sRegion);
	void setTipAdd(const string& sTip) { setTip.emplace(sTip); }
	void toggleMobile();
	void treeClicked();
	void treeClicked(int& geoCode, string& sRegion);
	void updatePinButtons(string mapUnit);
	void updatePinButtons(string mapUnit, string sRegion);
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
	shared_ptr<Wt::WMemoryResource> loadIcon(vector<unsigned char>& binIcon);
	void tableReceiveDouble(const double& width);
	void tableReceiveString(const string& sInfo);

	shared_ptr<Wt::WMemoryResource> iconChevronDown, iconChevronRight, iconClose, iconTrash;
	Wt::JSignal<string> jsInfo;
	Wt::JSignal<double> jsTWidth;
	
	Wt::WLength wlAuto = Wt::WLength::Auto;
	Wt::WLength wlDataFrameWidth, wlDataFrameHeight, wlTableWidth, wlTableHeight;
};

