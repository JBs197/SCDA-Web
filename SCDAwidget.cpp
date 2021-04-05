#include "SCDAwidget.h"

/*
void SCDAwidget::askRegion(SCDAserver& sRef, vector<string> ancestry)
{
	// This function is called whenever cbDesc is specified, or when a 
	// catalogue subtree is expanded. Form [sessionID, syear, sname, geo1, geo2, ...].
	sRef.pullRegion(ancestry);
}
void SCDAwidget::askRegionNX(vector<string> ancestry)
{
	// This function is called whenever cbDesc is specified, or when a 
	// catalogue subtree is expanded. Form [sessionID, syear, sname, geo1, geo2, ...].
	sRef.pullRegion(ancestry);
}
*/

void SCDAwidget::cbDescClicked()
{
	Wt::WString wsTemp;
	Wt::WString wsDesc = cbDesc->currentText();
	int yearIndex;
	Wt::WString wsYear = Wt::WString::fromUTF8(activeYear);
	vector<Wt::WTreeNode*> pYears = treeRoot->childNodes();
	for (int ii = 0; ii < pYears.size(); ii++)
	{
		wsTemp = pYears[ii]->label()->text();
		if (wsTemp == wsYear)
		{
			yearIndex = ii;
			break;
		}
	}
	vector<Wt::WTreeNode*> pDescs = pYears[yearIndex]->childNodes();
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt = { app->sessionId() };
	
	if (wsDesc == wsAll)
	{
		activeDesc = "All";
		for (int ii = 0; ii < pYears.size(); ii++)
		{
			if (ii != yearIndex)
			{
				pYears[ii]->collapse();
			}
			else
			{
				pYears[ii]->expand();
				pYears[ii]->setHidden(0);
			}
		}
		for (int ii = 0; ii < pDescs.size(); ii++)
		{
			pDescs[ii]->setHidden(0);
			pDescs[ii]->collapse();
		}
		panelRegion->setTitle(defNames[2]);
		cbRegion->setCurrentIndex(0);
		cbRegion->setEnabled(0);
	}
	else
	{
		prompt.resize(3);
		prompt[1] = activeYear;
		prompt[2] = wsDesc.toUTF8();
		setLayer(Description, prompt);
		activeDesc = prompt[2];
		for (int ii = 0; ii < pYears.size(); ii++)
		{
			if (ii != yearIndex)
			{
				pYears[ii]->setHidden(1);
			}
			else
			{
				pYears[ii]->expand();
				pYears[ii]->setHidden(0);
			}
		}
		for (int ii = 0; ii < pDescs.size(); ii++)
		{
			wsTemp = pDescs[ii]->label()->text();
			if (wsTemp == wsDesc)
			{
				pDescs[ii]->setHidden(0);
			}
			else
			{
				pDescs[ii]->setHidden(1);
				pDescs[ii]->collapse();
			}
		}
	}
}
void SCDAwidget::cbRegionClicked()
{

}
void SCDAwidget::cbYearClicked()
{
	Wt::WString wsyear = cbYear->currentText();
	vector<Wt::WTreeNode*> pYears = treeRoot->childNodes();
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt = { app->sessionId() };
	if (wsyear == wsAll)
	{
		setLayer(Root, prompt);
		activeYear = "All";
		for (int ii = 0; ii < pYears.size(); ii++)
		{
			pYears[ii]->collapse();
			pYears[ii]->setHidden(0);
		}
		panelDesc->setTitle(defNames[1]);
		panelRegion->setTitle(defNames[2]);
		cbDesc->setCurrentIndex(0);
		cbDesc->setEnabled(0);
		cbRegion->setCurrentIndex(0);
		cbRegion->setEnabled(0);
	}
	else
	{
		prompt.push_back(wsyear.toUTF8());
		setLayer(Year, prompt);
		activeYear = prompt[1];
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
	int numTables;
	sRef.init(numTables);
	connect();
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt = { app->sessionId() };

	defNames.resize(3);
	defNames[0] = Wt::WString::tr("cbYear");
	defNames[1] = Wt::WString::tr("cbDesc");
	defNames[2] = Wt::WString::tr("cbRegion");
	makeUI();
	initUI(numTables);
	setLayer(Root, prompt);

	//cbActive.assign(num_filters, 0);
	//askTree();
}
void SCDAwidget::initUI(int numTables)
{
	Wt::WString wstemp;

	// Initial values for panelYear.
	panelYear->setTitle(defNames[0]);
	cbYear->changed().connect(this, &SCDAwidget::cbYearClicked);

	// Initial values for panelDesc.
	panelDesc->setTitle(defNames[1]);
	cbDesc->changed().connect(this, &SCDAwidget::cbDescClicked);
	cbDesc->addItem(wsAll);
	cbDesc->setEnabled(0);

	// Initial values for panelRegion.
	panelRegion->setTitle(defNames[2]);
	cbRegion->changed().connect(this, &SCDAwidget::cbRegionClicked);
	cbRegion->addItem(wsAll);
	cbRegion->setEnabled(0);

	// Initial values for the tree widget.
	auto tRoot = make_unique<Wt::WTreeNode>("Census Tables");
	treeRoot = tRoot.get();
	treeCata->setTreeRoot(move(tRoot));

	// Initial values for the text widget.
	string temp = "Database has " + to_string(numTables) + " table results.";
	wstemp = Wt::WString(temp);
	textTest->setText(wstemp);

	// Initial values for the test button.
	pbTest->clicked().connect(this, &SCDAwidget::pbTestClicked);

}
void SCDAwidget::makeUI()
{
	this->clear();
	auto vLayout = make_unique<Wt::WVBoxLayout>();
	auto hLayoutFilterTree = make_unique<Wt::WHBoxLayout>();
	auto hLayoutTextButton = make_unique<Wt::WHBoxLayout>();

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
	auto uniqueTree = make_unique<Wt::WTree>();
	treeCata = uniqueTree.get();
	auto uniqueBoxTable = make_unique<Wt::WContainerWidget>();
	boxTable = uniqueBoxTable.get();
	auto uniqueTable = make_unique<Wt::WTable>();
	wtTable = uniqueTable.get();
	auto uniqueBoxText = make_unique<Wt::WContainerWidget>();
	boxText = uniqueBoxText.get();
	auto uniqueTextTest = make_unique<Wt::WText>();
	textTest = uniqueTextTest.get();
	auto uniqueBoxLineEdit = make_unique<Wt::WContainerWidget>();
	boxLineEdit = uniqueBoxLineEdit.get();
	auto uniqueLineEdit = make_unique<Wt::WLineEdit>();
	lineEdit = uniqueLineEdit.get();
	auto uniqueBoxButton = make_unique<Wt::WContainerWidget>();
	boxButton = uniqueBoxButton.get();
	auto uniquePbTest = make_unique<Wt::WPushButton>("TEST BUTTON");
	pbTest = uniquePbTest.get();
	//auto uniqueSbList = make_unique<Wt::WSelectionBox>();
	//sbList = uniqueSbList.get();

	auto lenAuto = Wt::WLength();
	auto len4 = Wt::WLength("400px");
	auto len3 = Wt::WLength("300px");
	auto len1 = Wt::WLength("100px");
	boxControl->resize(len4, lenAuto);
	boxTreelist->resize(len4, lenAuto);
	boxText->resize(len3, lenAuto);
	boxLineEdit->resize(len4, lenAuto);
	boxButton->resize(len1, lenAuto);

	uniquePanelYear->setCentralWidget(move(uniqueCbYear));
	uniquePanelDesc->setCentralWidget(move(uniqueCbDesc));
	uniquePanelRegion->setCentralWidget(move(uniqueCbRegion));
	uniqueBoxControl->addWidget(move(uniquePanelYear));
	uniqueBoxControl->addWidget(move(uniquePanelDesc));
	uniqueBoxControl->addWidget(move(uniquePanelRegion));
	uniqueBoxTreelist->addWidget(move(uniqueTree));
	uniqueBoxTable->addWidget(move(uniqueTable));
	uniqueBoxText->addWidget(move(uniqueTextTest));
	uniqueBoxLineEdit->addWidget(move(uniqueLineEdit));
	uniqueBoxButton->addWidget(move(uniquePbTest));

	hLayoutFilterTree->addWidget(move(uniqueBoxControl));
	hLayoutFilterTree->addWidget(move(uniqueBoxTreelist));

	hLayoutTextButton->addWidget(move(uniqueBoxText));
	hLayoutTextButton->addWidget(move(uniqueBoxLineEdit));
	hLayoutTextButton->addWidget(move(uniqueBoxButton));

	vLayout->addLayout(move(hLayoutFilterTree));
	vLayout->addLayout(move(hLayoutTextButton));
	vLayout->addWidget(move(uniqueBoxTable));

	this->setLayout(move(vLayout));
}
void SCDAwidget::pbTestClicked()
{
	vector<string> prompt(2);
	Wt::WApplication* app = Wt::WApplication::instance();
	prompt[0] = app->sessionId();
	Wt::WString wsTable = lineEdit->text();
	prompt[1] = wsTable.toUTF8();
	if (prompt[1].size() < 1) 
	{ 
		prompt[1] = "TG_Region$97-570-X1986002"; 
	}
	setTable(prompt);
}
void SCDAwidget::processDataEvent(const DataEvent& event)
{
	jf.timerStart();
	Wt::WApplication* app = Wt::WApplication::instance();
	app->triggerUpdate();
	vector<Wt::WTreeNode*> pYears, pDescs, pRegions;
	Wt::WTreeNode* pTemp;
	Wt::WString wsYear, wsDesc;
	string sessionID = app->sessionId();
	vector<string> slist, nextPrompt;
	vector<vector<wstring>> wtable;
	unordered_map<wstring, int> mapW;
	long long timer;
	int inum;

	int etype = event.type();
	switch (etype)
	{
	case 0:  // Connect.
		break;
	case 1:  // Populate the table widget.
		wtable = event.get_wtable();
		wtTable->setHeaderCount(1, Wt::Orientation::Horizontal);
		wtTable->setHeaderCount(1, Wt::Orientation::Vertical);
		for (int ii = 0; ii < wtable.size(); ii++)
		{
			for (int jj = 0; jj < wtable[ii].size(); jj++)
			{
				const Wt::WString wsCell(wtable[ii][jj]);
				auto cellText = make_unique<Wt::WText>(wsCell);
				cellText->setMargin(len5p);
				wtTable->elementAt(ii, jj)->addWidget(move(cellText));
			}
		}
		break;
	case 2:  // Set the root layer.
		slist = event.get_list();
		pYears = treeRoot->childNodes();
		while (pYears.size() > 0)
		{
			treeRoot->removeChildNode(pYears[0]);
			pYears.erase(pYears.begin());
		}
		nextPrompt.resize(2);
		nextPrompt[0] = sessionID;
		cbYear->clear();
		cbYear->addItem(wsAll);
		for (int ii = 0; ii < slist.size(); ii++)
		{
			const Wt::WString wsYear(slist[ii]);
			cbYear->addItem(wsYear);
			auto uniqueTemp = make_unique<Wt::WTreeNode>(wsYear);
			nextPrompt[1] = slist[ii];
			function<void()> fn = bind(&SCDAwidget::setLayer, this, Year, nextPrompt);
			uniqueTemp->expanded().connect(fn);
			pTemp = treeRoot->addChildNode(move(uniqueTemp));
			pTemp->addChildNode(make_unique<Wt::WTreeNode>("Loading..."));
		}
		treeRoot->expand();
		break;
	case 3:  // Set the year layer.
		slist = event.get_list();
		pYears = treeRoot->childNodes();
		pDescs = pYears[treeActive[1]]->childNodes();
		while (pDescs.size() > 0)
		{
			pYears[treeActive[1]]->removeChildNode(pDescs[0]);
			pDescs.erase(pDescs.begin());
		}
		wsYear = pYears[treeActive[1]]->label()->text();
		nextPrompt.resize(3);
		nextPrompt[0] = sessionID;
		nextPrompt[1] = wsYear.toUTF8();
		cbDesc->clear();
		cbDesc->addItem(wsAll);
		for (int ii = 0; ii < slist.size(); ii++)
		{
			const Wt::WString wsDesc(slist[ii]);
			cbDesc->addItem(wsDesc);
			auto uniqueTemp = make_unique<Wt::WTreeNode>(wsDesc);
			nextPrompt[2] = slist[ii];
			function<void()> fn = bind(&SCDAwidget::setLayer, this, Description, nextPrompt);
			uniqueTemp->expanded().connect(fn);
			pTemp = pYears[treeActive[1]]->addChildNode(move(uniqueTemp));
			pTemp->addChildNode(make_unique<Wt::WTreeNode>("Loading..."));
		}
		pYears[treeActive[1]]->expand();
		cbDesc->setEnabled(1);
		activeYear = nextPrompt[1];
		break;
	case 4:  // Set the description layer.
		wtable = event.get_wtable();
		timer = jf.timerRestart();
		jf.logTime("get_wtable", timer);
		pYears = treeRoot->childNodes();
		pDescs = pYears[treeActive[1]]->childNodes();
		pRegions = pDescs[treeActive[2]]->childNodes();
		while (pRegions.size() > 0)
		{
			pDescs[treeActive[2]]->removeChildNode(pRegions[0]);
			pRegions.erase(pRegions.begin());
		}
		wsYear = pYears[treeActive[1]]->label()->text();
		wsDesc = pDescs[treeActive[2]]->label()->text();
		nextPrompt.resize(4);
		nextPrompt[0] = sessionID;
		nextPrompt[1] = wsYear.toUTF8();
		nextPrompt[2] = wsDesc.toUTF8();
		timer = jf.timerRestart();
		jf.logTime("misc before widgets", timer);
		cbRegion->clear();
		cbRegion->addItem(wsAll);
		inum = 0;
		for (int ii = 0; ii < wtable.size(); ii++)
		{
			if (wtable[ii][2].size() < 1)
			{
				mapW.emplace(wtable[ii][0], inum);
				const Wt::WString wsRegion(wtable[ii][1]);
				cbRegion->addItem(wsRegion);
				auto uniqueTemp = make_unique<Wt::WTreeNode>(wsRegion);
				pTemp = pDescs[treeActive[2]]->addChildNode(move(uniqueTemp));
			}
		}
		timer = jf.timerRestart();
		jf.logTime("add region roots", timer);
		pRegions = pDescs[treeActive[2]]->childNodes();
		for (int ii = 0; ii < wtable.size(); ii++)
		{
			if (wtable[ii][2].size() > 0)
			{
				try	{ inum = mapW.at(wtable[ii][2]); }
				catch (out_of_range& oor) { jf.err("mapW-SCDAwidget.processDataEvent"); }
				const Wt::WString wsRegion(wtable[ii][1]);
				cbRegion->addItem(wsRegion);
				auto uniqueTemp = make_unique<Wt::WTreeNode>(wsRegion);
				nextPrompt[3] = jf.utf16to8(wtable[ii][1]);
				function<void()> fn = bind(&SCDAwidget::setLayer, this, Region, nextPrompt);
				uniqueTemp->expanded().connect(fn);
				pTemp = pRegions[inum]->addChildNode(move(uniqueTemp));
				pTemp->addChildNode(make_unique<Wt::WTreeNode>("Loading..."));
			}
		}
		timer = jf.timerRestart();
		jf.logTime("add secondary regions", timer);
		pDescs[treeActive[2]]->expand();
		for (int ii = 0; ii < pRegions.size(); ii++)
		{
			pRegions[ii]->expand();
		}
		cbRegion->setEnabled(1);
		activeDesc = nextPrompt[2];
		break;
	}

	timer = jf.timerStop();
	jf.logTime("processDataEvent#" + to_string(etype), timer);
}
void SCDAwidget::setLayer(Layer layer, vector<string> prompt)
{
	jf.timerStart();
	sRef.pullLayer(layer, prompt);
	treeActive.clear();
	treeActive.resize(layer + 1);
	Wt::WString wsTemp, wsYear, wsDesc;
	vector<Wt::WTreeNode*> pYears, pDescs;
	for (int ii = 0; ii <= layer; ii++)
	{
		switch (ii)
		{
		case 0:  // set Root
			treeActive[0] = 0;
			break;
		case 1:  // set Year
			wsYear = Wt::WString::fromUTF8(prompt[1]);
			pYears = treeRoot->childNodes();
			for (int jj = 0; jj < pYears.size(); jj++)
			{
				wsTemp = pYears[jj]->label()->text();
				if (wsTemp == wsYear)
				{
					pYears[jj]->setHidden(0);
					pYears[jj]->expand();
					treeActive[1] = jj;
					break;
				}
			}
			panelDesc->setTitle(defNames[1] + " (" + wsYear + ")");
			break;
		case 2:  // set Desc
			wsDesc = Wt::WString::fromUTF8(prompt[2]);
			pDescs = pYears[treeActive[1]]->childNodes();
			for (int jj = 0; jj < pDescs.size(); jj++)
			{
				wsTemp = pDescs[jj]->label()->text();
				if (wsTemp == wsDesc)
				{
					pDescs[jj]->setHidden(0);
					pDescs[jj]->expand();
					treeActive[2] = jj;
					break;
				}
			}
			panelRegion->setTitle(defNames[2] + " (" + wsDesc + ")");
			break;
		}
	}
	long long timer = jf.timerStop();
	jf.logTime("setLayer(" + to_string(layer) + ")", timer);
}
void SCDAwidget::setTable(vector<string> prompt)
{
	sRef.pullTable(prompt);
}
