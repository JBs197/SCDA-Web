#pragma once
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WSpinBox.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WTreeView.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/WStandardItem.h>
#include <Wt/WTable.h>
#include <Wt/WTableCell.h>
#include <Wt/WPanel.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLength.h>
#include <Wt/WTabWidget.h>
#include <Wt/WMenuItem.h>
#include <Wt/WImage.h>
#include <Wt/WEvent.h>
#include <Wt/WString.h>
#include "SCDAserver.h"

using namespace std;
extern const string sroot;

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	string activeCata, activeCategory, activeColTopic, activeRowTopic, activeYear;
	string activeTableColTitle, activeTableRowTitle;
	vector<int> cbActive;
	string db_path = sroot + "\\SCDA.db";
	JFUNC jf;
	JTREE jtRegion;
	enum Layer { Root, Year, Description, Region, Division };
	unordered_map<string, int> mapVarIndex;  // unique id -> var index (panel, CBs)
	unordered_map<string, int> mapNumVar;  // sCata -> number of variables (excluding col/row)
	const int num_filters = 3;
	int numPreVariable = -1;  // Number of widgets in boxConfig prior to the "variable" panels.
	Wt::WString selectedRegion, selectedFolder;
	int selectedCol = -1, selectedRow = -1;
	string sessionID, mapRegion;
	vector<int> treeActive;
	enum treeType { Tree, Subtree };
	vector<vector<string>> vvsParameter;  // Form [variable index][MID0, MID1, ..., variable title].
	const Wt::WString wsAll = Wt::WString("All");
	const Wt::WString wsNoneSel = Wt::WString("[None selected]");

	WTPAINT* wtMap;

	Wt::WColor colourSelectedStrong, colourSelectedWeak, colourWhite;
	Wt::WComboBox *cbCategory, *cbColTopicTitle, *cbColTopicTable, *cbColTopicSel;
	Wt::WComboBox *cbRowTopicSel, *cbRowTopicTitle, *cbRowTopicTable, *cbYear;
	Wt::WContainerWidget *boxConfig, *boxData, *boxMap, *boxTest;
	Wt::WImage* imgMap;
	Wt::WVBoxLayout* layoutConfig;
	Wt::WLineEdit* leTest;
	Wt::WPanel *panelCategory, *panelColTopic, *panelRowTopic, *panelYear;
	Wt::WPushButton *pbColTopic, *pbMobile, *pbRowTopic;
	Wt::WSelectionBox* sbList;
	Wt::WSpinBox* spinBoxMapX, *spinBoxMapY, *spinBoxMapRot;
	SCDAserver& sRef;
	Wt::WTable* tableData;
	Wt::WTabWidget* tabData;
	Wt::WText* textLegend, *tableTitle, *textTable, *textList;
	Wt::WTree* treeRegion;

	vector<Wt::WComboBox*> varMID, varTitle;
	vector<Wt::WPanel*> varPanel;

	void addVariable(vector<string>& vsVariable);
	void addVariable(vector<vector<string>>& vvsCandidate);
	void cbCategoryClicked();
	void cbColRowSelClicked();
	void cbColRowTitleClicked(string id);
	void cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem);
	void cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem);
	void cbRenew(Wt::WComboBox*& cb, string sTop, vector<vector<string>>& vvsItem);
	void cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem, string selItem);
	void cbVarTitleClicked(string id);
	void cbVarMIDClicked(string id);
	void cbYearClicked();
	void connect();
	int getHeight();
	Wt::WString getTextLegend();
	vector<vector<string>> getVariable();
	int getWidth();
	void init();
	void initUI();
	void makeUI();
	void mapAreaClicked(int areaIndex);
	void mapAreaDoubleClicked(int areaIndex);
	void populateTree(JTREE& jt, Wt::WTreeNode*& node);
	void processDataEvent(const DataEvent& event);
	void removeVariable(int varIndex);
	void resetMap();
	void resetTable();
	void resetTopicSel();
	void resetTree();
	void resetVariables();
	void setTable(int geoCode, string sRegion);
	void tableCBUpdate(int iRow, int iCol);
	void tableClicked(int iRow, int iCol);
	void tableDeselect(int iRow, int iCol);
	void tableDoubleClicked(int iRow, int iCol);
	void tableSelect(int iRow, int iCol);
	void test();
	void treeClicked();

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver) 
	{ 
		init();
	}

	Wt::WLength wlAuto = Wt::WLength::Auto;
	Wt::WLength len5p = Wt::WLength("5px");
	Wt::WLength len200p = Wt::WLength("200px");
	Wt::WLength len300p = Wt::WLength("300px");
	Wt::WLength len600p = Wt::WLength("600px");
	Wt::WLength len700p = Wt::WLength("700px");
	Wt::WLength len800p = Wt::WLength("800px");
};

