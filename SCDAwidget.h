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
#include <Wt/WTable.h>
#include <Wt/WTableCell.h>
#include <Wt/WPanel.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLength.h>
#include <Wt/WTabWidget.h>
#include <Wt/WImage.h>
#include <Wt/WEvent.h>
#include "SCDAserver.h"

using namespace std;
extern const string sroot;

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	string activeDesc, activeDiv, activeRegion, activeYear;
	vector<int> cbActive;
	vector<string> commMap;
	string db_path = sroot + "\\SCDA.db";
	bool debug = 1;
	vector<Wt::WString> defNames, mapRegionList;
	JFUNC jf;
	JTREE jt;
	enum Layer { Root, Year, Description, Region, Division };
	unordered_map<string, int> mapTok;
	unordered_map<wstring, vector<int>> mapTree;
	const int num_filters = 3;
	Wt::WString selectedRegion, selectedFolder;
	vector<int> selectedCell = { -1,-1 };
	int selectedRow = -1;
	string sessionID, mapRegion;
	vector<int> treeActive;
	enum treeType { Tree, Subtree };
	const Wt::WString wsAll = Wt::WString("All");

	WTFUNC *wtMap;
	Wt::WColor colourSelected, colourWhite;
	Wt::WContainerWidget *boxControl, *boxData, *boxTest, *boxLoad, *boxSearch, *boxTable, *boxList;
	Wt::WComboBox *cbYear, *cbDesc, *cbRegion, *cbDiv;
	Wt::WImage* imgMap;
	Wt::WLineEdit* lineEditTest, *lineEditSearch;
	Wt::WPanel *panelYear, *panelDesc, *panelRegion, *panelDiv;
	Wt::WPushButton *pbTest, *pbTable, *pbMap, *pbSearch;
	Wt::WSelectionBox* sbList;
	Wt::WSpinBox* spinBoxMapX, *spinBoxMapY, *spinBoxMapRot;
	SCDAserver& sRef;
	Wt::WTable* wtTable;
	Wt::WTabWidget* tabData;
	Wt::WText* textMessage, *tableTitle, *textTable, *textList;
	Wt::WTree *treeCata;
	Wt::WTreeNode* treeRoot;

	void cbDescClicked();
	void cbDivClicked();
	void cbRegionClicked();
	void cbYearClicked();
	void connect();
	void folderClicked(Wt::WString wsTable);
	void init();
	void initUI();
	void makeUI();
	void mouseMapClick(const Wt::WMouseEvent& e);
	void pbMapClicked();
	void pbSearchClicked();
	void pbTestClicked();
	void pbTableClicked();
	void processDataEvent(const DataEvent& event);
	void selectTableCell(int iRow, int iCol);
	void selectTableRow(int iRow);
	void setLayer(Layer layer, vector<string> prompt);
	void setTable(vector<string> prompt);
	void tableClicked(Wt::WString wsTable);
	void updateMapRegionList(vector<string>& sList, vector<Wt::WString>& wsList, int mode);

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver) 
	{ 
		init();
	}

	Wt::WLength len5p = Wt::WLength("5px");

	// TEMPLATES

	
};

