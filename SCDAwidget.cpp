#include "SCDAwidget.h"

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
void SCDAwidget::cbDivClicked()
{
	Wt::WString wsTemp;
	Wt::WString wsDiv = cbDiv->currentText();
	int yearIndex, descIndex, region1Index, region2Index;
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

	Wt::WString wsDesc = Wt::WString::fromUTF8(activeDesc);
	vector<Wt::WTreeNode*> pDescs = pYears[yearIndex]->childNodes();
	for (int ii = 0; ii < pDescs.size(); ii++)
	{
		wsTemp = pDescs[ii]->label()->text();
		if (wsTemp == wsDesc)
		{
			descIndex = ii;
			break;
		}
	}

	vector<Wt::WTreeNode*> pRegion1s = pDescs[descIndex]->childNodes();
	region1Index = treeActive[3];
	vector<Wt::WTreeNode*> pRegion2s = pRegion1s[region1Index]->childNodes();
	region2Index = treeActive[4];
	Wt::WString wsRegion = pRegion2s[region2Index]->label()->text();

	wstring wtemp = wsDiv.value();
	activeDiv = jf.utf16to8(wtemp);

	vector<vector<Wt::WTreeNode*>> pDivs;

	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt = { app->sessionId() };

	if (wsDiv == wsAll)
	{
		activeDiv = "All";
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
			if (ii != descIndex)
			{
				pDescs[ii]->collapse();
			}
			else
			{
				pDescs[ii]->expand();
				pDescs[ii]->setHidden(0);
			}
		}
		for (int ii = 0; ii < pRegion1s.size(); ii++)
		{
			if (ii != region1Index)
			{
				pRegion1s[ii]->collapse();
			}
			else
			{
				pRegion1s[ii]->expand();
				pRegion1s[ii]->setHidden(0);
			}
		}
		for (int ii = 0; ii < pRegion2s.size(); ii++)
		{
			pRegion2s[ii]->setHidden(0);
			pRegion2s[ii]->collapse();
		}
	}
	else
	{
		prompt.resize(4);
		prompt[1] = activeYear;
		prompt[2] = activeDesc;
		prompt[3] = activeDiv;
		setLayer(Division, prompt);

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
			if (ii != descIndex)
			{
				pDescs[ii]->setHidden(1);
			}
			else
			{
				pDescs[ii]->expand();
				pDescs[ii]->setHidden(0);
			}
		}
		for (int ii = 0; ii < pRegion1s.size(); ii++)
		{
			if (ii != region1Index)
			{
				pRegion1s[ii]->setHidden(1);
			}
			else
			{
				pRegion1s[ii]->expand();
				pRegion1s[ii]->setHidden(0);
			}
		}
		for (int ii = 0; ii < pRegion2s.size(); ii++)
		{
			if (ii != region2Index)
			{
				pRegion2s[ii]->setHidden(1);
			}
			else
			{
				pRegion2s[ii]->expand();
				pRegion2s[ii]->setHidden(0);
			}
		}


	}
}
void SCDAwidget::cbRegionClicked()
{
	bool letmeout = 0;
	string temp;
	wstring wtemp;
	Wt::WString wsTemp;
	Wt::WString wsRegion = cbRegion->currentText();
	int yearIndex, descIndex, region1Index, region2Index;
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

	Wt::WString wsDesc = Wt::WString::fromUTF8(activeDesc);
	vector<Wt::WTreeNode*> pDescs = pYears[yearIndex]->childNodes();
	for (int ii = 0; ii < pDescs.size(); ii++)
	{
		wsTemp = pDescs[ii]->label()->text();
		if (wsTemp == wsDesc)
		{
			descIndex = ii;
			break;
		}
	}

	vector<Wt::WTreeNode*> pRegion1s = pDescs[descIndex]->childNodes();
	vector<vector<Wt::WTreeNode*>> pRegion2s;
	for (int ii = 0; ii < pRegion1s.size(); ii++)
	{
		pRegion2s.push_back(pRegion1s[ii]->childNodes());
	}
	for (int ii = 0; ii < pRegion2s.size(); ii++)
	{
		wsTemp = pRegion1s[ii]->label()->text();
		if (wsTemp == wsRegion)
		{
			region1Index = ii;
			region2Index = -1;
			break;
		}
		for (int jj = 0; jj < pRegion2s[ii].size(); jj++)
		{
			wsTemp = pRegion2s[ii][jj]->label()->text();
			if (wsTemp == wsRegion)
			{
				region1Index = ii;
				region2Index = jj;
				letmeout = 1;
				break;
			}
		}
		if (letmeout) { break; }
	}
	letmeout = 0;

	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt = { app->sessionId() };

	if (wsRegion == wsAll)
	{
		activeRegion = "All";
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
			if (ii != descIndex)
			{
				pDescs[ii]->collapse();
			}
			else
			{
				pDescs[ii]->expand();
				pDescs[ii]->setHidden(0);
			}
		}
		for (int ii = 0; ii < pRegion1s.size(); ii++)
		{
			pRegion1s[ii]->expand();
			pRegion1s[ii]->setHidden(0);
		}
		for (int ii = 0; ii < pRegion2s.size(); ii++)
		{
			for (int jj = 0; jj < pRegion2s[ii].size(); jj++)
			{
				pRegion2s[ii][jj]->setHidden(0);
				pRegion2s[ii][jj]->collapse();
			}
		}
		panelDiv->setTitle(defNames[3]);
		cbDiv->setCurrentIndex(0);
		cbDiv->setEnabled(0);
	}
	else if (region2Index < 0)
	{
		for (int ii = 0; ii < pRegion1s.size(); ii++)
		{
			if (ii == region1Index)
			{
				pRegion1s[ii]->setHidden(0);
				pRegion1s[ii]->expand();
				for (int jj = 0; jj < pRegion2s[region1Index].size(); jj++)
				{
					pRegion2s[region1Index][jj]->setHidden(0);
					pRegion2s[region1Index][jj]->collapse();
				}
			}
			else
			{
				pRegion1s[ii]->setHidden(1);
			}
		}
		wtemp = wsRegion.value();
		activeRegion = jf.utf16to8(wtemp);
	}
	else
	{
		prompt.resize(4);
		prompt[1] = activeYear;
		prompt[2] = activeDesc;
		temp = wsRegion.toUTF8();
		wtemp = wsRegion.value();
		prompt[3] = jf.utf16to8(wtemp);
		setLayer(Region, prompt);
		activeRegion = prompt[3];

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
			if (ii != descIndex)
			{
				pDescs[ii]->setHidden(1);
			}
			else
			{
				pDescs[ii]->expand();
				pDescs[ii]->setHidden(0);
			}
		}
		for (int ii = 0; ii < pRegion1s.size(); ii++)
		{
			if (ii != region1Index)
			{
				pRegion1s[ii]->setHidden(1);
			}
			else
			{
				pRegion1s[ii]->expand();
				pRegion1s[ii]->setHidden(0);
			}
		}
		for (int ii = 0; ii < pRegion2s[region1Index].size(); ii++)
		{
			if (region2Index < 0)
			{
				pRegion2s[region1Index][ii]->collapse();
				pRegion2s[region1Index][ii]->setHidden(0);
			}
			else if (ii != region2Index)
			{
				pRegion2s[region1Index][ii]->setHidden(1);
			}
			else
			{
				pRegion2s[region1Index][ii]->expand();
				pRegion2s[region1Index][ii]->setHidden(0);
			}
		}
	}
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
void SCDAwidget::folderClicked(Wt::WString wsFolder)
{
	selectedFolder = wsFolder;
	if (selectedRow >= 0)
	{
		int numCol = wtTable->columnCount();
		for (int ii = 0; ii < numCol; ii++)
		{
			wtTable->elementAt(selectedRow, ii)->decorationStyle().setBackgroundColor(colourWhite);
		}
		selectedRow = -1;
	}
	pbTable->setEnabled(0);
	pbMap->setEnabled(0);
}
void SCDAwidget::init()
{
	//sRef.init();
	connect();
	jt.init("Census Tables", sroot);
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt = { app->sessionId() };

	defNames.resize(4);
	defNames[0] = Wt::WString::tr("cbYear");
	defNames[1] = Wt::WString::tr("cbDesc");
	defNames[2] = Wt::WString::tr("cbRegion");
	defNames[3] = Wt::WString::tr("cbDivision");
	
	makeUI();
	initUI();
	setLayer(Root, prompt);
}
void SCDAwidget::initUI()
{
	Wt::WString wstemp;
	string temp;

	auto lenAuto = Wt::WLength();
	auto len1200 = Wt::WLength("1200px");
	auto len825 = Wt::WLength("825px");
	auto len800 = Wt::WLength("800px");
	auto len700 = Wt::WLength("700px");
	auto len600 = Wt::WLength("600px");
	auto len500 = Wt::WLength("500px");
	auto len400 = Wt::WLength("400px");
	auto len300 = Wt::WLength("300px");
	auto len275 = Wt::WLength("275px");
	auto len200 = Wt::WLength("200px");
	auto len100 = Wt::WLength("100px");
	auto len75 = Wt::WLength("75px");
	auto len50 = Wt::WLength("50px");
	auto len35 = Wt::WLength("35px");

	// Box sizes.
	boxControl->resize(len300, len800);
	boxData->resize(len825, len800);
	boxTest->resize(lenAuto, len50);
	boxLoad->resize(lenAuto, len50);
	boxSearch->resize(lenAuto, len50);

	// Colourful things.
	colourSelected.setRgb(200, 200, 255, 50);
	colourWhite.setRgb(255, 255, 255, 255);

	// Initial values for panelYear.
	panelYear->setTitle(defNames[0]);
	panelYear->resize(len275, len100);
	cbYear->changed().connect(this, &SCDAwidget::cbYearClicked);

	// Initial values for panelDesc.
	panelDesc->setTitle(defNames[1]);
	panelDesc->resize(len275, len100);
	cbDesc->changed().connect(this, &SCDAwidget::cbDescClicked);
	cbDesc->addItem(wsAll);
	cbDesc->setEnabled(0);

	// Initial values for panelRegion.
	panelRegion->setTitle(defNames[2]);
	panelRegion->resize(len275, len300);
	cbRegion->changed().connect(this, &SCDAwidget::cbRegionClicked);
	cbRegion->addItem(wsAll);
	cbRegion->setEnabled(0);

	// Initial values for panelDivision.
	/*
	panelDiv->setTitle(defNames[3]);
	cbDiv->changed().connect(this, &SCDAwidget::cbDivClicked);
	cbDiv->addItem(wsAll);
	cbDiv->setEnabled(0);
	*/

	// Initial values for the tab widget.
	tabData->resize(len800, len700);

	// Initial values for the tree widget.
	wstemp = Wt::WString("Census Tables");
	treeCata->setSelectionMode(Wt::SelectionMode::Single);
	auto tRoot = make_unique<Wt::WTreeNode>(wstemp);
	function<void()> fn = bind(&SCDAwidget::folderClicked, this, wstemp);
	tRoot->selected().connect(fn);
	treeRoot = tRoot.get();
	treeCata->setTreeRoot(move(tRoot));

	// Initial values for the buttons.
	pbTest->clicked().connect(this, &SCDAwidget::pbTestClicked);
	pbTable->clicked().connect(this, &SCDAwidget::pbTableClicked);
	pbTable->setEnabled(0);
	pbMap->clicked().connect(this, &SCDAwidget::pbMapClicked);
	pbMap->setEnabled(0);
	pbSearch->setMinimumSize(len100, len50);
	pbTest->resize(len75, len35);

	// Initial values for the map widget.
	wtMap->initSize(800.0, 570.0);
	wtMap->clicked().connect(this, &SCDAwidget::mouseMapClick);
}
void SCDAwidget::makeUI()
{
	this->clear();
	auto hLayout = make_unique<Wt::WHBoxLayout>();
	auto uniqueBoxControl = make_unique<Wt::WContainerWidget>();
	boxControl = uniqueBoxControl.get();
	auto uniqueBoxData = make_unique<Wt::WContainerWidget>();
	boxData = uniqueBoxData.get();

	auto vControlLayout = make_unique<Wt::WVBoxLayout>();
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
	auto uniqueBoxTest = make_unique<Wt::WContainerWidget>();
	boxTest = uniqueBoxTest.get();
	auto uniqueBoxLoad = make_unique<Wt::WContainerWidget>();
	boxLoad = uniqueBoxLoad.get();

	auto vDataLayout = make_unique<Wt::WVBoxLayout>();
	auto uniqueTabData = make_unique<Wt::WTabWidget>();
	tabData = uniqueTabData.get();
	auto uniqueTreeRegion = make_unique<Wt::WTree>();
	treeCata = uniqueTreeRegion.get();
	auto uniqueBoxTable = make_unique<Wt::WContainerWidget>();
	boxTable = uniqueBoxTable.get();
	auto uniqueBoxSearch = make_unique<Wt::WContainerWidget>();
	boxSearch = uniqueBoxSearch.get();
	auto uniqueWtMap = make_unique<WTFUNC>(commMap);
	wtMap = uniqueWtMap.get();
	auto uniqueListSearch = make_unique<Wt::WSelectionBox>();
	sbList = uniqueListSearch.get();

	auto hTestLayout = make_unique<Wt::WHBoxLayout>();
	auto uniqueLineEditTest = make_unique<Wt::WLineEdit>();
	lineEditTest = uniqueLineEditTest.get();
	auto uniquePbTest = make_unique<Wt::WPushButton>("TEST");
	pbTest = uniquePbTest.get();

	auto hLoadLayout = make_unique<Wt::WHBoxLayout>();
	auto uniquePbTable = make_unique<Wt::WPushButton>("LOAD \n TABLE");
	pbTable = uniquePbTable.get();
	auto uniquePbMap = make_unique<Wt::WPushButton>("LOAD \n MAP");
	pbMap = uniquePbMap.get();

	auto vTableLayout = make_unique<Wt::WVBoxLayout>();
	auto uniqueTextTable = make_unique<Wt::WText>();
	textTable = uniqueTextTable.get();
	auto uniqueTableRegion = make_unique<Wt::WTable>();
	wtTable = uniqueTableRegion.get();

	auto hSearchLayout = make_unique<Wt::WHBoxLayout>();
	auto uniqueTextMessage = make_unique<Wt::WText>("Selected region:\nNone");
	textMessage = uniqueTextMessage.get();
	auto uniqueLineEditSearch = make_unique<Wt::WLineEdit>();
	lineEditSearch = uniqueLineEditSearch.get();
	auto uniquePbSearch = make_unique<Wt::WPushButton>("SEARCH");
	pbSearch = uniquePbSearch.get();

	//uniquePanelDivision->setCentralWidget(move(uniqueCbDivision));
	//uniqueBoxControl->addWidget(move(uniquePanelDivision));

	uniquePanelYear->setCentralWidget(move(uniqueCbYear));
	uniquePanelDesc->setCentralWidget(move(uniqueCbDesc));
	uniquePanelRegion->setCentralWidget(move(uniqueCbRegion));

	hTestLayout->addWidget(move(uniqueLineEditTest));
	hTestLayout->addWidget(move(uniquePbTest));
	uniqueBoxTest->setLayout(move(hTestLayout));

	hLoadLayout->addWidget(move(uniquePbTable));
	hLoadLayout->addWidget(move(uniquePbMap));
	uniqueBoxLoad->setLayout(move(hLoadLayout));

	vTableLayout->addWidget(move(uniqueTextTable));
	vTableLayout->addWidget(move(uniqueTableRegion));
	uniqueBoxTable->setLayout(move(vTableLayout));

	uniqueTabData->addTab(move(uniqueTreeRegion), "Regions");
	uniqueTabData->addTab(move(uniqueBoxTable), "Data");
	uniqueTabData->addTab(move(uniqueWtMap), "Map");
	uniqueTabData->addTab(move(uniqueListSearch), "List");

	hSearchLayout->addWidget(move(uniqueTextMessage));
	hSearchLayout->addWidget(move(uniqueLineEditSearch));
	hSearchLayout->addWidget(move(uniquePbSearch));
	uniqueBoxSearch->setLayout(move(hSearchLayout));

	vControlLayout->addWidget(move(uniquePanelYear));
	vControlLayout->addWidget(move(uniquePanelDesc));
	vControlLayout->addWidget(move(uniquePanelRegion));
	vControlLayout->addWidget(move(uniqueBoxTest));
	vControlLayout->addWidget(move(uniqueBoxLoad));
	uniqueBoxControl->setLayout(move(vControlLayout));

	vDataLayout->addWidget(move(uniqueTabData));
	vDataLayout->addWidget(move(uniqueBoxSearch));
	uniqueBoxData->setLayout(move(vDataLayout));

	hLayout->addWidget(move(uniqueBoxControl));
	hLayout->addWidget(move(uniqueBoxData));
	this->setLayout(move(hLayout));
}
void SCDAwidget::mouseMapClick(const Wt::WMouseEvent& e)
{
	auto coord = e.widget();
	cout << "(" << to_string(coord.x) << "," << to_string(coord.y) << ")" << endl;
	Wt::WLength wlTemp = Wt::WLength(wtMap->width());
	double widthD = wlTemp.value();
	cout << "Widget width: " << to_string(widthD) << endl;
	int patience = 5;
	while (patience > 0)
	{
		this_thread::sleep_for(20ms);
		m_map.lock();
		if (commMap.size() > 0)
		{
			mapRegion = commMap[0];
			commMap.clear();
			patience = -1;
		}
		m_map.unlock();
		patience--;
	}
	if (patience < 0)
	{
		size_t posCan = mapRegion.rfind("(Canada)");
		string sTemp;
		if (posCan < mapRegion.size())
		{
			sTemp = mapRegion.substr(0, posCan);
		}
		else { sTemp = mapRegion; }
		Wt::WString wsTemp = "Selected region:\n" + Wt::WString::fromUTF8(sTemp);
		textMessage->setText(wsTemp);
	}
}
void SCDAwidget::pbMapClicked()
{
	Wt::WLength len = wtMap->width();
	double width = len.value();
	len = wtMap->height();
	double height = len.value();
	Wt::WText *wtTemp = dynamic_cast<Wt::WText*>(wtTable->elementAt(0, selectedCell[1])->widget(0));
	Wt::WString wsColTitle = wtTemp->text();
	string colTitle = wsColTitle.toUTF8();
	wtTemp = dynamic_cast<Wt::WText*>(wtTable->elementAt(selectedCell[0], 0)->widget(0));
	Wt::WString wsRowTitle = wtTemp->text();
	string rowTitle = wsRowTitle.toUTF8();
	vector<string> prompt(6);
	Wt::WApplication* app = Wt::WApplication::instance();
	prompt[0] = app->sessionId();
	prompt[1] = activeDesc;
	prompt[2] = "Canada";  // TEMPORARY.
	prompt[3] = to_string(width) + "," + to_string(height);
	prompt[4] = wsRowTitle.toUTF8();
	prompt[5] = wsColTitle.toUTF8();
	sRef.pullMap(prompt);
	tabData->setCurrentIndex(2);
}
void SCDAwidget::pbSearchClicked()
{
	// RESUME HERE. You know what to do. 
}
void SCDAwidget::pbTableClicked()
{
	vector<string> prompt(3);
	Wt::WApplication* app = Wt::WApplication::instance();
	prompt[0] = app->sessionId();
	prompt[1] = activeDesc;
	prompt[2] = selectedRegion.toUTF8();
	sRef.pullRegion(prompt);
	wtTable->clear();
	string title = "Census Region Data for " + prompt[2];
	Wt::WString wsTable = Wt::WString::fromUTF8(title);
	textTable->setText(title);
	tabData->setCurrentIndex(1);
}
void SCDAwidget::pbTestClicked()
{
	vector<string> prompt(2);
	Wt::WApplication* app = Wt::WApplication::instance();
	prompt[0] = app->sessionId();
	Wt::WString wsTable = lineEditTest->text();
	prompt[1] = wsTable.toUTF8();
	if (prompt[1].size() < 1)
	{
		prompt[1] = "sqlite_master";
	}
	setTable(prompt);
}
void SCDAwidget::processDataEvent(const DataEvent& event)
{
	jf.timerStart();
	Wt::WApplication* app = Wt::WApplication::instance();
	app->triggerUpdate();
	vector<Wt::WTreeNode*> pYears, pDescs, pRegion1s, pRegion2s, pDivs;
	vector<vector<Wt::WTreeNode*>> pTemps;
	vector<vector<Wt::WPointF>> areas;
	Wt::WTreeNode* pParent = nullptr;
	Wt::WTreeNode* pTemp = nullptr;
	Wt::WString wsYear, wsDesc, wsRegion;
	wstring wtemp;
	string sessionID = app->sessionId(), nameAreaSel;
	vector<string> slist, nextPrompt;
	vector<wstring> wlist;
	vector<vector<wstring>> wtable;
	unordered_map<wstring, int> mapW;
	long long timer;
	vector<int> tempIndex;
	vector<double> regionData;
	int inum, maxCol;

	int etype = event.type();
	switch (etype)
	{
	case 0:  // Connect.
		break;
	case 1:  // Populate the table widget.
	{
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
				function<void()> fn = bind(&SCDAwidget::selectTableCell, this, ii, jj);
				cellText->clicked().connect(fn);		
				wtTable->elementAt(ii, jj)->addWidget(move(cellText));
			}
		}
		selectedRow = -1;
		pbMap->setEnabled(0);
		break;
	}
	case 2:  // Display the map on the painter widget.
	{
		slist = event.get_list();
		areas = event.get_areas();
		regionData = event.get_regionData();		
		wtMap->drawMap(areas, slist, regionData);
		slist.pop_back();
		updateMapRegionList(slist, mapRegionList, 1);
		break;
	}
	case 3:  // Set the root layer.
	{
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
			function<void()> fn2 = bind(&SCDAwidget::folderClicked, this, wsYear);
			uniqueTemp->selected().connect(fn2);
			pTemp = treeRoot->addChildNode(move(uniqueTemp));
			pTemp->addChildNode(make_unique<Wt::WTreeNode>("Loading..."));
		}
		treeRoot->expand();
		break;
	}
	case 4:  // Set the year layer.
	{
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
		wtemp = wsYear.value();
		nextPrompt[1] = jf.utf16to8(wtemp);
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
			function<void()> fn2 = bind(&SCDAwidget::folderClicked, this, wsDesc);
			uniqueTemp->selected().connect(fn2);
			pTemp = pYears[treeActive[1]]->addChildNode(move(uniqueTemp));
			pTemp->addChildNode(make_unique<Wt::WTreeNode>("Loading..."));
		}
		pYears[treeActive[1]]->expand();
		cbDesc->setEnabled(1);
		activeYear = nextPrompt[1];
		break;
	}
	case 5:  // Set the description layer.
	{
		wtable = event.get_wtable();
		timer = jf.timerRestart();
		jf.logTime("get_wtable", timer);
		pYears = treeRoot->childNodes();
		pDescs = pYears[treeActive[1]]->childNodes();
		pRegion1s = pDescs[treeActive[2]]->childNodes();
		while (pRegion1s.size() > 0)
		{
			pDescs[treeActive[2]]->removeChildNode(pRegion1s[0]);
			pRegion1s.erase(pRegion1s.begin());
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
		tempIndex.push_back(0);
		for (int ii = 0; ii < wtable.size(); ii++)
		{
			if (wtable[ii].size() < 3)
			{
				mapW.emplace(wtable[ii][0], tempIndex[0]);
				tempIndex[0]++;
				const Wt::WString wsRegion(wtable[ii][1]);
				cbRegion->addItem(wsRegion);
				auto uniqueTemp = make_unique<Wt::WTreeNode>(wsRegion);
				uniqueTemp->setSelectable(1);
				function<void()> fn = bind(&SCDAwidget::tableClicked, this, wsRegion);
				uniqueTemp->selected().connect(fn);
				pTemp = pDescs[treeActive[2]]->addChildNode(move(uniqueTemp));
			}
		}
		timer = jf.timerRestart();
		jf.logTime("add region roots", timer);
		pRegion1s = pDescs[treeActive[2]]->childNodes();
		for (int ii = 0; ii < wtable.size(); ii++)
		{
			if (wtable[ii].size() > 2)
			{
				try { inum = mapW.at(wtable[ii][2]); }
				catch (out_of_range& oor) { jf.err("mapW-SCDAwidget.processDataEvent"); }
				const Wt::WString wsRegion(wtable[ii][1]);
				cbRegion->addItem(wsRegion);
				auto uniqueTemp = make_unique<Wt::WTreeNode>(wsRegion);
				nextPrompt[3] = jf.utf16to8(wtable[ii][1]);
				function<void()> fn = bind(&SCDAwidget::setLayer, this, Region, nextPrompt);
				uniqueTemp->expanded().connect(fn);
				uniqueTemp->setSelectable(1);
				function<void()> fn2 = bind(&SCDAwidget::tableClicked, this, wsRegion);
				uniqueTemp->selected().connect(fn2);
				pTemp = pRegion1s[inum]->addChildNode(move(uniqueTemp));
				pTemp->addChildNode(make_unique<Wt::WTreeNode>("Loading..."));
			}
		}
		timer = jf.timerRestart();
		jf.logTime("add secondary regions", timer);
		pDescs[treeActive[2]]->expand();
		for (int ii = 0; ii < pRegion1s.size(); ii++)
		{
			pRegion1s[ii]->expand();
		}
		cbRegion->setEnabled(1);
		activeDesc = nextPrompt[2];
		break;
	}
	case 6:  // Set the region layer.
	{
		// wtable columns:  GID, Region Name, param4, param5, ...
		wtable = event.get_wtable();
		timer = jf.timerRestart();
		jf.logTime("get_wtable", timer);
		pYears = treeRoot->childNodes();
		pDescs = pYears[treeActive[1]]->childNodes();
		pRegion1s = pDescs[treeActive[2]]->childNodes();
		pRegion2s = pRegion1s[treeActive[3]]->childNodes();
		pDivs = pRegion2s[treeActive[4]]->childNodes();
		while (pDivs.size() > 0)
		{
			pRegion2s[treeActive[4]]->removeChildNode(pDivs[0]);
			pDivs.erase(pDivs.begin());
		}
		wsYear = pYears[treeActive[1]]->label()->text();
		wsDesc = pDescs[treeActive[2]]->label()->text();
		wsRegion = pRegion2s[treeActive[4]]->label()->text();
		nextPrompt.resize(5);
		nextPrompt[0] = sessionID;
		nextPrompt[1] = wsYear.toUTF8();
		nextPrompt[2] = wsDesc.toUTF8();
		wtemp = wsRegion.value();
		nextPrompt[3] = jf.utf16to8(wtemp);
		timer = jf.timerRestart();
		jf.logTime("misc before widgets", timer);
		//cbDiv->clear();
		//cbDiv->addItem(wsAll);
		inum = 0;

		// Incorporate the first new layer, that uses prompt as root.
		tempIndex.push_back(0);
		pParent = pRegion2s[treeActive[4]];
		maxCol = jf.maxNumCol(wtable);
		if (maxCol == 2)  // If all entries are the final tree layer...
		{
			for (int ii = 0; ii < wtable.size(); ii++)
			{
				mapW.emplace(wtable[ii][0], tempIndex[0]);
				tempIndex[0]++;
				const Wt::WString wsDiv(wtable[ii][1]);
				//cbDiv->addItem(wsDiv);
				auto uniqueTemp = make_unique<Wt::WTreeNode>(wsDiv);
				function<void()> fn = bind(&SCDAwidget::tableClicked, this, wsDiv);
				uniqueTemp->selected().connect(fn);
				pTemp = pParent->addChildNode(move(uniqueTemp));
			}
			pTemps.push_back(pRegion2s[treeActive[4]]->childNodes());
			timer = jf.timerRestart();
			jf.logTime("add (single) region roots", timer);
		}
		else
		{
			for (int ii = 0; ii < wtable.size(); ii++)
			{
				if (wtable[ii].size() < 3)
				{
					mapW.emplace(wtable[ii][0], tempIndex[0]);
					tempIndex[0]++;
					const Wt::WString wsDiv(wtable[ii][1]);
					//cbDiv->addItem(wsDiv);
					auto uniqueTemp = make_unique<Wt::WTreeNode>(wsDiv);
					function<void()> fn = bind(&SCDAwidget::tableClicked, this, wsDiv);
					uniqueTemp->selected().connect(fn);
					pTemp = pParent->addChildNode(move(uniqueTemp));
				}
			}
			pTemps.push_back(pRegion2s[treeActive[4]]->childNodes());  // First generation of new parents.
			timer = jf.timerRestart();
			jf.logTime("add region roots", timer);

			// Incorporate all subsequent layers.
			for (int ii = 2; ii < maxCol; ii++)
			{
				tempIndex.push_back(0);
				for (int jj = 0; jj < wtable.size(); jj++)
				{
					if (wtable[jj].size() == ii + 1)
					{
						try { inum = mapW.at(wtable[jj][ii]); }
						catch (out_of_range& oor) { jf.err("mapW2-SCDAwidget.processDataEvent"); }
						pParent = pTemps[pTemps.size() - 1][inum];
						mapW.emplace(wtable[jj][0], tempIndex[tempIndex.size() - 1]);
						tempIndex[tempIndex.size() - 1]++;
						const Wt::WString wsDiv(wtable[jj][1]);
						//cbDiv->addItem(wsDiv);
						auto uniqueTemp = make_unique<Wt::WTreeNode>(wsDiv);
						function<void()> fn = bind(&SCDAwidget::tableClicked, this, wsDiv);
						uniqueTemp->selected().connect(fn);
						pTemp = pParent->addChildNode(move(uniqueTemp));
					}
				}
			}
			timer = jf.timerRestart();
			jf.logTime("add region leafs", timer);
		}

		pRegion2s[treeActive[4]]->expand();
		for (int ii = 0; ii < pTemps[0].size(); ii++)
		{
			pTemps[0][ii]->collapse();
		}
		//cbDiv->setEnabled(1);
		activeRegion = nextPrompt[3];
		break;
	}
	case 7:  // Set the division filter.
	{
		wlist = event.get_wtree_pl();
		pTemps.resize(wlist.size() - 2);
		pYears = treeRoot->childNodes();
		pDescs = pYears[treeActive[1]]->childNodes();
		pRegion1s = pDescs[treeActive[2]]->childNodes();
		pRegion2s = pRegion1s[treeActive[3]]->childNodes();
		pTemps[0] = pRegion2s[treeActive[4]]->childNodes();

		for (int ii = 0; ii < wlist.size() - 2; ii++)
		{
			for (int jj = 0; jj < pTemps[ii].size(); jj++)
			{
				wtemp = pTemps[ii][jj]->label()->text().value();
				if (wtemp == wlist[ii + 2])
				{
					tempIndex.push_back(jj);
					if (ii < wlist.size() - 3)
					{
						pTemps[ii + 1] = pTemps[ii][jj]->childNodes();
					}
					break;
				}
			}
		}
		for (int ii = 0; ii < pTemps.size(); ii++)
		{
			for (int jj = 0; jj < pTemps[ii].size(); ii++)
			{
				if (jj == tempIndex[ii])
				{
					pTemps[ii][jj]->setHidden(0);
					pTemps[ii][jj]->expand();
				}
				else
				{
					pTemps[ii][jj]->setHidden(1);
				}
			}
		}
		break;
	}
	}
	timer = jf.timerStop();
	jf.logTime("processDataEvent#" + to_string(etype), timer);
}
void SCDAwidget::selectTableCell(int iRow, int iCol)
{
	if (iRow < 1 || iCol < 1) { return; }
	int numCol = wtTable->columnCount();
	if (selectedCell[0] >= 0 && selectedCell[1] >= 0)
	{
		wtTable->elementAt(selectedCell[0], selectedCell[1])->decorationStyle().setBackgroundColor(colourWhite);
	}
	wtTable->elementAt(iRow, iCol)->decorationStyle().setBackgroundColor(colourSelected);
	selectedCell[0] = iRow;
	selectedCell[1] = iCol;
	if (pbTable->isEnabled()) { pbMap->setEnabled(1); }
}
void SCDAwidget::selectTableRow(int iRow)
{
	int numCol = wtTable->columnCount();
	if (selectedRow >= 0)
	{
		for (int ii = 0; ii < numCol; ii++)
		{
			wtTable->elementAt(selectedRow, ii)->decorationStyle().setBackgroundColor(colourWhite);
			wtTable->elementAt(iRow, ii)->decorationStyle().setBackgroundColor(colourSelected);
		}
	}
	else
	{
		for (int ii = 0; ii < numCol; ii++)
		{
			wtTable->elementAt(iRow, ii)->decorationStyle().setBackgroundColor(colourSelected);
		}
	}
	selectedRow = iRow;
	if (pbTable->isEnabled()) { pbMap->setEnabled(1); }
}
void SCDAwidget::setLayer(Layer layer, vector<string> prompt)
{
	sRef.pullLayer(layer, prompt);
	treeActive.clear();
	if (layer < 3)
	{
		treeActive.resize(layer + 1);
	}
	else
	{
		treeActive.resize(layer + 2);
	}
	Wt::WString wsTemp, wsYear, wsDesc, wsRegion;
	wstring wtemp1, wtemp2;
	vector<Wt::WTreeNode*> pYears, pDescs, pRegion1s, pRegion2s;
	bool letmeout = 0;
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
		case 3:  // set Region
			wtemp1 = jf.utf8to16(prompt[3]);
			wsRegion = Wt::WString(wtemp1);
			pRegion1s = pDescs[treeActive[2]]->childNodes();
			for (int jj = 0; jj < pRegion1s.size(); jj++)
			{
				pRegion2s = pRegion1s[jj]->childNodes();
				for (int kk = 0; kk < pRegion2s.size(); kk++)
				{
					wsTemp = pRegion2s[kk]->label()->text();
					wtemp2 = wsTemp.value();
					if (wtemp1 == wtemp2)
					{
						pRegion1s[jj]->setHidden(0);
						pRegion1s[jj]->expand();
						pRegion2s[kk]->setHidden(0);
						pRegion2s[kk]->expand();
						treeActive[3] = jj;
						treeActive[4] = kk;
						break;
					}
				}
				if (letmeout) { break; }
			}
			//panelDiv->setTitle(defNames[3] + " (" + wsRegion + ")");
			letmeout = 0;
			break;
		case 4:  // set Division
			break;
		}
	}

}
void SCDAwidget::setTable(vector<string> prompt)
{
	sRef.pullTable(prompt);
}
void SCDAwidget::tableClicked(Wt::WString wsTable)
{
	selectedRegion = wsTable;
	pbTable->setEnabled(1);
	if (selectedRow >= 0) { pbMap->setEnabled(1); }
}
void SCDAwidget::updateMapRegionList(vector<string>& sList, vector<Wt::WString>& wsList, int mode)
{
	// Mode 0 = standard translation, mode 1 = ignore first sList entry.
	wsList.clear();
	switch (mode)
	{
	case 1:
	{
		wsList.resize(sList.size() - 1);
		for (int ii = 0; ii < wsList.size(); ii++)
		{
			wsList[ii] = Wt::WString::fromUTF8(sList[ii + 1]);
		}
	}
	}

}
