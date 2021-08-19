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
	vector<int> cbActive;
	string db_path = sroot + "\\SCDA.db";
	vector<Wt::WString> mapRegionList;
	JFUNC jf;
	JTREE jtRegion;
	enum Layer { Root, Year, Description, Region, Division };
	unordered_map<string, int> mapVarIndex;  // unique id -> var index (panel, CBs)
	unordered_map<string, int> mapNumVar;  // sCata -> number of variables (excluding col/row)
	const int num_filters = 3;
	int numPreVariable = -1;  // Number of widgets in boxConfig prior to the "variable" panels.
	Wt::WString selectedRegion, selectedFolder;
	vector<int> selectedCell = { -1,-1 };
	string sessionID, mapRegion;
	vector<int> treeActive;
	enum treeType { Tree, Subtree };
	vector<vector<string>> vvsParameter;  // Form [variable index][MID0, MID1, ..., variable title].
	const Wt::WString wsAll = Wt::WString("All");

	WTPAINT* wtMap;

	Wt::WColor colourSelected, colourWhite;
	Wt::WComboBox *cbCategory, *cbColTopic, *cbColTopicTable, *cbRowTopic, *cbRowTopicTable;
	Wt::WComboBox *cbYear;
	Wt::WContainerWidget *boxConfig, *boxData, *boxTest;
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
	Wt::WText* textMessage, *tableTitle, *textTable, *textList;
	Wt::WTree* treeRegion;

	vector<Wt::WComboBox*> varMID, varTitle;
	vector<Wt::WPanel*> varPanel;

	void addVariable(vector<string>& vsVariable);
	void addVariable(vector<vector<string>>& vvsCandidate);
	void cbCategoryClicked();
	void cbColRowClicked(string id);
	void cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem);
	void cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem);
	void cbRenew(Wt::WComboBox*& cb, string sTop, vector<vector<string>>& vvsItem);
	void cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem, string selItem);
	void cbVarTitleClicked(string id);
	void cbVarMIDClicked(string id);
	void cbYearClicked();
	void connect();
	void dataClick();
	vector<vector<string>> getVariable();
	void init();
	void initUI();
	void makeUI();
	void mouseMapClick(const Wt::WMouseEvent& e);
	void populateTree(JTREE& jt, Wt::WTreeNode*& node);
	void processDataEvent(const DataEvent& event);
	void removeVariable(int varIndex);
	void resetVariables();
	void selectTableCell(int iRow, int iCol);
	void selectTableRow(int iRow);
	void setTable(int geoCode, string sRegion);
	void tableClicked(int iRow, int iCol);
	void tableDoubleClicked(int iRow, int iCol);
	void test();
	void treeClicked();
	void updateMapRegionList(vector<string>& sList, vector<Wt::WString>& wsList, int mode);

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver) 
	{ 
		init();
	}

	Wt::WLength wlAuto = Wt::WLength::Auto;
	Wt::WLength len5p = Wt::WLength("5px");
	Wt::WLength len300p = Wt::WLength("300px");
	Wt::WLength len600p = Wt::WLength("600px");

};

