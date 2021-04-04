#include "SCDAwidget.h"

void SCDAwidget::askRegion(SCDAserver& sRef, vector<string> ancestry)
{
	// This function is called whenever cbDesc is specified, or when a 
	// catalogue subtree is expanded. Form [sessionID, syear, sname, geo1, geo2, ...].
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
void SCDAwidget::cbDescClicked()
{
	const Wt::WString wsdesc = cbDesc->currentText();
	const Wt::WString wsyear(activeYear);
	Wt::WTreeNode* pRoot = treeCata->treeRoot();
	vector<Wt::WTreeNode*> pYears = pRoot->childNodes();
	if (wsdesc == wsAll)
	{
		activeDesc = "All";
		for (int ii = 0; ii < pYears.size(); ii++)
		{
			Wt::WString wstemp = pYears[ii]->label()->text();
			if (wstemp == wsyear)
			{
				pYears[ii]->setHidden(0);
				pYears[ii]->expand();
			}
		}
		panelRegion->setTitle("Select a Geographical Region");
		cbRegion->setCurrentIndex(0);
		cbRegion->setEnabled(0);
	}
	else
	{
		Wt::WApplication* app = Wt::WApplication::instance();
		vector<string> ancestry(3);
		ancestry[0] = app->sessionId();
		ancestry[1] = wsyear.toUTF8();
		ancestry[2] = wsdesc.toUTF8();
		activeDesc = ancestry[2];
		setDesc(sRef, ancestry);
		int pyIndex;
		for (int ii = 0; ii < pYears.size(); ii++)
		{
			Wt::WString wstemp = pYears[ii]->label()->text();
			if (wstemp == wsyear)
			{
				pYears[ii]->setHidden(0);
				pYears[ii]->expand();
				pyIndex = ii;
			}
			else
			{
				pYears[ii]->setHidden(1);
				pYears[ii]->collapse();
			}
		}
		vector<Wt::WTreeNode*> pDescs = pYears[pyIndex]->childNodes();
		for (int ii = 0; ii < pDescs.size(); ii++)
		{
			Wt::WString wstemp = pDescs[ii]->label()->text();
			if (wstemp == wsdesc)
			{
				pDescs[ii]->setHidden(0);
				pDescs[ii]->expand();
			}
			else
			{
				pDescs[ii]->setHidden(1);
				pDescs[ii]->collapse();
			}
		}
	}
}
void SCDAwidget::cbYearClicked()
{
	Wt::WString wsyear = cbYear->currentText();
	Wt::WTreeNode* pRoot = treeCata->treeRoot();
	vector<Wt::WTreeNode*> pYears = pRoot->childNodes();
	if (wsyear == wsAll)
	{
		activeYear = "All";
		for (int ii = 0; ii < pYears.size(); ii++)
		{
			pYears[ii]->collapse();
			pYears[ii]->setHidden(0);
		}
		panelDesc->setTitle("Select a Catalogue Description");
		cbDesc->setCurrentIndex(0);
		cbDesc->setEnabled(0);
		cbRegion->setCurrentIndex(0);
		cbRegion->setEnabled(0);
	}
	else
	{
		Wt::WApplication* app = Wt::WApplication::instance();
		vector<string> ancestry(3);
		ancestry[0] = app->sessionId();
		ancestry[1] = wsyear.toUTF8();
		activeYear = ancestry[1];
		setYear(sRef, ancestry);
		for (int ii = 0; ii < pYears.size(); ii++)
		{
			Wt::WString wstemp = pYears[ii]->label()->text();
			if (wstemp == wsyear)
			{
				pYears[ii]->setHidden(0);
				pYears[ii]->expand();
			}
			else
			{
				pYears[ii]->setHidden(1);
				pYears[ii]->collapse();
			}
		}
	}
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
	cbYear->changed().connect(this, &SCDAwidget::cbYearClicked);
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
	cbDesc->changed().connect(this, &SCDAwidget::cbDescClicked);
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
	bool wstr = 0;
	long long timer;
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
		jf.timerStart();
		// Make a pointer to the parent catalogue in the main tree.
		vector<vector<int>> tree_st = event.get_tree_st();
		vector<string> tree_pl = event.get_tree_pl();
		if (tree_pl.size() < 1) { wstr = 1; }
		vector<wstring> wtree_pl = event.get_wtree_pl();
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
		timer = jf.timerRestart();
		cout << "PDE(get cata pointer): " << timer << "ms" << endl;

		// Use the catalogue pointer as the root for the subtree.
		vector<Wt::WTreeNode*> child = pDesc->childNodes();
		pDesc->removeChildNode(child[0]);  // Remove the dummy entry.
		vector<int> roots = jf.get_roots(tree_st);
		for (int ii = 0; ii < roots.size(); ii++)
		{
			if (wstr)
			{
				const Wt::WString wstemp(wtree_pl[roots[ii]]);
				auto subRoot = make_unique<Wt::WTreeNode>(wstemp);
				auto subTreeRoot = subRoot.get();
				pDesc->addChildNode(move(subRoot));
				processDataEventHelper(Subtree, tree_st, wtree_pl, roots[ii], subTreeRoot, sessionID);
			}
			else
			{
				const Wt::WString wstemp(tree_pl[roots[ii]]);
				auto subRoot = make_unique<Wt::WTreeNode>(wstemp);
				auto subTreeRoot = subRoot.get();
				pDesc->addChildNode(move(subRoot));
				processDataEventHelper(Subtree, tree_st, tree_pl, roots[ii], subTreeRoot, sessionID);
			}
		}
		timer = jf.timerRestart();
		cout << "PDE(build tree): " << timer << "ms" << endl;
		break;
	}
	case 4:  // Label.
	{
		string message = event.get_text();
		textTest->setText(Wt::WString::fromUTF8(message));
		break;
	}
	case 6:  // DescList.
	{
		vector<string> list = event.get_list();
		activeYear = list[0];
		const Wt::WString wsyear(list[0]);
		Wt::WTreeNode* pRoot = treeCata->treeRoot();
		vector<Wt::WTreeNode*> pYears = pRoot->childNodes();
		Wt::WString wstemp;
		string temp;
		for (int ii = 0; ii < pYears.size(); ii++)
		{
			wstemp = pYears[ii]->label()->text();
			if (wstemp == wsyear)
			{
				pYears[ii]->expand();
				panelDesc->setTitle("Select a Catalogue Description (" + wsyear + ")");
				cbDesc->clear();
				cbDesc->addItem(wsAll);
				for (int jj = 1; jj < list.size(); jj++)
				{
					const Wt::WString wsDesc(list[jj]);
					cbDesc->addItem(wsDesc);
				}
				cbDesc->setEnabled(1);
				break;
			}
		}
		break;
	}

	}
}
void SCDAwidget::setDesc(SCDAserver& sRef, vector<string> ancestry)
{
	// RESUME HERE.
}
void SCDAwidget::setYear(SCDAserver& sRef, vector<string> ancestry)
{
	// This function is called whenever a 'year' branch is expanded in the tree widget,
	// or when a year is specified in the 'year' combobox.
	sRef.setYear(ancestry);
}
