#include "SCDAwidget.h"

void SCDAwidget::askRegion(SCDAserver& sRef, vector<string> ancestry)
{
	// This function is called whenever cbDesc is specified, or when a 
	// catalogue subtree is expanded. Form [sessionID, syear, sname].
	
	sRef.pullRegion(ancestry);
}
void SCDAwidget::askTree()
{
	// This function is called whenever any filter is changed.

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

	temp = "All";
	for (int ii = 0; ii < num_filters; ii++)
	{
		if (filters[ii] == temp)
		{
			cbActive[ii] = 0;
		}
		else
		{
			cbActive[ii] = 1;
		}
	}

	sRef.pullTree(sessionID, filters);
}
void SCDAwidget::cbYearClicked()
{
	// Update cbActive.
	Wt::WString yearFilter = cbYear->currentText();
	Wt::WString wstemp("All");
	if (yearFilter == wstemp)
	{
		cbActive.assign(num_filters, 0);
	}
	else
	{
		cbActive[0] = 1;
		for (int ii = 1; ii < num_filters; ii++)
		{
			cbActive[ii] = 0;
		}
	}

	// Pull the filtered tree from the database.
	askTree();
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
	connect();
	cbActive.assign(num_filters, 0);
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
	cbRegion = uniqueCbRegion.get();
	auto uniqueBoxTreelist = make_unique<Wt::WContainerWidget>();
	boxTreelist = uniqueBoxTreelist.get();
	auto uniqueSbList = make_unique<Wt::WSelectionBox>();
	sbList = uniqueSbList.get();
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
	cbDesc->changed().connect(this, &SCDAwidget::askTree);
	cbDesc->setEnabled(0);

	// Initial values for cbRegion.
	wstemp = Wt::WString("All");
	cbRegion->addItem(wstemp);
	cbRegion->setEnabled(0);

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

}
void SCDAwidget::processDataEvent(const DataEvent& event)
{
	Wt::WApplication* app = Wt::WApplication::instance();
	app->triggerUpdate();
	string sessionID = app->sessionId();
	int display = event.type();
	switch (display)
	{
	case 0:  // Connect.
	{
		break;
	}
	case 1:  // Tree.
	{
		vector<vector<int>> tree_st = event.get_tree_st();
		vector<string> tree_pl = event.get_tree_pl();
		boxTreelist->clear();
		auto tree = make_unique<Wt::WTree>();
		treeCata = boxTreelist->addWidget(move(tree));
		const Wt::WString wstemp(event.tree_pl[0].c_str());
		auto tRoot = make_unique<Wt::WTreeNode>(wstemp);
		auto treeRoot = tRoot.get();
		treeCata->setTreeRoot(move(tRoot));
		processDataEventHelper(Tree, tree_st, tree_pl, 0, treeRoot, sessionID);
		treeRoot->expand();

		vector<int> cataInYear;
		Wt::WString wsAll = "All";
		if (cbActive[0] == 0)
		{
			cbDesc->clear();
			cbDesc->addItem(wsAll);
			cbDesc->setEnabled(0);
			cbRegion->clear();
			cbRegion->addItem(wsAll);
			cbRegion->setEnabled(0);
		}
		else if (cbActive[1] == 0)
		{
			cbDesc->clear();
			cbDesc->addItem(wsAll);
			cataInYear.assign(tree_st[1].begin() + 2, tree_st[1].end());
			for (int ii = 0; ii < cataInYear.size(); ii++)
			{
				Wt::WString wstemp = Wt::WString::fromUTF8(tree_pl[cataInYear[ii]]);
				cbDesc->addItem(wstemp);
			}
			cbDesc->setEnabled(1);

			cbRegion->clear();
			cbRegion->addItem(wsAll);
			cbRegion->setEnabled(0);
		}
		else                                    // A catalogue has been specified.
		{
			//askRegion();   // Is this correct??
		}
		break;
	}
	case 2:  // Subtree.
	{
		// Make a pointer to the parent catalogue in the main tree.
		vector<vector<int>> tree_st = event.get_tree_st();
		vector<string> tree_pl = event.get_tree_pl();
		vector<string> ancestry = event.get_ancestry();
		const Wt::WString wsYear = Wt::WString::fromUTF8(ancestry[1]);
		const Wt::WString wsDesc = Wt::WString::fromUTF8(ancestry[2]);
		vector<Wt::WTreeNode*> pYears = treeCata->treeRoot()->childNodes();
		Wt::WTreeNode* pYear = nullptr;
		Wt::WTreeNode* pDesc = nullptr;
		Wt::WText* wsText = nullptr;
		for (int ii = 0; ii < pYears.size(); ii++)
		{
			wsText = pYears[ii]->label();
			const Wt::WString wstemp = wsText->text();
			if (wstemp == wsYear)
			{
				pYear = pYears[ii];
				break;
			}
		}
		vector<Wt::WTreeNode*> pDescs = pYear->childNodes();
		for (int ii = 0; ii < pDescs.size(); ii++)
		{
			wsText = pDescs[ii]->label();
			const Wt::WString wstemp = wsText->text();
			if (wstemp == wsDesc)
			{
				pDesc = pDescs[ii];
				break;
			}
		}
		
		// Use the catalogue pointer as the root for the subtree.
		vector<Wt::WTreeNode*> child = pDesc->childNodes();
		pDesc->removeChildNode(child[0]);  // Remove the dummy entry.
		vector<int> roots = jf.get_roots(tree_st);
		for (int ii = 0; ii < roots.size(); ii++)
		{
			const Wt::WString wstemp(tree_pl[roots[ii]].c_str());
			auto subRoot = make_unique<Wt::WTreeNode>(wstemp);
			auto subTreeRoot = subRoot.get();
			pDesc->addChildNode(move(subRoot));
			processDataEventHelper(Subtree, tree_st, tree_pl, roots[ii], subTreeRoot, sessionID);
		}
		break;
	}

	/*
	case 2:  // List.
	{
		vector<string> list = event.get_list();
		Wt::WComboBox* pCb = nullptr;
		switch (cbRecent)
		{
		case 1:
		{
			pCb = cbDesc;
			break;
		}
		}
		pCb->clear();
		pCb->addItem("All");
		for (int ii = 0; ii < list.size(); ii++)
		{
			Wt::WString wstemp(list[ii].c_str());
			pCb->addItem(wstemp);
		}
		pCb->setEnabled(1);
		break;
	}
	*/

	}
}
void SCDAwidget::processDataEventHelper(treeType tt, vector<vector<int>>& tree_st, vector<string>& tree_pl, int pl_index, Wt::WTreeNode*& me, string sessionID)
{
	vector<int> kids;
	vector<Wt::WTreeNode*> pkids;
	int pivot;
	int rowSize = tree_st[pl_index].size();

	for (int ii = 0; ii < rowSize; ii++)
	{
		if (tree_st[pl_index][ii] < 0)
		{
			pivot = ii;
			break;
		}
		else if (ii == rowSize - 1)
		{
			pivot = 0;
		}
	}
	if (tt == 0 && pivot == 2)  // This node has exactly 2 parents (root->year) so it's a catalogue.
	{
		vector<string> ancestry(3);
		ancestry[0] = sessionID;
		ancestry[2] = tree_pl[pl_index];
		int iparent = tree_st[pl_index][1];
		ancestry[1] = tree_pl[iparent];
		function<void()> fn = bind(&SCDAwidget::askRegion, ref(sRef), ancestry);
		me->expanded().connect(fn);
	}
	if (rowSize <= pivot + 1)  // Not a parent.
	{
		if (tt == 1) { return; }
		else if (tt == 0)  // Catalogue nodes receive a dummy entry by default. If the user
		{                  // tries to view a catalogue, it will be loaded on demand.
			const Wt::WString wstemp("Loading...");
			auto branch = make_unique<Wt::WTreeNode>(wstemp);
			me->addChildNode(move(branch));
			return;
		}
	}  
	kids.resize(rowSize - pivot - 1);
	pkids.resize(rowSize - pivot - 1);
	for (int ii = 0; ii < kids.size(); ii++)
	{
		kids[ii] = abs(tree_st[pl_index][pivot + 1 + ii]);
	}
	for (int ii = 0; ii < kids.size(); ii++)
	{
		const Wt::WString wstemp(tree_pl[kids[ii]]);
		auto branch = make_unique<Wt::WTreeNode>(wstemp);
		pkids[ii] = me->addChildNode(move(branch));
		
	}
	for (int ii = 0; ii < pkids.size(); ii++)
	{
		processDataEventHelper(tt, tree_st, tree_pl, kids[ii], pkids[ii], sessionID);
	}
}

