#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WTable.h>
#include <Wt/WPanel.h>
#include <Wt/WPushButton.h>
#include "SCDAserver.h"

using namespace std;

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	string db_path = sroot + "\\SCDA.db";
	const int num_filters = 3;
	string sessionID;

	Wt::WContainerWidget *boxControl, *boxTreelist, *boxTable;
	Wt::WComboBox *cbYear, *cbDesc, *cbRegion;
	Wt::WPanel *panelYear, *panelDesc, *panelRegion;
	Wt::WPushButton* pbTest;
	Wt::WSelectionBox* sbList;
	SCDAserver& sRef;
	Wt::WTable* wtTable;
	Wt::WText* textTest;
	Wt::WTree* treeCata;

	void connect();
	void init();
	void processDataEvent(const DataEvent& event);

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver) { init(); }

	void askTree();
	void updateTree(vector<vector<int>>, vector<string>);
};

