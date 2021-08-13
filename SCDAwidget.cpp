#include "SCDAwidget.h"

void SCDAwidget::cbCategoryClicked()
{
	// Populate tableData with its "input form", containing options for the user
	// to choose column and row topics. 
	resetVariable();
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
	resetVariable();
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
	vector<string> prompt(5);
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCategory;
	prompt[3] = activeColTopic;
	prompt[4] = activeRowTopic;
	auto boxChildren = boxConfig->children();
	int numVar = boxChildren.size() - 5;
	vector<vector<string>> vvsVariable;
	string sTitle, sMID;
	Wt::WString wTemp;
	for (int ii = 0; ii < numVar; ii++)
	{
		wTemp = varTitle[ii]->currentText();
		sTitle = wTemp.toUTF8();
		wTemp = varMID[ii]->currentText();
		sMID = wTemp.toUTF8();
		vvsVariable.push_back({ sTitle, sMID });
	}  
	sRef.pullVariable(prompt, vvsVariable);
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
void SCDAwidget::init()
{
	connect();
	jt.init("Census Tables", sroot);
	makeUI();
	initUI();
}
void SCDAwidget::initUI()
{
	Wt::WString wstemp;
	string temp;
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt = { app->sessionId() };

	// Colourful things.
	colourSelected.setRgb(200, 200, 255, 50);
	colourWhite.setRgb(255, 255, 255, 255);

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

	// Initial values for the buttons.
	pbMobile->setText("Toggle Mobile Version");
	pbMobile->setEnabled(0);

	// Initial values for the map widget.

}
void SCDAwidget::makeUI()
{
	this->clear();
	auto hLayout = make_unique<Wt::WHBoxLayout>();

	auto boxConfigUnique = make_unique<Wt::WContainerWidget>();
	boxConfig = boxConfigUnique.get();
	auto vLayoutConfig = make_unique<Wt::WVBoxLayout>();
	layoutConfig = vLayoutConfig.get();
	auto pbMobileUnique = make_unique<Wt::WPushButton>();
	pbMobile = pbMobileUnique.get();
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

	auto tabDataUnique = make_unique<Wt::WTabWidget>();
	tabData = tabDataUnique.get();
	auto treeRegionUnique = make_unique<Wt::WTree>();
	treeRegion = treeRegionUnique.get();
	auto tableDataUnique = make_unique<Wt::WTable>();
	tableData = tableDataUnique.get();
	auto wtMapUnique = make_unique<WTPAINT>(commMap);
	wtMap = wtMapUnique.get();

	panelYearUnique->setCentralWidget(move(cbYearUnique));
	panelCategoryUnique->setCentralWidget(move(cbCategoryUnique));
	panelColTopicUnique->setCentralWidget(move(cbColTopicUnique));
	panelRowTopicUnique->setCentralWidget(move(cbRowTopicUnique));

	vLayoutConfig->addWidget(move(pbMobileUnique));
	vLayoutConfig->addWidget(move(panelYearUnique));
	vLayoutConfig->addWidget(move(panelCategoryUnique));
	vLayoutConfig->addWidget(move(panelRowTopicUnique));
	vLayoutConfig->addWidget(move(panelColTopicUnique));
	numPreVariable = vLayoutConfig->count();
	boxConfigUnique->setLayout(move(vLayoutConfig));

	tabDataUnique->addTab(move(treeRegionUnique), "Geographic Region");
	tabDataUnique->addTab(move(tableDataUnique), "Data Table");
	tabDataUnique->addTab(move(wtMapUnique), "Data Map");

	hLayout->addWidget(move(boxConfigUnique), 1);
	hLayout->addWidget(move(tabDataUnique), 2);
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
void SCDAwidget::populateTree(Wt::WTree*& tree, JTREE& jt)
{
	auto oldRoot = tree->treeRoot();
	if (oldRoot != nullptr) { tree->removeWidget(oldRoot); }
	string temp = jt.getRootName();
	auto rootUnique = make_unique<Wt::WTreeNode>(temp.c_str());
	auto root = rootUnique.get();
	populateTreeHelper(root, jt);
	tree->setTreeRoot(move(rootUnique));
}
void SCDAwidget::populateTreeHelper(Wt::WTreeNode*& node, JTREE& jt)
{
	// Recursive function that takes an existing node and makes its children.
	vector<string> vsChildren;
	vector<int> viChildren;
	Wt::WString wTemp = node->label()->text();
	string sNode = wTemp.toUTF8();
	jt.listChildren(sNode, viChildren, vsChildren);
	for (int ii = 0; ii < vsChildren.size(); ii++)
	{
		auto childUnique = make_unique<Wt::WTreeNode>(vsChildren[ii].c_str());
		auto child = childUnique.get();
		populateTreeHelper(child, jt);
		node->addChildNode(move(childUnique));
	}
}
void SCDAwidget::processDataEvent(const DataEvent& event)
{
	jf.timerStart();
	Wt::WApplication* app = Wt::WApplication::instance();
	app->triggerUpdate();
	vector<vector<Wt::WPointF>> areas;
	Wt::WString wsYear, wsDesc, wsRegion;
	wstring wtemp;
	string sessionID = app->sessionId(), nameAreaSel;
	vector<string> slist, listCol, listRow;
	vector<wstring> wlist;
	vector<vector<string>> table, vvsCata, vvsVariable;
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
	case 1:  // Label: display it.
	{
		break;
	}
	case 2:  // Map: display it on the painter widget.
	{
		slist = event.get_list();
		areas = event.get_areas();
		regionData = event.get_regionData();		
		wtMap->drawMap(areas, slist, regionData);
		slist.pop_back();
		updateMapRegionList(slist, mapRegionList, 1);
		break;
	}
	case 3:  // Table: populate the widget with data.
	{
		table = event.getTable();
		listCol = event.getListCol();
		listRow = event.getListRow();
		tableData->setHeaderCount(1, Wt::Orientation::Horizontal);
		tableData->setHeaderCount(1, Wt::Orientation::Vertical);
		tableData->elementAt(0, 0)->addWidget(make_unique<Wt::WText>(""));
		for (int ii = 0; ii < listCol.size(); ii++)
		{
			tableData->elementAt(0, ii + 1)->addWidget(make_unique<Wt::WText>(listCol[ii].c_str()));
		}
		for (int ii = 0; ii < table.size(); ii++)
		{
			tableData->elementAt(ii + 1, 0)->addWidget(make_unique<Wt::WText>(listRow[ii].c_str()));
			for (int jj = 0; jj < table[ii].size(); jj++)
			{
				tableData->elementAt(ii + 1, jj + 1)->addWidget(make_unique<Wt::WText>(table[ii][jj].c_str()));
			}
		}
		selectedRow = -1;
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
		jt = event.getTree();
		populateTree(treeRegion, jt);
		treeRegion->setSelectionMode(Wt::SelectionMode::Single);
		auto treeRoot = treeRegion->treeRoot();
		treeRegion->select(treeRoot);
		Wt::WApplication* app = Wt::WApplication::instance();
		app->processEvents();
		setTable(treeRegion);
		tabData->setCurrentIndex(1);
		break;
	}
	case 6:  // Variable: create a new panel with options for the user to specify.
	{
		vvsVariable = event.getVariable();  // Form [variable index][MID0, MID1, ..., variable title].
		if (vvsVariable.size() < 1) { jf.err("Variable event missing input-SCDAwidget.processDataEvent"); }
		int index = varPanel.size();
		if (!index) { vvsParameter = vvsVariable; }  // vvsVariable represents the full list of variables.
		string sTop = "[None selected]";
		string sTitle = "Select a parameter ...";
		Wt::WString wTemp;
		string temp;
		
		// Add a new blank panel only if there isn't already a blank panel.
		if (varPanel.size() > 0)
		{
			wTemp = varPanel[varPanel.size() - 1]->title();
			temp = wTemp.toUTF8();
		}
		if (temp != sTitle) 
		{
			auto varPanelUnique = make_unique<Wt::WPanel>();
			varPanel.push_back(varPanelUnique.get());
			temp = varPanelUnique->id();
			mapVarIndex.emplace(temp, index);
			auto varBoxUnique = make_unique<Wt::WContainerWidget>();
			auto varVLayoutUnique = make_unique<Wt::WVBoxLayout>();

			auto varCBTitleUnique = make_unique<Wt::WComboBox>();
			varTitle.push_back(varCBTitleUnique.get());
			temp = varCBTitleUnique->id();
			mapVarIndex.emplace(temp, index);
			function<void()> fnVarTitle = bind(&SCDAwidget::cbVarTitleClicked, this, temp);
			varTitle[index]->changed().connect(fnVarTitle);

			auto varCBMIDUnique = make_unique<Wt::WComboBox>();
			varMID.push_back(varCBMIDUnique.get());
			temp = varCBMIDUnique->id();
			mapVarIndex.emplace(temp, index);
			function<void()> fnVarMID = bind(&SCDAwidget::cbVarMIDClicked, this, temp);
			varMID[index]->changed().connect(fnVarMID);

			if (vvsVariable.size() == 1)  // Display the only possibility with its default MID.
			{
				varPanel[index]->setTitle("Parameter");
				varTitle[index]->addItem(vvsVariable[0].back());
				vvsVariable[0].pop_back();
				cbRenew(varMID[index], vvsVariable[0]);
			}
			else
			{
				varPanel[index]->setTitle(sTitle);
				cbRenew(varTitle[index], sTop, vvsVariable);
				varMID[index]->addItem(sTop);
				varMID[index]->setEnabled(0);
			}
			varVLayoutUnique->addWidget(move(varCBTitleUnique));
			varVLayoutUnique->addWidget(move(varCBMIDUnique));
			varBoxUnique->setLayout(move(varVLayoutUnique));
			varPanelUnique->setCentralWidget(move(varBoxUnique));
			layoutConfig->addWidget(move(varPanelUnique));
		}

		// If there are no more unspecified variables, populate the region tree tab.
		if (vvsVariable.size() == 1)
		{
			Wt::WApplication* app = Wt::WApplication::instance();
			app->processEvents();
			vvsCata = event.getCata();
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
}
void SCDAwidget::resetVariable()
{
	// Remove all "variable" panels and clear all "variable" buffers.
	auto boxChildren = boxConfig->children();
	for (int ii = 5; ii < boxChildren.size(); ii++)
	{
		boxConfig->removeWidget(boxChildren[ii]);
	}
	vvsParameter.clear();
	varPanel.clear();
	varTitle.clear();
	varMID.clear();
	mapVarIndex.clear();
}
void SCDAwidget::selectTableCell(int iRow, int iCol)
{
	if (iRow < 1 || iCol < 1) { return; }
	int numCol = tableData->columnCount();
	if (selectedCell[0] >= 0 && selectedCell[1] >= 0)
	{
		tableData->elementAt(selectedCell[0], selectedCell[1])->decorationStyle().setBackgroundColor(colourWhite);
	}
	tableData->elementAt(iRow, iCol)->decorationStyle().setBackgroundColor(colourSelected);
	selectedCell[0] = iRow;
	selectedCell[1] = iCol;
	//if (pbTable->isEnabled()) { pbMap->setEnabled(1); }
}
void SCDAwidget::selectTableRow(int iRow)
{
	int numCol = tableData->columnCount();
	if (selectedRow >= 0)
	{
		for (int ii = 0; ii < numCol; ii++)
		{
			tableData->elementAt(selectedRow, ii)->decorationStyle().setBackgroundColor(colourWhite);
			tableData->elementAt(iRow, ii)->decorationStyle().setBackgroundColor(colourSelected);
		}
	}
	else
	{
		for (int ii = 0; ii < numCol; ii++)
		{
			tableData->elementAt(iRow, ii)->decorationStyle().setBackgroundColor(colourSelected);
		}
	}
	selectedRow = iRow;
	//if (pbTable->isEnabled()) { pbMap->setEnabled(1); }
}
void SCDAwidget::setTable(Wt::WTree*& tree)
{
	// This variant loads a table using a selected region from the tree. 
	auto selNodeSet = tree->selectedNodes();
	if (selNodeSet.size() != 1) { return; }
	auto selNodeIt = selNodeSet.begin();
	auto selNode = *selNodeIt;
	Wt::WString wTemp = selNode->label()->text();

}
void SCDAwidget::tableClicked(Wt::WString wsTable)
{
	selectedRegion = wsTable;
	//pbTable->setEnabled(1);
	//if (selectedRow >= 0) { pbMap->setEnabled(1); }
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
