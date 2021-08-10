#include "SCDAwidget.h"

void SCDAwidget::cbCategoryClicked()
{
	// Populate tableData with its "input form", containing options for the user
	// to choose column and row topics. 
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
		sRef.pullVariable(prompt);
	}
}
void SCDAwidget::cbRenew(Wt::WComboBox*& cb, string sTop, vector<string>& vsItem)
{
	cbRenew(cb, sTop, vsItem, sTop);  // Default is top item shown. 
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
void SCDAwidget::cbVariableClicked()
{
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(6);
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCategory;
	prompt[3] = activeColTopic;
	prompt[4] = activeRowTopic;
	prompt[5] = getVariable();
	sRef.pullVariable(prompt);
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
string SCDAwidget::getVariable()
{
	string sVar = "";
	int iVar;
	for (int ii = 0; ii < varCB.size(); ii++)
	{
		iVar = varCB[ii]->currentIndex();
		if (ii > 0) { sVar += "$"; }
		sVar += to_string(iVar);
	}
	return sVar;
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

	tabDataUnique->addTab(move(tableDataUnique), "Table");
	tabDataUnique->addTab(move(wtMapUnique), "Map");

	hLayout->addLayout(move(vLayoutConfig), 1);
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
	vector<string> slist, listCol, listRow;
	vector<wstring> wlist;
	vector<vector<string>> table;
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
			cbVariableClicked();
		}
		break;
	}
	case 5:  // Variable: create a new panel with options for the user to specify.
	{
		slist = event.get_list();  // Last value is the variable name.
		if (slist.size() < 3) { jf.err("Variable event missing input-SCDAwidget.processDataEvent"); }
		string sTop = "[None selected]";
		int index = varCB.size();
		auto varCBUnique = make_unique<Wt::WComboBox>();
		varCB.push_back(varCBUnique.get());		
		auto varPanelUnique = make_unique<Wt::WPanel>();
		varPanel.push_back(varPanelUnique.get());

		varPanel[index]->setTitle(slist[slist.size() - 1].c_str());
		slist.pop_back();
		cbRenew(varCB[index], sTop, slist);
		varPanelUnique->setCentralWidget(move(varCBUnique));
		layoutConfig->addWidget(move(varPanelUnique));
		
		break;
	}
	}
	timer = jf.timerStop();
	jf.logTime("processDataEvent#" + to_string(etype), timer);
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
void SCDAwidget::setTable(vector<string> prompt)
{
	sRef.pullTable(prompt);
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
