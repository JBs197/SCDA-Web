#pragma once
#define TOK 1
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WTable.h>
#include <Wt/WTableCell.h>
#include <Wt/WPanel.h>
#include <Wt/WPushButton.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLength.h>
#include "SCDAserver.h"

using namespace std;

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	string activeDesc, activeDiv, activeRegion, activeYear;
	vector<int> cbActive;
	string db_path = sroot + "\\SCDA.db";
	vector<Wt::WString> defNames;
	JFUNC jf;
	JTREE jt;
	enum Layer { Root, Year, Description, Region, Division };
	unordered_map<string, int> mapTok;
	unordered_map<wstring, vector<int>> mapTree;
	const int num_filters = 3;
	Wt::WString selectedRegion;
	string sessionID;
	vector<int> treeActive;
	enum treeType { Tree, Subtree };
	const Wt::WString wsAll = Wt::WString("All");

	Wt::WContainerWidget *boxControl, *boxTreelist, *boxTable, *boxText, *boxButtonTest, *boxButtonTable, *boxLineEdit;
	Wt::WComboBox *cbYear, *cbDesc, *cbRegion, *cbDiv;
	Wt::WLineEdit* lineEdit;
	Wt::WPanel *panelYear, *panelDesc, *panelRegion, *panelDiv;
	Wt::WPushButton *pbTest, *pbTable;
	Wt::WSelectionBox* sbList;
	SCDAserver& sRef;
	Wt::WTable* wtTable;
	Wt::WText* textTest, *tableTitle;
	Wt::WTree* treeCata;
	Wt::WTreeNode* treeRoot;

	void cbDescClicked();
	void cbDivClicked();
	void cbRegionClicked();
	void cbYearClicked();
	void connect();
	void init();
	void initTOK();
	void initUI(int);
	void makeUI();
	void makeUItok();
	void pbTestClicked();
	void pbTableClicked();
	void processDataEvent(const DataEvent& event);
	void setLayer(Layer layer, vector<string> prompt);
	void setTable(vector<string> prompt);
	void tableClicked(Wt::WString wsTable);
	void tokClicked();

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver) 
	{ 
		if (TOK) { initTOK(); }
		else { init(); }
	}

	Wt::WLength len5p = Wt::WLength("5px");

	// TEMPLATES

	
};

