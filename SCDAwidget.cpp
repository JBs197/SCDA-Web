#include "SCDAwidget.h"

void SCDAwidget::addVariable(vector<string>& vsVariable)
{
	// This variant is for known values. vsVariable form [title, MID].
	if (vsVariable.size() != 2) { jf.err("Missing prompt-SCDAwidget.addVariable"); }
	Wt::WString wTemp;
	string temp;
	int titleIndex = -1, MIDIndex = -1;
	vector<string> vsTitle(vvsParameter.size());
	for (int ii = 0; ii < vvsParameter.size(); ii++)
	{
		vsTitle[ii] = vvsParameter[ii].back();
		if (vsTitle[ii] == vsVariable[0]) { titleIndex = ii; }
	}
	if (titleIndex < 0) { jf.err("Parameter title not found-SCDAwidget.addVariable"); }
	vector<string> vsMID(vvsParameter[titleIndex].size() - 1);
	for (int ii = 0; ii < vsMID.size(); ii++)
	{
		vsMID[ii] = vvsParameter[titleIndex][ii];
		if (vsMID[ii] == vsVariable[1]) { MIDIndex = ii; }
	}
	if (MIDIndex < 0) { jf.err("Parameter MID not found-SCDAwidget.addVariable"); }

	int index = varPanel.size();
	auto varPanelUnique = make_unique<Wt::WPanel>();
	varPanel.push_back(varPanelUnique.get());
	temp = varPanelUnique->id();
	mapVarIndex.emplace(temp, index);
	auto varBoxUnique = make_unique<Wt::WContainerWidget>();
	auto varVLayoutUnique = make_unique<Wt::WVBoxLayout>();

	int indexTest = varTitle.size();
	if (indexTest != index) { jf.err("varTitle size mismatch-SCDAwidget.addVariable"); }
	auto varCBTitleUnique = make_unique<Wt::WComboBox>();
	varTitle.push_back(varCBTitleUnique.get());
	temp = varCBTitleUnique->id();
	mapVarIndex.emplace(temp, index);
	function<void()> fnVarTitle = bind(&SCDAwidget::cbVarTitleClicked, this, temp);
	varTitle[index]->changed().connect(fnVarTitle);

	indexTest = varMID.size();
	if (indexTest != index) { jf.err("varMID size mismatch-SCDAwidget.addVariable"); }
	auto varCBMIDUnique = make_unique<Wt::WComboBox>();
	varMID.push_back(varCBMIDUnique.get());
	temp = varCBMIDUnique->id();
	mapVarIndex.emplace(temp, index);
	function<void()> fnVarMID = bind(&SCDAwidget::cbVarMIDClicked, this, temp);
	varMID[index]->changed().connect(fnVarMID);

	varPanel[index]->setTitle("Parameter");
	cbRenew(varTitle[index], vsTitle);
	varTitle[index]->setCurrentIndex(titleIndex);
	cbRenew(varMID[index], vsMID);
	varMID[index]->setCurrentIndex(MIDIndex);

	varVLayoutUnique->addWidget(move(varCBTitleUnique));
	varVLayoutUnique->addWidget(move(varCBMIDUnique));
	varBoxUnique->setLayout(move(varVLayoutUnique));
	varPanelUnique->setCentralWidget(move(varBoxUnique));
	layoutConfig->addWidget(move(varPanelUnique));
}
void SCDAwidget::addVariable(vector<vector<string>>& vvsCandidate)
{
	// This variant is for undetermined possibilities. 
	// vvsCandidate form [variable index][
}
void SCDAwidget::cbCategoryClicked()
{
	// Populate tableData with its "input form", containing options for the user
	// to choose column and row topics. 
	resetVariables();
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(5);
	activeColTopic = "*";
	activeRowTopic = "*";
	int index = cbCategory->currentIndex();
	if (index == 0) 
	{ 
		activeCategory = "*"; 
		panelColTopic->setHidden(1);
		panelRowTopic->setHidden(1);
		return;
	}
	else
	{
		Wt::WString wsTemp = cbCategory->currentText();
		activeCategory = wsTemp.toUTF8();
	}
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCategory;
	prompt[3] = "*";  // Column topic.
	prompt[4] = "*";  // Row topic.
	sRef.pullTopic(prompt);
	panelColTopic->setHidden(0);
	panelRowTopic->setHidden(0);
}
void SCDAwidget::cbColRowClicked(string id)
{
	// When a column or row topic is specified, obtain an updated listing for
	// its opposite (column/row), and for its mirror (side panel/input table).
	resetVariables();
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(5);
	Wt::WString wTemp;
	int index;
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCategory;
	if (id == "panelCol")
	{
		index = cbColTopic->currentIndex();
		if (index == 0)
		{
			activeColTopic = "*";
			activeRowTopic = "*";
		}
		else
		{
			wTemp = cbColTopic->currentText();
			activeColTopic = wTemp.toUTF8();
		}
	}
	else if (id == "panelRow")
	{
		index = cbRowTopic->currentIndex();
		if (index == 0)
		{
			activeRowTopic = "*";
			activeColTopic = "*";
		}
		else
		{
			wTemp = cbRowTopic->currentText();
			activeRowTopic = wTemp.toUTF8();
		}
	}
	else { jf.err("Unknown input id-SCDAwidget-cbColRowClicked"); }
	prompt[3] = activeColTopic;
	prompt[4] = activeRowTopic;
	
	// Query the server for the next stage, depending on the current state of specificity.
	if (prompt[3] == "*" || prompt[4] == "*")  
	{
		sRef.pullTopic(prompt);  // Rows or columns not yet determined.
	}
	else
	{
		vector<vector<string>> vvsDummy;
		sRef.pullVariable(prompt, vvsDummy);
	}
}
void SCDAwidget::cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem)
{
	// Repopulate the combobox with the given list, showing the top item. 
	cb->clear();
	for (int ii = 0; ii < vsItem.size(); ii++)
	{
		cb->addItem(vsItem[ii]);
	}
	cb->setCurrentIndex(0);
}
void SCDAwidget::cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem)
{
	cbRenew(cb, sTop, vsItem, sTop);  // Default is top item shown. 
}
void SCDAwidget::cbRenew(Wt::WComboBox*& cb, string sTop, vector<vector<string>>& vvsItem)
{
	// Populates the combobox with sTop, followed by the final elements of vvsItem.
	cb->clear();
	cb->addItem(sTop);
	for (int ii = 0; ii < vvsItem.size(); ii++)
	{
		cb->addItem(vvsItem[ii].back());
	}
	cb->setCurrentIndex(0);
}
void SCDAwidget::cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem, string selItem)
{
	// Repopulate the combobox with the selected item shown. 
	int index = -1;
	cb->clear();
	cb->addItem(sTop.c_str());
	for (int ii = 0; ii < vsItem.size(); ii++)
	{
		cb->addItem(vsItem[ii].c_str());
		if (vsItem[ii] == selItem) { index = ii; }
	}
	cb->setCurrentIndex(index + 1);
}
void SCDAwidget::cbVarMIDClicked(string id)
{
	Wt::WApplication* app = Wt::WApplication::instance();
	string sRegion;
	int geoCode;
	if (activeCata.size() > 0)
	{
		treeClicked();
	}
	else
	{
		int bbq = 1;
	/*
	vector<vector<string>> vvsVariable = getVariable();
	vector<string> prompt(5);
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCategory;
	prompt[3] = activeColTopic;
	prompt[4] = activeRowTopic;
	sRef.pullVariable(prompt, vvsVariable);
	*/
	}

}
void SCDAwidget::cbVarTitleClicked(string id)
{
	int removeVar = -1;
	int index = mapVarIndex.at(id);
	Wt::WString wTemp = varTitle[index]->currentText();
	string sTitle;
	string sTitleClicked = wTemp.toUTF8();
	string sTop = "[None selected]";
	if (sTitleClicked == sTop)
	{
		varPanel[index]->setTitle("Select a parameter ...");
		varMID[index]->clear();
		varMID[index]->addItem(sTop);
		varMID[index]->setEnabled(0);
		return; 
	}
	else  // If this title already exists in a panel, delete that panel. (There can be only one ... !)
	{
		auto boxChildren = boxConfig->children();
		for (int ii = numPreVariable; ii < boxChildren.size(); ii++)
		{
			if (ii - numPreVariable == index) { continue; }
			wTemp = varTitle[ii - numPreVariable]->currentText();
			sTitle = wTemp.toUTF8();
			if (sTitle == sTitleClicked)
			{
				removeVar = ii - numPreVariable;
				break;
			}
		}
	}
	vector<string> vsMID;
	for (int ii = 0; ii < vvsParameter.size(); ii++)
	{
		if (vvsParameter[ii].back() == sTitleClicked)
		{
			vsMID = vvsParameter[ii];
			break;
		}
	}
	vsMID.pop_back();
	varMID[index]->setEnabled(1);
	cbRenew(varMID[index], vsMID);
	varPanel[index]->setTitle("Parameter");
	cbVarMIDClicked("");
	if (removeVar >= 0) 
	{ 
		Wt::WApplication* app = Wt::WApplication::instance();
		app->processEvents();
		removeVariable(removeVar); 
	}
}
void SCDAwidget::cbYearClicked()
{
	Wt::WString wsYear = cbYear->currentText();
	string sYear = wsYear.toUTF8();
	// INCOMPLETE: finish later if more years are to be added.
}
void SCDAwidget::connect()
{
	if (sRef.connect(this, bind(&SCDAwidget::processDataEvent, this, placeholders::_1)))
	{
		Wt::WApplication::instance()->enableUpdates(1);
		sessionID = Wt::WApplication::instance()->sessionId();
	}
}
void SCDAwidget::dataClick()
{
	// When something is double-clicked within the data box, determine which fn to launch.
	int tabIndex = tabData->currentIndex();

}
int SCDAwidget::getHeight()
{
	const Wt::WEnvironment& env = Wt::WApplication::instance()->environment();
	int iHeight = env.screenHeight();
	if (iHeight < 0) { jf.err("Failed to obtain widget dimensions-wtpaint.getDimensions"); }
	return iHeight;
}
vector<vector<string>> SCDAwidget::getVariable()
{
	// Checks the active state of each "variable" panel and returns each title and MID.
	int numVar = varPanel.size();
	vector<vector<string>> vvsVariable(numVar);
	string sTitle, sMID;
	Wt::WString wTemp;
	for (int ii = 0; ii < numVar; ii++)
	{
		wTemp = varTitle[ii]->currentText();
		sTitle = wTemp.toUTF8();
		wTemp = varMID[ii]->currentText();
		sMID = wTemp.toUTF8();
		vvsVariable[ii] = { sTitle, sMID };
	}
	return vvsVariable;
}
int SCDAwidget::getWidth()
{
	const Wt::WEnvironment& env = Wt::WApplication::instance()->environment();
	int iWidth = env.screenWidth();
	if (iWidth < 0) { jf.err("Failed to obtain widget dimensions-wtpaint.getDimensions"); }
	return iWidth;
}
void SCDAwidget::init()
{
	connect();	
	makeUI();
	initUI();
}
void SCDAwidget::initUI()
{
	Wt::WString wstemp;
	string temp;
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt = { app->sessionId() };
	string sPath = app->docRoot();
	sPath += "\\SCDA-Wt.css";
	auto cssLink = Wt::WLink(sPath);
	app->useStyleSheet(cssLink);

	// Colourful things.
	colourSelected.setRgb(200, 200, 255, 50);
	colourWhite.setRgb(255, 255, 255, 255);

	// Initial values for widget sizes.
	boxConfig->setMaximumSize(len300p, wlAuto);
	boxMap->setStyleClass("paintMap");
	boxMap->setStyleClass("box");
	//boxData->setMinimumSize(len800p, len800p);
	//boxMap->setMinimumSize(len700p, len700p);

	// Initial values for cbYear.
	activeYear = "2016";  // Default for now, as it is the only possibility.
	panelYear->setTitle("Census Year");
	cbYear->addItem(activeYear);
	cbYear->setCurrentIndex(0);
	cbYear->setEnabled(0);
	cbYear->changed().connect(this, &SCDAwidget::cbYearClicked);
	
	// Initial values for the category panel.
	panelCategory->setTitle("Topical Category");
	vector<string> vsTopic = sRef.getTopicList(activeYear);
	cbCategory->addItem("[Choose a topical category]");
	for (int ii = 0; ii < vsTopic.size(); ii++)
	{
		cbCategory->addItem(vsTopic[ii].c_str());
	}
	cbCategory->changed().connect(this, &SCDAwidget::cbCategoryClicked);

	// Initial values for the col/row panels.
	panelColTopic->setTitle("Table Column Topic");
	temp = "panelCol";
	function<void()> fnColTopic = bind(&SCDAwidget::cbColRowClicked, this, temp);
	cbColTopic->changed().connect(fnColTopic);
	panelColTopic->setHidden(1);
	panelRowTopic->setTitle("Table Row Topic");
	temp = "panelRow";
	function<void()> fnRowTopic = bind(&SCDAwidget::cbColRowClicked, this, temp);
	cbRowTopic->changed().connect(fnRowTopic);
	panelRowTopic->setHidden(1);

	// Initial values for the tab widget.
	tabData->setTabEnabled(0, 0);
	tabData->setTabEnabled(1, 0);
	tabData->setTabEnabled(2, 0);
	tabData->setCurrentIndex(0);

	// Initial values for the region tree widget.
	treeRegion->itemSelectionChanged().connect(this, &SCDAwidget::treeClicked);

	// Initial values for the buttons.
	pbMobile->setEnabled(0);
	pbMobile->clicked().connect(this, &SCDAwidget::test);

}
void SCDAwidget::makeUI()
{
	this->clear();
	auto hLayout = make_unique<Wt::WHBoxLayout>();

	auto boxConfigUnique = make_unique<Wt::WContainerWidget>();
	boxConfig = boxConfigUnique.get();
	auto vLayoutConfig = make_unique<Wt::WVBoxLayout>();
	layoutConfig = vLayoutConfig.get();
	auto boxTestUnique = make_unique<Wt::WContainerWidget>();
	boxTest = boxTestUnique.get();
	auto hLayoutTestUnique = make_unique<Wt::WHBoxLayout>();
	auto pbMobileUnique = make_unique<Wt::WPushButton>("Toggle Mobile Version");
	pbMobile = pbMobileUnique.get();
	auto leTestUnique = make_unique<Wt::WLineEdit>();
	leTest = leTestUnique.get();
	auto panelYearUnique = make_unique<Wt::WPanel>();
	panelYear = panelYearUnique.get();
	auto cbYearUnique = make_unique<Wt::WComboBox>();
	cbYear = cbYearUnique.get();
	auto panelCategoryUnique = make_unique<Wt::WPanel>();
	panelCategory = panelCategoryUnique.get();
	auto cbCategoryUnique = make_unique<Wt::WComboBox>();
	cbCategory = cbCategoryUnique.get();
	auto panelRowTopicUnique = make_unique<Wt::WPanel>();
	panelRowTopic = panelRowTopicUnique.get();
	auto cbRowTopicUnique = make_unique<Wt::WComboBox>();
	cbRowTopic = cbRowTopicUnique.get();
	auto panelColTopicUnique = make_unique<Wt::WPanel>();
	panelColTopic = panelColTopicUnique.get();
	auto cbColTopicUnique = make_unique<Wt::WComboBox>();
	cbColTopic = cbColTopicUnique.get();

	auto boxDataUnique = make_unique<Wt::WContainerWidget>();
	boxData = boxDataUnique.get();
	auto tabDataUnique = make_unique<Wt::WTabWidget>();
	tabData = tabDataUnique.get();
	auto treeRegionUnique = make_unique<Wt::WTree>();
	treeRegion = treeRegionUnique.get();
	auto tableDataUnique = make_unique<Wt::WTable>();
	tableData = tableDataUnique.get();
	auto boxMapUnique = make_unique<Wt::WContainerWidget>();
	boxMap = boxMapUnique.get();

	hLayoutTestUnique->addWidget(move(pbMobileUnique));
	hLayoutTestUnique->addWidget(move(leTestUnique), 1);
	boxTestUnique->setLayout(move(hLayoutTestUnique));

	panelYearUnique->setCentralWidget(move(cbYearUnique));
	panelCategoryUnique->setCentralWidget(move(cbCategoryUnique));
	panelColTopicUnique->setCentralWidget(move(cbColTopicUnique));
	panelRowTopicUnique->setCentralWidget(move(cbRowTopicUnique));

	vLayoutConfig->addWidget(move(boxTestUnique));
	vLayoutConfig->addWidget(move(panelYearUnique));
	vLayoutConfig->addWidget(move(panelCategoryUnique));
	vLayoutConfig->addWidget(move(panelRowTopicUnique));
	vLayoutConfig->addWidget(move(panelColTopicUnique));
	numPreVariable = vLayoutConfig->count();
	boxConfigUnique->setLayout(move(vLayoutConfig));

	tabDataUnique->addTab(move(treeRegionUnique), "Geographic Region");
	tabDataUnique->addTab(move(tableDataUnique), "Data Table");
	tabDataUnique->addTab(move(boxMapUnique), "Data Map");
	boxDataUnique->addWidget(move(tabDataUnique));

	hLayout->addWidget(move(boxConfigUnique), 1);
	hLayout->addWidget(move(boxDataUnique), 2);
	this->setLayout(move(hLayout));
}
void SCDAwidget::populateTree(JTREE& jt, Wt::WTreeNode*& node)
{
	// Recursive function that takes an existing node and makes its children.
	vector<string> vsChildren;
	vector<int> viChildren;
	Wt::WString wTemp = node->label()->text();
	string sNode = wTemp.toUTF8();
	int iNode = jt.getIName(sNode);
	jt.listChildren(sNode, viChildren, vsChildren);
	for (int ii = 0; ii < vsChildren.size(); ii++)
	{
		wTemp = Wt::WString::fromUTF8(vsChildren[ii]);
		auto childUnique = make_unique<Wt::WTreeNode>(wTemp);
		auto child = childUnique.get();
		populateTree(jt, child);
		node->addChildNode(move(childUnique));
	}
}
void SCDAwidget::processDataEvent(const DataEvent& event)
{
	jf.timerStart();
	Wt::WApplication* app = Wt::WApplication::instance();
	string sPath = app->docRoot();
	sPath += "/SCDA-Wt.css";
	auto cssLink = Wt::WLink(sPath);
	app->useStyleSheet(cssLink);
	app->triggerUpdate();
	vector<vector<vector<double>>> areas;
	string temp;
	string sessionID = app->sessionId(), nameAreaSel;
	vector<string> slist, listCol, listRow, vsDIMIndex;
	vector<vector<string>> table, vvsCata, vvsVariable;
	long long timer;
	vector<int> tempIndex;
	vector<double> regionData;
	int inum, maxCol;

	int etype = event.type();
	switch (etype)
	{
	case 0:  // Connect.
		break;
	case 1:  // Label: display it.
	{
		break;
	}
	case 2:  // Map: display it on the painter widget.
	{
		slist = event.get_list();  // vsRegionName.
		areas = event.get_areas();  // Border coords.
		regionData = event.get_regionData();  // vdData.
		boxMap->setStyleClass("paintMap");
		auto wtMapUnique = make_unique<WTPAINT>();
		wtMap = boxMap->addWidget(move(wtMapUnique));
		wtMap->setStyleClass("paintMap");
		wtMap->drawMap(areas, slist, regionData);
		tabData->setTabEnabled(2, 1);
		tabData->setCurrentIndex(2);
		break;
	}
	case 3:  // Table: populate the widget with data.
	{
		table = event.getTable();
		listCol = event.getListCol();
		listRow = event.getListRow();
		activeTableColTitle = listCol.back();
		listCol.pop_back();
		activeTableRowTitle = listRow.back();
		listRow.pop_back();
		int iRow, iCol;
		Wt::WString wTemp;
		tableData->clear();
		tableData->setHeaderCount(1, Wt::Orientation::Horizontal);
		tableData->setHeaderCount(1, Wt::Orientation::Vertical);
		for (int ii = 0; ii < listCol.size(); ii++)
		{
			tableData->elementAt(0, ii)->addNew<Wt::WText>(listCol[ii]);
		}
		for (int ii = 0; ii < table.size(); ii++)
		{
			iRow = ii + 1;
			tableData->elementAt(iRow, 0)->addNew<Wt::WText>(listRow[ii]);
			for (int jj = 0; jj < table[ii].size(); jj++)
			{
				iCol = jj + 1;
				auto textUnique = make_unique<Wt::WText>(table[ii][jj]);
				function<void()> fnSingle = bind(&SCDAwidget::tableClicked, this, ii, jj);
				textUnique->clicked().connect(fnSingle);
				function<void()> fnDouble = bind(&SCDAwidget::tableDoubleClicked, this, ii, jj);
				textUnique->doubleClicked().connect(fnDouble);
				tableData->elementAt(iRow, iCol)->addWidget(move(textUnique));
			}
		}
		tabData->setTabEnabled(1, 1);
		temp = "Data Table (" + listCol[0] + ")";
		wTemp = Wt::WString::fromUTF8(temp);
		auto tab = tabData->itemAt(1);
		tab->setText(wTemp);
		break;
	}
	case 4:  // Topic: update the GUI with options.
	{
		listCol = event.getListCol();
		listRow = event.getListRow();
		
		string colDefault = "[Choose a column topic]";
		string rowDefault = "[Choose a row topic]";
		if (listCol.size() == 1)
		{
			activeColTopic = listCol[0];
		}
		cbRenew(cbColTopic, colDefault, listCol, activeColTopic);
		if (listRow.size() == 1)
		{
			activeRowTopic = listRow[0];
		}
		cbRenew(cbRowTopic, rowDefault, listRow, activeRowTopic);
		
		if (listCol.size() == 1 && listRow.size() == 1)
		{
			vector<vector<string>> vvsVariable;
			vector<string> prompt(5);
			prompt[0] = app->sessionId();
			prompt[1] = activeYear;
			prompt[2] = activeCategory;
			prompt[3] = activeColTopic;
			prompt[4] = activeRowTopic;
			sRef.pullVariable(prompt, vvsVariable);
		}
		break;
	}
	case 5:  // Tree: populate the tree tab using the JTREE object.
	{
		jtRegion = event.getTree();
		string sRoot = jtRegion.getRootName();
		Wt::WString wTemp = Wt::WString::fromUTF8(sRoot);
		auto treeRootUnique = make_unique<Wt::WTreeNode>(wTemp);
		auto treeRoot = treeRootUnique.get();
		populateTree(jtRegion, treeRoot);
		treeRegion->setTreeRoot(move(treeRootUnique));
		treeRegion->setSelectionMode(Wt::SelectionMode::Single);
		tabData->setTabEnabled(0, 1);
		treeRoot = treeRegion->treeRoot();
		treeRoot->expand();
		treeRegion->select(treeRoot);
		break;
	}
	case 6:  // Variable: create new panels with options for the user to specify.
	{
		vvsVariable = event.getVariable();  // Form [variable index][MID0, MID1, ..., variable title].
		if (vvsVariable.size() < 1) { jf.err("Variable event missing input-SCDAwidget.processDataEvent"); }
		vvsCata = event.getCata();
		vsDIMIndex = event.get_list();
		int numCata = 0;
		for (int ii = 0; ii < vvsCata.size(); ii++)
		{
			numCata += vvsCata[ii].size() - 1;
		}

		int index = varPanel.size();
		int numVar = -1;
		if (numCata == 1)
		{
			// If only one catalogue satisfies the conditions, then load all variables locally.
			activeCata = vvsCata[0][1];
			if (mapNumVar.count(vvsCata[0][1]))
			{
				numVar = mapNumVar.at(vvsCata[0][1]);
			}
			else
			{
				numVar = vsDIMIndex.size() - 2;
				mapNumVar.emplace(vvsCata[0][1], numVar);
			}
			vector<string> vsTemp(2);
			if (!index) { vvsParameter = vvsVariable; }
			for (int ii = 0; ii < vvsVariable.size(); ii++)
			{
				vsTemp[0] = vvsVariable[ii].back();  // Title.
				vsTemp[1] = vvsVariable[ii][0];  // Default MID.
				addVariable(vsTemp);
			}
			for (int ii = index; ii < varTitle.size(); ii++)
			{
				varTitle[ii]->setEnabled(0);
			}
		}
		else
		{
			// Offer the user one parameter at a time, until only a single catalogue remains.
			int bbq = 1;
		}

		// If there are no more unspecified variables, populate the region tree tab.
		if (varPanel.size() == numVar)
		{
			app->processEvents();
			activeYear = vvsCata[0][0];
			activeCata = vvsCata[0][1];
			vector<string> promptTree(3);
			promptTree[0] = app->sessionId();
			promptTree[1] = vvsCata[0][0];
			promptTree[2] = vvsCata[0][1];
			sRef.pullTree(promptTree);
		}
	
		break;
	}
	}
	timer = jf.timerStop();
	jf.logTime("processDataEvent#" + to_string(etype), timer);
}
void SCDAwidget::removeVariable(int varIndex)
{
	// Remove the given variable's panel widget. Update mapVarIndex, varPanel, varTitle, varMID;
	int index;
	string id;
	Wt::WString wTemp;
	auto varPanelTemp = varPanel;
	varPanel.clear();
	auto varTitleTemp = varTitle;
	varTitle.clear();
	auto varMIDTemp = varMID;
	varMID.clear();
	mapVarIndex.clear();
	for (int ii = 0; ii < varPanelTemp.size(); ii++)
	{
		if (ii == varIndex) { continue; }
		index = varPanel.size();
		varPanel.push_back(varPanelTemp[ii]);
		id = varPanel[index]->id();
		mapVarIndex.emplace(id, index);
		varTitle.push_back(varTitleTemp[ii]);
		id = varTitle[index]->id();
		mapVarIndex.emplace(id, index);
		varMID.push_back(varMIDTemp[ii]);
		id = varMID[index]->id();
		mapVarIndex.emplace(id, index);
	}
	auto boxChildren = boxConfig->children();
	boxConfig->removeWidget(boxChildren[numPreVariable + varIndex]);
	Wt::WApplication* app = Wt::WApplication::instance();
	app->processEvents();
}
void SCDAwidget::resetVariables()
{
	// Remove all "variable" panels and clear all "variable" buffers.
	auto boxChildren = boxConfig->children();
	for (int ii = numPreVariable; ii < boxChildren.size(); ii++)
	{
		boxConfig->removeWidget(boxChildren[ii]);
	}
	activeCata.clear();
	vvsParameter.clear();
	varPanel.clear();
	varTitle.clear();
	varMID.clear();
	mapVarIndex.clear();
	Wt::WApplication* app = Wt::WApplication::instance();
	app->processEvents();
}
void SCDAwidget::selectTableCell(int iRow, int iCol)
{


	//if (pbTable->isEnabled()) { pbMap->setEnabled(1); }
}
void SCDAwidget::setTable(int geoCode, string sRegion)
{
	// This variant loads a table using a selected region from the tree. 
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(5);
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCata;
	prompt[3] = to_string(geoCode);
	prompt[4] = sRegion;
	vector<vector<string>> variable = getVariable();
	sRef.pullTable(prompt, variable);
	tabData->setTabEnabled(1, 1);
}
void SCDAwidget::tableClicked(int iRow, int iCol)
{
	if (iRow < 0 || iCol < 0) { return; }
	if (selectedCell[0] >= 0 && selectedCell[1] >= 0)
	{
		tableData->elementAt(selectedCell[0] + 1, selectedCell[1] + 1)->decorationStyle().setBackgroundColor(colourWhite);
	}
	selectedCell[0] = iRow;
	selectedCell[1] = iCol;
	tableData->elementAt(iRow + 1, iCol + 1)->decorationStyle().setBackgroundColor(colourSelected);
}
void SCDAwidget::tableDoubleClicked(int iRow, int iCol)
{
	tableClicked(iRow, iCol);  // Highlight.
	Wt::WText* cell = (Wt::WText*)tableData->elementAt(0, 0)->widget(0);
	Wt::WString wTemp = cell->text();
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(4);
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCata;
	prompt[3] = wTemp.toUTF8();  // Parent region name. 
	
	vector<vector<string>> variable = getVariable();
	cell = (Wt::WText*)tableData->elementAt(iRow + 1, 0)->widget(0);
	wTemp = cell->text();
	string temp = wTemp.toUTF8();
	variable.push_back({ activeTableRowTitle, temp });
	variable.push_back({ activeTableColTitle, to_string(iCol + 1) });
	sRef.pullMap(prompt, variable);
}
void SCDAwidget::test()
{

}
void SCDAwidget::treeClicked()
{
	auto selSet = treeRegion->selectedNodes();
	if (selSet.size() < 1) { return; }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	string sRegion = wTemp.toUTF8(); 
	int geoCode = jtRegion.getIName(sRegion);
	setTable(geoCode, sRegion);
}
