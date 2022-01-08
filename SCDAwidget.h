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

struct WJUNITPIN
{
	string activeRegion, activeUnit, sRegionPopulation;
	int regionPopulation = -1;
	string sUnitPreference = "% of population";
	unsigned xChecksum;
};

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	string activeCata;
	bool filtersEnabled = 0;
	bool first = 1;
	JCRC32 jcrc;
	JFUNC jf;
	bool jsEnabled = 0;
	JTREE jtRegion, jtWidget;
	unordered_map<string, int> mapNumVar;  // sCata -> number of variables (excluding col/row)
	unordered_map<int, string> mapTimeWord;  // word index -> word  (shown in init).
	unordered_map<string, Wt::WString> mapTooltip;  // sPrompt -> wsTooltip
	unordered_map<string, string> mapUnit;  // ambiguous unit -> definitive unit
	bool mobile = 0;
	const int num_filters = 3;
	int screenWidth, screenHeight;         // Unit of pixels, measured from the client. 
	Wt::WString selectedRegion, selectedFolder;
	string sessionID;
	unordered_set<string> setTip;         // List of helpful tips which have been dismissed by the user.
	vector<vector<string>> tableCol, tableCore, tableRow;
	int tableWidth, boxTableWidth;        // Used by JS functions.
	int tabRecent = 0;
	Wt::WCssDecorationStyle wcssAttention, wcssDefault, wcssHighlighted;
	const Wt::WString wsAll = Wt::WString("All");
	const Wt::WString wsNoneSel = Wt::WString("[None selected]");

	WJBARGRAPH* wjBarGraph = nullptr;
	WJCONFIG* wjConfig = nullptr;
	WJDOWNLOAD* wjDownload = nullptr;
	WJMAP* wjMap = nullptr;
	WJLEGEND* wjlMap = nullptr;
	WJTABLE* tableData = nullptr;
	WJTABLEBOX* wjTableBox = nullptr;
	WJUNITPIN wjUnitPin;
	WTPAINT* wtMap = nullptr;

	Wt::WColor wcGrey, wcSelectedStrong, wcSelectedWeak, wcWhite;
	Wt::WContainerWidget *boxData;
	Wt::WVBoxLayout* layoutConfig;
	Wt::WLineEdit* leTest;
	Wt::WSelectionBox* sbList;
	SCDAserver& sRef;
	Wt::WStackedWidget* stackedTabData;
	Wt::WTabWidget* tabData;
	Wt::WText *textTable;
	Wt::WTree *treeDialog, *treeRegion;
	Wt::WTreeNode *treeNodeSel;
	unique_ptr<Wt::WDialog> wdBlock = nullptr;

	vector<Wt::WPushButton*> allPB;

	void cleanUnit(string& unit);
	void connect();
	void err(string message);
	int getHeight();
	string getUnit();
	int getWidth();
	void incomingHeaderSignal(const int& iRow, const int& iCol);
	void incomingPreviewSignal(const int& type);
	void incomingPullSignal(const int& pullType);
	void incomingResetSignal(const int& resetType);
	void incomingVarSignal();
	void init();
	void initMaps();
	void initUI();
	unique_ptr<Wt::WContainerWidget> makeBoxData();
	unsigned makeParamChecksum(vector<vector<string>>& vvsParameter);
	void mapAreaClicked(int areaIndex);
	void populateTextLegend(WJLEGEND*& wjLegend);
	void populateTree(JTREE& jt, Wt::WTreeNode*& node);
	void processDataEvent(const DataEvent& event);
	void processEventCatalogue(string sYear, string sCata);
	void processEventCategory(vector<string> vsCategory);
	void processEventConnection();
	void processEventDemographic(vector<vector<string>> vvsDemo);
	void processEventDifferentiation(vector<string> vsDiff, string sTitle);
	void processEventMap(vector<string> vsRegion, vector<vector<vector<double>>> vvvdFrame, vector<vector<vector<double>>> vvvdArea, vector<vector<double>> vvdData);
	void processEventParameter(vector<vector<vector<string>>> vvvsParameter, vector<vector<string>> vvsCata);
	void processEventTable(vector<vector<string>>& vvsTable, vector<vector<string>>& vvsCol, vector<vector<string>>& vvsRow);
	void processEventTopic(vector<string> vsRowTopic, vector<string> vsColTopic);
	void processEventTree();
	void resetBarGraph();
	void resetDownload();
	void resetMap();
	void resetTabAll();
	void resetTable();
	void resetTree();
	void seriesAddToGraph(int mode);
	void seriesRemoveFromGraph(const int& seriesIndex);
	void setMap(int iRow, int iCol, string sRegion);
	void setTable(int geoCode, string sRegion);
	void setTipAdd(const string& sTip) { setTip.emplace(sTip); }
	void tabChanged(const int& tabIndex);
	void toggleMobile();
	void treeClicked();
	void treeClicked(int& geoCode, string& sRegion);
	void updateDownloadTab();
	void updatePinButtons();
	void updatePinButtons(int mode);
	void updateRegion(vector<string> vsNamePop);
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
	shared_ptr<Wt::WMemoryResource> loadCSS(vector<unsigned char>& binCSS);
	shared_ptr<Wt::WMemoryResource> loadIcon(vector<unsigned char>& binIcon);
	void tableReceiveDouble(const double& width);
	void tableReceiveString(const string& sInfo);

	shared_ptr<Wt::WMemoryResource> cssTextPlain, cssTextShaded;
	shared_ptr<Wt::WMemoryResource> iconChevronDown, iconChevronRight, iconClose, iconTrash;
	Wt::JSignal<string> jsInfo;
	Wt::JSignal<double> jsTWidth;
	
	Wt::WLength wlAuto = Wt::WLength::Auto;
	Wt::WLength wlDataFrameWidth, wlDataFrameHeight, wlTableWidth, wlTableHeight;
};

