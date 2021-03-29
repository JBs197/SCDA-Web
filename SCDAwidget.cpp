#include "SCDAwidget.h"

void SCDAwidget::askTree()
{
	vector<string> filters;
	filters.resize(num_filters);
	Wt::WString wstemp = cbYear->currentText();
	string temp = wstemp.toUTF8();
	filters[0] = temp;
	wstemp = cbDesc->currentText();
	temp = wstemp.toUTF8();
	filters[1] = temp;
	wstemp = cbRegion->currentText();
	temp = wstemp.toUTF8();
	filters[2] = temp;
	sRef.updateFilter(sessionID, filters);
}
void SCDAwidget::connect()
{
	if (sRef.connect(this, bind(&SCDAwidget::processDataEvent, this, placeholders::_1)))
	{
		Wt::WApplication::instance()->enableUpdates(1);
		sessionID = Wt::WApplication::instance()->sessionId();
	}
}
void SCDAwidget::init()
{
	clear();
	int numTables;
	sRef.init(numTables);
	string temp;

	auto uniqueBoxControl = make_unique<Wt::WContainerWidget>();
	boxControl = uniqueBoxControl.get();
	auto uniquePanelYear = make_unique<Wt::WPanel>();
	panelYear = uniquePanelYear.get();
	auto uniqueCbYear = make_unique<Wt::WComboBox>();
	cbYear = uniqueCbYear.get();
	auto uniquePanelDesc = make_unique<Wt::WPanel>();
	panelDesc = uniquePanelDesc.get();
	auto uniqueCbDesc = make_unique<Wt::WComboBox>();
	cbDesc = uniqueCbDesc.get();
	auto uniquePanelRegion = make_unique<Wt::WPanel>();
	panelRegion = uniquePanelRegion.get();
	auto uniqueCbRegion = make_unique<Wt::WComboBox>();
	cbRegion = uniqueCbDesc.get();
	auto uniqueBoxTreelist = make_unique<Wt::WContainerWidget>();
	boxTreelist = uniqueBoxTreelist.get();
	auto uniqueSbList = make_unique<Wt::WSelectionBox>();
	sbList = uniqueSbList.get();
	//auto uniqueTreeCata = make_unique<Wt::WTree>();
	//treeCata = uniqueTreeCata.get();
	auto uniqueBoxTable = make_unique<Wt::WContainerWidget>();
	boxTable = uniqueBoxTable.get();
	auto uniqueWtTable = make_unique<Wt::WTable>();
	wtTable = uniqueWtTable.get();
	auto uniqueTextTest = make_unique<Wt::WText>();
	textTest = uniqueTextTest.get();
	auto uniquePbTest = make_unique<Wt::WPushButton>("TEST BUTTON");
	pbTest = uniquePbTest.get();
	auto vLayout = make_unique<Wt::WVBoxLayout>();  
	auto hLayout = make_unique<Wt::WHBoxLayout>();

	// Initial values for cbYear.
	vector<string> year_list = sRef.getYearList();
	cbYear->changed().connect(this, &SCDAwidget::askTree);
	Wt::WString wstemp = Wt::WString("All");
	cbYear->addItem(wstemp);
	for (int ii = 0; ii < year_list.size(); ii++)
	{
		Wt::WString wstemp = Wt::WString(year_list[ii]);
		cbYear->addItem(wstemp);
	}

	// Initial values for cbDesc.
	wstemp = Wt::WString("All");
	cbDesc->addItem(wstemp);

	// Initial values for cbRegion.
	wstemp = Wt::WString("All");
	cbRegion->addItem(wstemp);

	uniquePanelYear->setTitle("Select a Year");
	uniquePanelYear->setCentralWidget(move(uniqueCbYear));
	uniqueBoxControl->addWidget(move(uniquePanelYear));
	uniquePanelDesc->setTitle("Select a Catalogue Description");
	uniquePanelDesc->setCentralWidget(move(uniqueCbDesc));
	uniqueBoxControl->addWidget(move(uniquePanelDesc));
	uniquePanelRegion->setTitle("Select a Geographical Region");
	uniquePanelRegion->setCentralWidget(move(uniqueCbRegion));
	uniqueBoxControl->addWidget(move(uniquePanelRegion));

	// Initial values for treeCata.
	askTree();

	// Initial values for sbList.
	uniqueSbList->setHidden(1);
	//uniqueBoxTreelist->addWidget(move(uniqueSbList));

	uniqueBoxTable->addWidget(move(uniqueWtTable));

	// Initial values for textTest.
	temp = "Database has " + to_string(numTables) + " table results.";
	Wt::WString wstext(temp.c_str());
	uniqueTextTest->setText(wstext);

	hLayout->addWidget(move(uniqueBoxControl));
	hLayout->addWidget(move(uniqueBoxTreelist));
	hLayout->addWidget(move(uniqueBoxTable));	
	
	vLayout->addLayout(move(hLayout));
	vLayout->addWidget(move(uniqueTextTest));
	vLayout->addWidget(move(uniquePbTest));
	this->setLayout(move(vLayout));

	connect();
}
void SCDAwidget::processDataEvent(const DataEvent& event)
{
	Wt::WApplication* app = Wt::WApplication::instance();
	app->triggerUpdate();
	int display = event.type();
	switch (display)
	{
	case 0:
		break;
	case 1:
		boxTreelist->clear();  // RESUME HERE. Get tree from event to widget.
		treeCata = boxTreelist->addWidget(event.makeTree());  
	}
}
void SCDAwidget::updateTree(vector<vector<int>> tree_st, vector<string> tree_pl)
{
	Wt::WTreeNode* wroot = treeCata->treeRoot();
	string temp = wroot->id();
	int bbq = 2;

}
