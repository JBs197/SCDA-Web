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

struct WJUNITPIN
{
	std::string activeRegion, activeUnit, sRegionPopulation;
	int regionPopulation = -1;
	std::string sUnitPreference = "% of population";
	unsigned xChecksum;
};

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	std::string activeCata;
	bool filtersEnabled = 0;
	bool first = 1;
	JCRC32 jcrc;
	bool jsEnabled = 0;
	JSORT jsort;
	JTIME jtime;
	std::unordered_map<std::string, int> mapNumVar;  // sCata -> number of variables (excluding col/row)
	std::unordered_map<int, std::string> mapTimeWord;  // word index -> word  (shown in init).
	std::unordered_map<std::string, Wt::WString> mapTooltip;  // sPrompt -> wsTooltip
	std::unordered_map<std::string, std::string> mapUnit;  // ambiguous unit -> definitive unit
	const int num_filters = 3;
	int screenWidth, screenHeight;         // Unit of pixels, measured from the client. 
	Wt::WString selectedRegion, selectedFolder;
	std::string sessionID;
	std::unordered_set<std::string> setTip;         // List of helpful tips which have been dismissed by the user.
	std::vector<std::vector<std::string>> tableCol, tableCore, tableRow;
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
	WJTREE* wjTree = nullptr;
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
	Wt::WTreeTable* treeDialog;
	Wt::WTreeNode *treeNodeSel;
	std::unique_ptr<Wt::WDialog> wdBlock = nullptr;

	std::vector<Wt::WPushButton*> allPB;

	void cleanUnit(std::string& unit);
	void connect();
	void err(std::string message);
	
	int getHeight();
	std::string getUnit();
	int getWidth();
	
	void incomingHeaderSignal(const int& iRow, const int& iCol);
	void incomingPreviewSignal(const int& type);
	void incomingPullSignal(const int& pullType);
	void incomingResetSignal(const int& resetType);
	void incomingVarSignal();
	
	void init();
	void initMaps();
	void initUI();
	
	std::unique_ptr<Wt::WContainerWidget> makeBoxData();
	unsigned makeParamChecksum(std::vector<std::vector<std::string>>& vvsParameter);
	void mapAreaClicked(int areaIndex);
	
	void populateTextLegend(WJLEGEND*& wjLegend);
	void populateTree(JTREE& jt, int parentID, Wt::WTreeNode*& parentNode);
	
	void processDataEvent(const DataEvent& event);
	void processEventCatalogue(std::string sYear, std::string sCata);
	void processEventCategory(std::vector<std::string> vsCategory);
	void processEventConnection();
	void processEventDemographic(std::vector<std::vector<std::string>> vvsDemo);
	void processEventDifferentiation(std::vector<std::string> vsDiff, std::string sTitle);
	void processEventMap(std::vector<std::string> vsRegion, std::vector<std::vector<std::vector<double>>> vvvdFrame, std::vector<std::vector<std::vector<double>>> vvvdArea, std::vector<std::vector<double>> vvdData);
	void processEventParameter(std::vector<std::vector<std::vector<std::string>>> vvvsParameter, std::vector<std::vector<std::string>> vvsCata);
	void processEventTable(std::vector<std::vector<std::string>>& vvsTable, std::vector<std::vector<std::string>>& vvsCol, std::vector<std::vector<std::string>>& vvsRow);
	void processEventTopic(std::vector<std::string> vsRowTopic, std::vector<std::string> vsColTopic);
	void processEventTree();
	
	void resetBarGraph();
	void resetDownload();
	void resetMap();
	void resetTabAll();
	void resetTable();
	void resetTree();
	
	void seriesAddToGraph(int mode);
	void seriesRemoveFromGraph(const int& seriesIndex);
	
	void setMap(int iRow, int iCol, std::string sRegion);
	void setTable(int geoCode, std::string sRegion);
	void setTipAdd(const std::string& sTip) { setTip.emplace(sTip); }
	
	void tabChanged(const int& tabIndex);
	void treeClicked();
	void getTreeClicked(int& geoCode, std::string& sRegion);
	
	void updateDownloadTab();
	void updatePinButtons();
	void updatePinButtons(int mode);
	void updateRegion(std::vector<std::string> vsNamePop);
	void updateTextCata(int numCata);
	void updateUnit(std::string sUnit);

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver), 
		jsInfo(this, "jsInfo", 1), jsTWidth(this, "jsTWidth", 1) { 
		this->setId("SCDAwidget");
		init();
	}

	void displayCata(const Wt::WKeyEvent& wKey);
	std::shared_ptr<Wt::WMemoryResource> loadCSS(std::vector<unsigned char>& binCSS);
	std::shared_ptr<Wt::WMemoryResource> loadIcon(std::vector<unsigned char>& binIcon);
	void tableReceiveDouble(const double& width);
	void tableReceiveString(const std::string& sInfo);

	std::shared_ptr<Wt::WMemoryResource> cssTextPlain, cssTextShaded;
	std::shared_ptr<Wt::WMemoryResource> iconChevronDown, iconChevronRight, iconClose, iconTrash;
	Wt::JSignal<std::string> jsInfo;
	Wt::JSignal<double> jsTWidth;
	
	Wt::WLength wlAuto = Wt::WLength::Auto;
	Wt::WLength wlDataFrameWidth, wlDataFrameHeight, wlTableWidth, wlTableHeight;
};

