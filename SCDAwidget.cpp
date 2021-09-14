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
	auto varPanelUnique = make_unique<Wt::WPanel>("Parameter");
	varPanel.push_back(varPanelUnique.get());
	varPanel[index]->setMaximumSize(wlAuto, len200p);

	temp = varPanelUnique->id();
	mapVarIndex.emplace(temp, index);

	auto cbjVar = make_unique<WJCOMBO>(2);
	cbjVar->setCB(vsTitle, 0, titleIndex);
	function<void()> fnVarTitle = bind(&SCDAwidget::cbVarTitleChanged, this, temp);
	cbjVar->vCB[0]->changed().connect(fnVarTitle);

	cbjVar->setCB(vsMID, 1, MIDIndex);
	cbjVar->vCB[1]->changed().connect(this, &SCDAwidget::cbVarMIDChanged);
	cbjVar->vCB[1]->clicked().connect(this, &SCDAwidget::cbVarMIDClicked);

	varPanelUnique->setCentralWidget(move(cbjVar));
	layoutConfig->addWidget(move(varPanelUnique));
	widgetMobile();
}
void SCDAwidget::cbCategoryChanged()
{
	// Populate the topic widgets with options for the user
	// to choose column and row topics. 
	resetVariables();
	resetMap();
	resetTable();
	resetTree();
	resetTopicSel();
	Wt::WString wsYear = cbjYear->vCB[0]->currentText();
	activeYear = wsYear.toUTF8();
	activeColTopic = "*";
	activeRowTopic = "*";
	int index = cbjCategory->vCB[0]->currentIndex();
	if (index == 0)
	{
		activeCategory = "*";
		panelTopicCol->setHidden(1);
		panelTopicRow->setHidden(1);
		cbjCategory->highlight(1);
		return;
	}

	cbjCategory->highlight(0);
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(5);
	Wt::WString wsTemp = cbjCategory->vCB[0]->currentText();
	activeCategory = wsTemp.toUTF8();
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCategory;
	prompt[3] = "*";  // Column topic.
	prompt[4] = "*";  // Row topic.
	sRef.pullTopic(prompt);
}
void SCDAwidget::cbColRowSelChanged()
{
	// Update the GUI using the most recent user input.
	long long timerTable = tableData->jf.timerReport();
	long long timerCB = jf.timerReport();
	vector<int> tableSel;
	if (timerCB < timerTable)  // CB values override table values. 
	{
		// Update the data table's selected cell. 
		int numRow = tableData->model->rowCount();
		int numCol = tableData->model->columnCount();
		if (numRow < 1 || numCol < 1) { return; }
		Wt::WString wsColSel = cbColTopicSel->currentText();
		string sColSel = wsColSel.toUTF8();
		Wt::WString wsRowSel = cbRowTopicSel->currentText();
		string sRowSel = wsRowSel.toUTF8();
		tableSel.assign(2, -2);
		string sCell;
		if (wsRowSel != wsNoneSel)
		{
			tableSel[0] = tableData->getRowIndex(sRowSel);
		}
		if (wsColSel != wsNoneSel)
		{
			tableSel[1] = tableData->getColIndex(sColSel);
		}
		tableData->cellSelect(tableSel[0], tableSel[1]);
	}
	else  // Table values override CB values. 
	{
		// Update the CB widgets. 
		tableSel = tableData->getRowColSel();
		int cbRowIndex = cbRowTopicSel->currentIndex();
		if (cbRowIndex != tableSel[0])
		{
			cbRowTopicSel->setCurrentIndex(tableSel[0]);
			//return;
		}
		int cbColIndex = cbColTopicSel->currentIndex();
		if (cbColIndex + 1 != tableSel[1]) 
		{ 
			cbColTopicSel->setCurrentIndex(tableSel[1] - 1); 
			//return;
		}
	}

	// Load a map.
	string sRegion = tableData->getCell(-1, 0);
	setMap(tableSel[0], tableSel[1], sRegion);
}
void SCDAwidget::cbColRowSelClicked()
{
	jf.timerStart();
}
void SCDAwidget::cbColRowTitleClicked(string id)
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
	if (id == "cbjTopicCol")
	{
		index = cbColTopicTitle->currentIndex();
		if (index == 0)
		{
			activeColTopic = "*";
			activeRowTopic = "*";
		}
		else
		{
			wTemp = cbColTopicTitle->currentText();
			activeColTopic = wTemp.toUTF8();
		}
	}
	else if (id == "panelRow")
	{
		index = cbRowTopicTitle->currentIndex();
		if (index == 0)
		{
			activeRowTopic = "*";
			activeColTopic = "*";
		}
		else
		{
			wTemp = cbRowTopicTitle->currentText();
			activeRowTopic = wTemp.toUTF8();
		}
	}
	else { jf.err("Unknown input id-SCDAwidget-cbColRowClicked"); }
	prompt[3] = activeColTopic;
	prompt[4] = activeRowTopic;

	if (activeRowTopic == "*")
	{
		cbRowTopicSel->clear();
		cbRowTopicSel->setHidden(1);
		pbSubsectionFilterCol->setHidden(1);
		pbSubsectionFilterRow->setHidden(1);
	}
	if (activeColTopic == "*")
	{
		cbColTopicSel->clear();
		cbColTopicSel->setHidden(1);
		pbSubsectionFilterCol->setHidden(1);
		pbSubsectionFilterRow->setHidden(1);
	}
	if (activeRowTopic == "*" && activeColTopic == "*")
	{
		resetMap();
		resetTable();
		resetTree();
	}

	// Query the server for the next stage, depending on the current state of specificity.
	if (prompt[3] == "*" || prompt[4] == "*")
	{
		sRef.pullTopic(prompt);  // Rows or columns not yet determined.
	}
	else
	{
		cbjTopicCol->highlight(0);
		cbjTopicRow->highlight(0);
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
void SCDAwidget::cbVarMIDChanged()
{
	if (activeCata.size() > 0) { treeClicked(); }
	else { jf.err("No activeCata-SCDAwidget.cbVarMIDclicked"); }
}
void SCDAwidget::cbVarMIDClicked()
{
	jf.timerStart();
}
void SCDAwidget::cbVarTitleChanged(string id)
{
	int removeVar = -1;
	vector<int> viEn;
	int index = mapVarIndex.at(id);
	WJCOMBO* cbjVar = (WJCOMBO*)varPanel[index]->centralWidget();
	int iActive = cbjVar->vCB[0]->currentIndex();
	string sTitle;
	string sTitleClicked = cbjVar->vvsList[0][iActive];
	string sTop = "[None selected]";
	if (sTitleClicked == sTop)
	{
		varPanel[index]->setTitle("Select a parameter ...");
		cbjVar->vCB[1]->clear();
		viEn = { 1, 1, 0 };
		cbjVar->setEnabled(viEn);
		return;
	}
	else  // If this title already exists in a panel, delete that panel. (There can be only one ... !)
	{
		auto boxChildren = boxConfig->children();
		for (int ii = numPreVariable; ii < boxChildren.size(); ii++)
		{
			if (ii - numPreVariable == index) { continue; }
			WJCOMBO* cbjVarOther = (WJCOMBO*)varPanel[ii - numPreVariable]->centralWidget();
			iActive = cbjVarOther->vCB[0]->currentIndex();
			sTitle = cbjVarOther->vvsList[0][iActive];
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
	viEn = { 1, 1, 1 };
	cbjVar->setEnabled(viEn);
	cbjVar->setCB(vsMID, 1);
	varPanel[index]->setTitle("Parameter");
	cbVarMIDChanged();
	if (removeVar >= 0)
	{
		Wt::WApplication* app = Wt::WApplication::instance();
		app->processEvents();
		removeVariable(removeVar);
	}
}

void SCDAwidget::cleanUnit(string& unit)
{
	string temp;
	if (mapUnit.count(unit))
	{
		temp = mapUnit.at(unit);
		unit = temp;
	}
}
void SCDAwidget::connect()
{
	if (sRef.connect(this, bind(&SCDAwidget::processDataEvent, this, placeholders::_1)))
	{
		Wt::WApplication::instance()->enableUpdates(1);
		sessionID = Wt::WApplication::instance()->sessionId();
		if (jsEnabled)
		{
			this->jsTWidth.connect(this, std::bind(&SCDAwidget::tableReceiveDouble, this, std::placeholders::_1));
			this->jsInfo.connect(this, std::bind(&SCDAwidget::tableReceiveString, this, std::placeholders::_1));
		}
	}
}
void SCDAwidget::dialogSubsectionFilterCol()
{
	wdColFilter = this->addWidget(make_unique<Wt::WDialog>("Choose a column item to display exclusively with its interior items -"));
	
	string priorSelected, sID;
	JTREE jtCol = tableData->getTreeCol(priorSelected);
	jtCol.expandGeneration = -1;  // Expand all nodes automatically.
	string sRoot = jtCol.getRootName();
	Wt::WString wsTemp = Wt::WString::fromUTF8(sRoot);
	auto treeRootUnique = make_unique<Wt::WTreeNode>(wsTemp);
	treeRootUnique->setLoadPolicy(Wt::ContentLoading::Eager);
	auto treeRoot = treeRootUnique.get();
	populateTree(jtCol, treeRoot);

	auto tree = make_unique<Wt::WTree>();
	tree->setSelectionMode(Wt::SelectionMode::Single);
	//treeRootUnique->setNodeVisible(0);
	tree->setTreeRoot(move(treeRootUnique));
	if (priorSelected.size() > 0)
	{
		sID = jtCol.mapSID.at(priorSelected);
		Wt::WTreeNode* wtNode = dynamic_cast<Wt::WTreeNode*>(tree->findById(sID));
		tree->select(wtNode);
	}

	tree->itemSelectionChanged().connect(this, &SCDAwidget::dialogSubsectionFilterColEnd);
	treeDialogCol = tree.get();
	auto wBox = wdColFilter->contents();
	wBox->addWidget(move(tree));
	wdColFilter->show();
}
void SCDAwidget::dialogSubsectionFilterColEnd()
{
	auto selSet = treeDialogCol->selectedNodes();
	if (selSet.size() < 1) { return; }
	if (selSet.size() > 1) { jf.err("Invalid selection-SCDAwidget.dialogColSubsectionFilterEnd"); }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	string sHeader = wTemp.toUTF8();
	wdColFilter->accept();

	int colIndex = tableData->mapColIndex.at(sHeader);
	tableData->setSubsectionFilterCol(colIndex);
	tabData->setCurrentIndex(1);
}
void SCDAwidget::dialogSubsectionFilterRow()
{
	wdRowFilter = this->addWidget(make_unique<Wt::WDialog>("Choose a row item to display exclusively with its interior items -"));

	string priorSelected, sID;
	JTREE jtRow = tableData->getTreeRow(priorSelected);
	jtRow.expandGeneration = -1;  // Expand all nodes automatically.
	string sRoot = jtRow.getRootName();
	Wt::WString wsTemp = Wt::WString::fromUTF8(sRoot);
	auto treeRootUnique = make_unique<Wt::WTreeNode>(wsTemp);
	treeRootUnique->setLoadPolicy(Wt::ContentLoading::Eager);
	auto treeRoot = treeRootUnique.get();
	populateTree(jtRow, treeRoot);

	auto tree = make_unique<Wt::WTree>();
	tree->setSelectionMode(Wt::SelectionMode::Single);
	tree->setTreeRoot(move(treeRootUnique));
	if (priorSelected.size() > 0)
	{
		sID = jtRow.mapSID.at(priorSelected);
		Wt::WTreeNode* wtNode = dynamic_cast<Wt::WTreeNode*>(tree->findById(sID));
		tree->select(wtNode);
	}

	tree->itemSelectionChanged().connect(this, &SCDAwidget::dialogSubsectionFilterRowEnd);
	treeDialogRow = tree.get();
	auto wBox = wdRowFilter->contents();
	wBox->addWidget(move(tree));
	wdRowFilter->show();
}
void SCDAwidget::dialogSubsectionFilterRowEnd()
{
	auto selSet = treeDialogRow->selectedNodes();
	if (selSet.size() < 1) { return; }
	if (selSet.size() > 1) { jf.err("Invalid selection-SCDAwidget.dialogRowSubsectionFilterEnd"); }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	string sHeader = wTemp.toUTF8();
	wdRowFilter->accept();

	int rowIndex = tableData->mapRowIndex.at(sHeader);
	tableData->setSubsectionFilterRow(rowIndex);
	tabData->setCurrentIndex(1);
}
void SCDAwidget::displayCata(const Wt::WKeyEvent& wKey)
{
	Wt::WString wsCata;
	size_t len;
	auto key = wKey.key();
	if (key == Wt::Key::Delete) 
	{
		if (activeCata.size() < 1) 
		{ 
			wsCata = "<None>"; 
			len = 6;
		}
		else 
		{ 
			wsCata = Wt::WString(activeCata); 
			len = activeCata.size();
		}
		textCata->setText(wsCata);
	}
}
void SCDAwidget::downloadMap()
{
	auto app = Wt::WApplication::instance();
	string fileRoot = app->docRoot();
	string filePath = fileRoot + "/MapTest.pdf";
	wtMap->printPDF(filePath);
}

vector<string> SCDAwidget::getDemo()
{
	vector<string> vsCata;
	if (vvsDemographic.size() < 1) { jf.err("No Demographic init-SCDAwidget.getDemo"); }
	if (cbjDemo == nullptr) 
	{ 
		vsCata.assign(vvsDemographic[0].begin() + 1, vvsDemographic[0].end());
		return vsCata;
	}
	Wt::WString wsDemo = cbjDemo->vCB[0]->currentText();
	if (wsDemo == wsNoneSel) { return vsCata; }
	string forWhom = wsDemo.toUTF8();
	for (int ii = 0; ii < vvsDemographic.size(); ii++)
	{
		if (vvsDemographic[ii][0] == forWhom)
		{
			vsCata.assign(vvsDemographic[ii].begin() + 1, vvsDemographic[ii].end());
			break;
		}
	}
	return vsCata;
}
int SCDAwidget::getHeight()
{
	const Wt::WEnvironment& env = Wt::WApplication::instance()->environment();
	int iHeight = env.screenHeight();
	if (iHeight < 0) { jf.err("Failed to obtain widget dimensions-wtpaint.getDimensions"); }
	return iHeight;
}
vector<string> SCDAwidget::getNextCBLayer(Wt::WComboBox*& wCB)
{
	// Return a list of the selected item's immediate children, with the parent on top.
	int parentIndex = wCB->currentIndex();
	Wt::WString wsTemp = wCB->currentText();
	vector<string> vsLayer = { wsTemp.toUTF8() };
	int indent = 0;
	while (vsLayer[0][0] == ' ') { vsLayer[0].erase(vsLayer[0].begin()); }
	//
	return vsLayer;
}
Wt::WString SCDAwidget::getTextLegend(Wt::WPanel*& wPanel)
{
	Wt::WContainerWidget* wBox = (Wt::WContainerWidget*)wPanel->centralWidget();
	int numCB = wBox->count();
	Wt::WComboBox* cbTitle = (Wt::WComboBox*)wBox->widget(0);
	Wt::WString wsLegend = cbTitle->currentText();
	if (numCB < 2) { return wsLegend; }

	Wt::WComboBox* cbSel = (Wt::WComboBox*)wBox->widget(1);
	string temp;
	Wt::WString wsTemp = cbSel->currentText();
	vector<string> vsSel = { wsTemp.toUTF8() };
	size_t sizeMID = vsSel[0].size();

	// Populate the hierarchy of MIDs in reverse order.
	int countLast = 0, countTemp, index;
	while (vsSel[0][0] == '+')
	{
		countLast++;
		vsSel[0].erase(vsSel[0].begin());
	}
	countLast--;
	index = cbSel->currentIndex();
	while (countLast > 0)
	{
		index--;
		wsTemp = cbSel->itemText(index);
		temp = wsTemp.toUTF8();
		countTemp = 0;
		while (temp[0] == '+')
		{
			countTemp++;
			temp.erase(temp.begin());
		}
		if (countTemp == countLast)
		{
			vsSel.push_back(temp);
			countLast--;
		}
	}

	size_t pos1 = vsSel[0].find("Total "); 
	size_t pos2 = vsSel[0].find(temp);	
	if (pos1 == 0 && pos2 < sizeMID) { vsSel[0] = "Total"; }

	for (int ii = vsSel.size() - 1; ii >= 0; ii--)
	{
		wsLegend += " | " + Wt::WString::fromUTF8(vsSel[ii]);
	}
	return wsLegend;
}
string SCDAwidget::getUnit()
{
	if (activeCata.size() < 1) { return ""; }

	// Look for a unit using the data table.
	string unit = tableData->getUnit();
	if (unit.size() > 0) 
	{ 
		cleanUnit(unit);
		return unit; 
	}

	// No unit found, so assign the default unit.
	unit = "# of persons";
	return unit;
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
		if (sMID == "") { sMID = "*"; }
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

void SCDAwidget::incomingPullSignal(const int& pullType)
{
	auto app = Wt::WApplication::instance();
	string sessionID = app->sessionId();
	Wt::WString wsTemp;
	vector<string> prompt;
	switch (pullType)
	{
	case 0:  // Pull category.
		prompt.resize(2);
		prompt[0] = sessionID;
		wsTemp = wjConfig->wjcYear->wcbTitle->currentText();
		prompt[1] = wsTemp.toUTF8();
		sRef.pullCategory(prompt);
		break;
	}
}
void SCDAwidget::incomingResetSignal(const int& resetType)
{
	switch (resetType)
	{
	case 0:
		resetMap();
		break;
	case 1:
		resetTable();
		break;
	case 2:
		resetTopicSel();
		break;

	}
}
void SCDAwidget::init()
{
	this->clear();
	auto hLayout = make_unique<Wt::WHBoxLayout>();

	auto boxConfigUnique = makeBoxConfig(boxConfig);  // RESUME HERE
	hLayout->addWidget(move(boxConfigUnique));
	auto boxDataUnique = makeBoxData(boxData);
	hLayout->addWidget(move(boxDataUnique));

	makeUI();
	connect();
	initUI();

	mapTimeWord.emplace(0, "seconds");
	mapTimeWord.emplace(1, "minutes");
	mapTimeWord.emplace(2, "hours");
	mapTimeWord.emplace(3, "days");
	mapTimeWord.emplace(4, "weeks");
	mapTimeWord.emplace(5, "months");
	mapTimeWord.emplace(6, "years");

	Wt::WString wsTooltip = "Single-click a table cell to highlight it.\nDouble-click a table cell to load a new map using that column and row.";
	mapTooltip.emplace("table", wsTooltip);

	mapUnit.emplace("percentage", "%");
	mapUnit.emplace("number", "#");

	if (jsEnabled)
	{
		string getInfoJS = jsMakeFunctionTableWidth(tableData, "tableData");
		app->declareJavaScriptFunction("getInfo", getInfoJS);
		string scrollToJS = jsMakeFunctionTableScrollTo(tableData);
		app->declareJavaScriptFunction("scrollTo", scrollToJS);
	}

	int iWidth = getWidth();
	int iHeight = getHeight();
	if (iHeight > iWidth) { toggleMobile(); }
	tableData->setMaximumSize(len780p, iHeight - 80);
	tableData->setOverflow(Wt::Overflow::Auto, Wt::Orientation::Horizontal | Wt::Orientation::Vertical);

	jf.timerStart();
	cbYearChanged();
}
void SCDAwidget::initUI()
{
	Wt::WString wstemp;
	string temp;
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt = { app->sessionId() };

	// Colourful things.
	colourSelectedWeak.setRgb(200, 200, 255);
	colourSelectedStrong.setRgb(150, 150, 192);
	colourWhite.setRgb(255, 255, 255, 255);

	// Box names.
	boxConfig->setObjectName("boxConfig");
	boxData->setObjectName("boxData");

	// Initial values for box sizes.
	boxConfig->setMaximumSize(len200p, wlAuto);
	boxConfig->setMinimumSize(len300p, wlAuto);
	boxData->setMaximumSize(len800p, wlAuto);

	// Initialize often-used decoration styles.
	wcssDefault = Wt::WCssDecorationStyle();
	Wt::WBorder wBorder = Wt::WBorder(Wt::BorderStyle::Dotted);
	wcssAttention.setBorder(wBorder);
	wcssHighlighted.setBackgroundColor(colourSelectedWeak);

	// Initial values for the column panel.
	temp = "panelCol";
	function<void()> fnColTopicTitle = bind(&SCDAwidget::cbColRowTitleClicked, this, temp);
	cbColTopicTitle->changed().connect(fnColTopicTitle);
	panelColTopic->setMaximumSize(wlAuto, len250p);
	panelColTopic->setHidden(1);
	cbColTopicSel->changed().connect(this, &SCDAwidget::cbColRowSelChanged);
	cbColTopicSel->clicked().connect(this, &SCDAwidget::cbColRowSelClicked);
	pbSubsectionFilterCol->clicked().connect(this, &SCDAwidget::dialogSubsectionFilterCol);


	// Initial values for the row panel.
	temp = "panelRow";
	function<void()> fnRowTopicTitle = bind(&SCDAwidget::cbColRowTitleClicked, this, temp);
	cbRowTopicTitle->changed().connect(fnRowTopicTitle);
	panelRowTopic->setMaximumSize(wlAuto, len200p);
	panelRowTopic->setHidden(1);
	cbRowTopicSel->changed().connect(this, &SCDAwidget::cbColRowSelChanged);
	cbRowTopicSel->clicked().connect(this, &SCDAwidget::cbColRowSelClicked);
	pbSubsectionFilterRow->clicked().connect(this, &SCDAwidget::dialogSubsectionFilterRow);

	// Initial values for the tab widget.
	int numTab = tabData->count();
	for (int ii = 0; ii < numTab; ii++)
	{
		tabData->setTabEnabled(ii, 0);
	}
	tabData->setCurrentIndex(0);

	// Initial values for the region tree widget.
	treeRegion->itemSelectionChanged().connect(this, &SCDAwidget::treeClicked);

	// Initial values for the map tab.
	textUnit->setTextAlignment(Wt::AlignmentFlag::Middle);
	boxMap->setContentAlignment(Wt::AlignmentFlag::Center);

	// Initial values for the download tab.
	//pbDownloadPDF->clicked().connect(this, std::bind(&SCDAwidget::downloadMap, this));

	// Initial values for the miscellaneous buttons.
	pbMobile->setEnabled(1);
	pbMobile->setMaximumSize(wlAuto, len50p);
	pbMobile->clicked().connect(this, &SCDAwidget::toggleMobile);

}
string SCDAwidget::jsMakeFunctionTableScrollTo(WJTABLE*& boxTable)
{
	// Given a number of pixels displaced from the left, scroll to that position.
	string sID = boxTable->id();
	vector<string> vsID = { sID };
	int treeIndex = jtWidget.getIndex(sID);
	vector<int> viAncestor = jtWidget.treeSTanc[treeIndex];
	for (int ii = viAncestor.size() - 1; ii >= 0; ii--)
	{
		sID = jtWidget.treePL[viAncestor[ii]];
		vsID.push_back(sID);
	}
	string sII;
	int numID = vsID.size();
	string scrollTo = "function scrollTo(fromLeft) { var start = document.getElementById(\"";
	scrollTo += vsID.back() + "\"); var coll" + to_string(numID - 1) + " = start.children; ";
	for (int ii = numID - 2; ii >= 0; ii--)
	{
		sII = to_string(ii);
		scrollTo += "var node" + sII + " = coll" + to_string(ii + 1);
		scrollTo += ".namedItem(\"" + vsID[ii] + "\"); ";
		scrollTo += "var coll" + sII + " = node" + sII + ".children; ";
	}
	scrollTo += "node0.scrollLeft = fromLeft; }";
	return scrollTo;
}
string SCDAwidget::jsMakeFunctionTableWidth(WJTABLE*& boxTable, string tableID)
{
	// Given the sID of a table (and its immediate parent), return a string that 
	// defines a JavaScript function to return that table's width in pixels. 
	string sID = boxTable->id();
	vector<string> vsID = { tableID, sID };
	int treeIndex = jtWidget.getIndex(sID);
	vector<int> viAncestor = jtWidget.treeSTanc[treeIndex];
	for (int ii = viAncestor.size() - 1; ii >= 0; ii--)
	{
		sID = jtWidget.treePL[viAncestor[ii]];
		vsID.push_back(sID);
	}
	
	string sII;
	int numID = vsID.size();
	string getInfo = "function getInfo() { var sInfo = \"\"; var start = document.getElementById(\"";
	getInfo += vsID.back() + "\"); var coll" + to_string(numID - 1) + " = start.children; ";
	for (int ii = numID - 2; ii >= 0; ii--)
	{
		sII = to_string(ii);
		getInfo += "var node" + sII + " = coll" + to_string(ii + 1);
		getInfo += ".namedItem(\"" + vsID[ii] + "\"); ";
		getInfo += "var coll" + sII + " = node" + sII + ".children; ";
	}	
	getInfo += "let tableRect = node0.getBoundingClientRect(); ";
	getInfo += "var tableWidth = tableRect.width; ";
	getInfo += "Wt.emit('SCDAwidget', 'jsTWidth', tableWidth); }";
	return getInfo;
}

unique_ptr<Wt::WContainerWidget> SCDAwidget::makeBoxConfig(Wt::WContainerWidget*& wBox)
{
	auto boxConfigUnique = make_unique<Wt::WContainerWidget>();
	boxConfigUnique->setObjectName("boxConfig");
	wBox = boxConfigUnique.get();

	auto pbMobileUnique = make_unique<Wt::WPushButton>("Toggle Mobile Version");
	pbMobile = pbMobileUnique.get();
	allPB.push_back(pbMobile);
	auto textCataUnique = make_unique<Wt::WText>();
	textCata = textCataUnique.get();

	auto panelYearUnique = makePanelYear({ "2016" });  // Currently the only available year.
	auto panelCategoryUnique = makePanelCategory();
	auto panelTopicColUnique = makePanelTopicCol();
	auto panelTopicRowUnique = makePanelTopicRow();

	auto vLayoutConfig = make_unique<Wt::WVBoxLayout>();
	vLayoutConfig->addWidget(move(pbMobileUnique));
	vLayoutConfig->addWidget(move(textCataUnique));
	vLayoutConfig->addWidget(move(panelYearUnique));
	vLayoutConfig->addWidget(move(panelCategoryUnique));
	vLayoutConfig->addWidget(move(panelTopicColUnique));
	vLayoutConfig->addWidget(move(panelTopicRowUnique));
	numPreVariable = vLayoutConfig->count();
}
unique_ptr<Wt::WContainerWidget> SCDAwidget::makeBoxData(Wt::WContainerWidget*& wBox)
{
	auto boxDataUnique = make_unique<Wt::WContainerWidget>();
	boxDataUnique->setObjectName("boxData");
	wBox = boxDataUnique.get();

	auto tabDataUnique = make_unique<Wt::WTabWidget>();
	tabDataUnique->setObjectName("tabData");
	tabData = tabDataUnique.get();
	stackedTabData = tabData->contentsStack();

	auto treeRegionUnique = makeTreeRegion(treeRegion);
	auto tableDataUnique = makeTableData(tableData);
	auto boxTableUnique = make_unique<Wt::WContainerWidget>();
	boxTableUnique->setObjectName("boxTable");
	boxTable = boxTableUnique.get();

	tabDataUnique->addTab(move(treeRegionUnique), "Geographic Region", Wt::ContentLoading::Eager);


}
unique_ptr<WJTABLE> SCDAwidget::makeTableData(WJTABLE*& wjTable)
{

}
unique_ptr<Wt::WTree> SCDAwidget::makeTreeRegion(Wt::WTree*& wTree)
{
	auto treeRegionUnique = make_unique<Wt::WTree>();
	treeRegionUnique->setObjectName("treeRegion");
	wTree = treeRegionUnique.get();
	return treeRegionUnique;
}
void SCDAwidget::makeUI()
{
	auto cbColTopicSelUnique = make_unique<Wt::WComboBox>();
	cbColTopicSel = cbColTopicSelUnique.get();
	auto boxFilterColUnique = make_unique<Wt::WContainerWidget>();
	auto layoutFilterCol = make_unique<Wt::WVBoxLayout>();
	auto pbSubsectionFilterColUnique = make_unique<Wt::WPushButton>("Apply Subsection Filter");
	pbSubsectionFilterCol = pbSubsectionFilterColUnique.get();
	allPB.push_back(pbSubsectionFilterCol);

	auto cbRowTopicSelUnique = make_unique<Wt::WComboBox>();
	cbRowTopicSel = cbRowTopicSelUnique.get();
	allCB.push_back(cbRowTopicSel);
	cbID = cbRowTopicSel->id();
	jtWidget.addChild(cbID, -2, boxRowTopicUnique->id());
	auto boxFilterRowUnique = make_unique<Wt::WContainerWidget>();
	auto layoutFilterRow = make_unique<Wt::WVBoxLayout>();
	auto pbSubsectionFilterRowUnique = make_unique<Wt::WPushButton>("Apply Subsection Filter");
	pbSubsectionFilterRow = pbSubsectionFilterRowUnique.get();
	allPB.push_back(pbSubsectionFilterRow);

	auto boxTableUnique = make_unique<Wt::WContainerWidget>();
	boxTableUnique->setObjectName("boxTable");
	boxTable = boxTableUnique.get();
	auto tableDataUnique = make_unique<WJTABLE>();
	tableDataUnique->setObjectName("tableData");
	tableData = tableDataUnique.get();

	boxTableUnique->addWidget(move(tableDataUnique));
	tabDataUnique->addTab(move(boxTableUnique), "Data Table", Wt::ContentLoading::Eager);

	auto phantom1 = stackedTabData->widget(1);
	jtWidget.addChild(phantom1->id(), -2, stackedTabData->id());
	jtWidget.addChild(boxTable->id(), -2, phantom1->id());
	jtWidget.addChild(tableData->id(), -2, boxTable->id());

	auto boxMapAllUnique = make_unique<Wt::WContainerWidget>();
	boxMapAll = boxMapAllUnique.get();
	auto boxMapOptionUnique = make_unique<Wt::WContainerWidget>();
	boxMapOption = boxMapOptionUnique.get();
	auto textUnitUnique = make_unique<Wt::WText>();
	textUnit = textUnitUnique.get();
	auto pbUnitUnique = make_unique<Wt::WPushButton>();
	pbUnit = pbUnitUnique.get();
	auto popupUnitUnique = make_unique<Wt::WPopupMenu>();
	popupUnit = popupUnitUnique.get();
	auto boxMapUnique = make_unique<Wt::WContainerWidget>();
	boxMap = boxMapUnique.get();
	auto boxTextLegendUnique = make_unique<Wt::WContainerWidget>();
	boxTextLegend = boxTextLegendUnique.get();

	auto hLayoutMap = make_unique<Wt::WHBoxLayout>();
	pbUnitUnique->setMenu(move(popupUnitUnique));
	hLayoutMap->addWidget(move(textUnitUnique));
	hLayoutMap->addWidget(move(pbUnitUnique));
	hLayoutMap->addStretch(1);
	boxMapOptionUnique->setLayout(move(hLayoutMap));

	auto vLayoutMap = make_unique<Wt::WVBoxLayout>();
	vLayoutMap->addWidget(move(boxMapOptionUnique));
	vLayoutMap->addWidget(move(boxMapUnique));
	vLayoutMap->addWidget(move(boxTextLegendUnique));
	boxMapAllUnique->setLayout(move(vLayoutMap));
	tabDataUnique->addTab(move(boxMapAllUnique), "Data Map");

	auto phantom2 = stackedTabData->widget(2);
	jtWidget.addChild(phantom2->id(), -2, stackedTabData->id());
	jtWidget.addChild(boxMapAll->id(), -2, phantom2->id());
	jtWidget.addChild(boxMapOption->id(), -2, boxMapAll->id());
	jtWidget.addChild(textUnit->id(), -2, boxMapOption->id());
	jtWidget.addChild(pbUnit->id(), -2, boxMapOption->id());
	jtWidget.addChild(popupUnit->id(), -2, pbUnit->id());
	jtWidget.addChild(boxMap->id(), -2, boxMapAll->id());
	jtWidget.addChild(boxTextLegend->id(), -2, boxMapAll->id());

	auto boxDownloadUnique = make_unique<Wt::WContainerWidget>();
	boxDownload = boxDownloadUnique.get();
	auto pbDownloadPDFUnique = make_unique<Wt::WPushButton>("PDF");
	pbDownloadPDF = pbDownloadPDFUnique.get();
	boxDownloadUnique->addWidget(move(pbDownloadPDFUnique));
	//tabDataUnique->addTab(move(boxDownloadUnique), "Download");

	boxCBYearUnique->addWidget(move(cbYearUnique));
	panelYearUnique->setCentralWidget(move(boxCBYearUnique));

	boxCBCategoryUnique->addWidget(move(cbCategoryUnique));
	panelCategoryUnique->setCentralWidget(move(boxCBCategoryUnique));

	boxColTopicUnique->addWidget(move(cbColTopicTitleUnique));
	boxColTopicUnique->addWidget(move(cbColTopicSelUnique));
	layoutFilterCol->addWidget(move(pbSubsectionFilterColUnique));
	boxFilterColUnique->setLayout(move(layoutFilterCol));
	boxColTopicUnique->addWidget(move(boxFilterColUnique));
	panelColTopicUnique->setCentralWidget(move(boxColTopicUnique));

	boxRowTopicUnique->addWidget(move(cbRowTopicTitleUnique));
	boxRowTopicUnique->addWidget(move(cbRowTopicSelUnique));
	layoutFilterRow->addWidget(move(pbSubsectionFilterRowUnique));
	boxFilterRowUnique->setLayout(move(layoutFilterRow));
	boxRowTopicUnique->addWidget(move(boxFilterRowUnique));
	panelRowTopicUnique->setCentralWidget(move(boxRowTopicUnique));

	vLayoutConfig->addWidget(move(pbMobileUnique));
	vLayoutConfig->addWidget(move(textCataUnique));
	vLayoutConfig->addWidget(move(panelYearUnique));
	vLayoutConfig->addWidget(move(panelCategoryUnique));
	vLayoutConfig->addWidget(move(panelColTopicUnique));
	vLayoutConfig->addWidget(move(panelRowTopicUnique));
	numPreVariable = vLayoutConfig->count();
	boxConfigUnique->setLayout(move(vLayoutConfig));

	boxDataUnique->addWidget(move(tabDataUnique));

	hLayout->addWidget(move(boxConfigUnique), 1);
	hLayout->addWidget(move(boxDataUnique), 2);
	this->setLayout(move(hLayout));
}
void SCDAwidget::mapAreaClicked(int areaIndex)
{
	string sRegionClicked = wtMap->areaClicked(areaIndex);
	if (sRegionClicked == "bgArea")
	{
		auto selSet = treeRegion->selectedNodes();
		if (selSet.size() < 1) { return; }
		auto selIt = selSet.begin();
		auto selNode = *selIt;
		auto wTemp = selNode->label()->text();
		string sRegion = wTemp.toUTF8();
		sRegionClicked = jtRegion.getParent(sRegion);
		if (sRegionClicked.size() < 1) { return; }
	}
	string sID = jtRegion.mapSID.at(sRegionClicked);
	Wt::WTreeNode* nodeSel = (Wt::WTreeNode*)treeRegion->findById(sID);
	if (!treeRegion->isSelected(nodeSel)) { treeRegion->select(nodeSel); }
}

void SCDAwidget::populateTextLegend(Wt::WContainerWidget*& boxTextLegend)
{
	// Read the topic and parameter panels, and make a list of the selected options. 
	boxTextLegend->clear();
	auto vLayout = make_unique<Wt::WVBoxLayout>();
	Wt::WColor wGrey = Wt::WColor(210, 210, 210);
	bool grey = 0;

	Wt::WString wsTemp = panelYear->title() + ":  " + getTextLegend(panelYear);
	auto wText = make_unique<Wt::WText>(wsTemp);
	wText->setTextAlignment(Wt::AlignmentFlag::Left);
	wText->decorationStyle().font().setSize(Wt::FontSize::Large);
	if (grey)
	{
		wText->decorationStyle().setBackgroundColor(wGrey);
		grey = 0;
	}
	else { grey = 1; }
	vLayout->addWidget(move(wText));

	wsTemp = panelCategory->title() + ":  " + getTextLegend(panelCategory);
	wText = make_unique<Wt::WText>(wsTemp);
	wText->setTextAlignment(Wt::AlignmentFlag::Left);
	wText->decorationStyle().font().setSize(Wt::FontSize::Large);
	if (grey)
	{
		wText->decorationStyle().setBackgroundColor(wGrey);
		grey = 0;
	}
	else { grey = 1; }
	vLayout->addWidget(move(wText));

	wsTemp = panelColTopic->title() + ":  " + getTextLegend(panelColTopic);
	wText = make_unique<Wt::WText>(wsTemp);
	wText->setTextAlignment(Wt::AlignmentFlag::Left);
	wText->decorationStyle().font().setSize(Wt::FontSize::Large);
	if (grey)
	{
		wText->decorationStyle().setBackgroundColor(wGrey);
		grey = 0;
	}
	else { grey = 1; }
	vLayout->addWidget(move(wText));

	wsTemp = panelRowTopic->title() + ":  " + getTextLegend(panelRowTopic);
	wText = make_unique<Wt::WText>(wsTemp);
	wText->setTextAlignment(Wt::AlignmentFlag::Left);
	wText->decorationStyle().font().setSize(Wt::FontSize::Large);
	if (grey)
	{
		wText->decorationStyle().setBackgroundColor(wGrey);
		grey = 0;
	}
	else { grey = 1; }
	vLayout->addWidget(move(wText));

	if (panelDemographic != nullptr)
	{
		wsTemp = panelDemographic->title() + ":  " + getTextLegend(panelDemographic);
		wText = make_unique<Wt::WText>(wsTemp);
		wText->setTextAlignment(Wt::AlignmentFlag::Left);
		wText->decorationStyle().font().setSize(Wt::FontSize::Large);
		if (grey)
		{
			wText->decorationStyle().setBackgroundColor(wGrey);
			grey = 0;
		}
		else { grey = 1; }
		vLayout->addWidget(move(wText));
	}

	for (int ii = 0; ii < varPanel.size(); ii++)
	{
		wsTemp = varPanel[ii]->title() + ":  " + getTextLegend(varPanel[ii]);
		wText = make_unique<Wt::WText>(wsTemp);
		wText->setTextAlignment(Wt::AlignmentFlag::Left);
		wText->decorationStyle().font().setSize(Wt::FontSize::Large);
		if (grey)
		{
			wText->decorationStyle().setBackgroundColor(wGrey);
			grey = 0;
		}
		else { grey = 1; }
		vLayout->addWidget(move(wText));
	}

	boxTextLegend->setLayout(move(vLayout));
}
void SCDAwidget::populateTree(JTREE& jt, Wt::WTreeNode*& node)
{
	// Recursive function that takes an existing node and makes its children.
	vector<string> vsChildren;
	vector<int> viChildren;
	Wt::WString wTemp = node->label()->text();
	string sNode = wTemp.toUTF8();
	if (jt.isExpanded(sNode)) { node->expand(); }
	string sID = node->id();
	jt.mapSID.emplace(sNode, sID);
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
	Wt::WApplication* app = Wt::WApplication::instance();
	app->triggerUpdate();
	int etype = event.type();
	switch (etype)
	{
	case 0:  // Catalogue: store it in memory, and launch a Variable event.
		updateTextCata(event.getNumCata());
		processEventCatalogue(event.getSYear(), event.getSCata());
		break;
	case 1:  // Category: display options to the user on a new panel.
		updateTextCata(event.getNumCata());
		processEventCategory(event.get_list());
	case 2:  // Connect.
		break;
	case 3:  // Demographic: display options to the user on a new panel.
		updateTextCata(event.getNumCata());
		processEventDemographic(event.getVariable());
		break;
	case 4:  // Differentiation: create a single parameter panel, chosen to specify the catalogue.
		updateTextCata(event.getNumCata());
		addDifferentiator(event.get_list());
		break;
	case 5:  // Map: display it on the painter widget.
		processEventMap(event.get_list(), event.get_areas(), event.getRegionData());
		break;
	case 6:  // Parameter: create new panels with options for the user to specify.
		updateTextCata(event.getNumCata());
		processEventParameter(event.getVariable(), event.getCata(), event.get_list());
		break;
	case 7:  // Table: populate the widget with data.
		processEventTable(event.getTable(), event.getListCol(), event.getListRow());
		break;
	case 8:  // Topic: update the GUI with row/column options.
		updateTextCata(event.getNumCata());
		processEventTopic(event.getListRow(), event.getListCol());
		break;
	case 9:  // Tree: populate the tree tab using the JTREE object.
		updateTextCata(event.getNumCata());
		processEventTree(event.getTree());
		break;
	}
}
void SCDAwidget::processEventCatalogue(string sYear, string sCata)
{
	activeYear = sYear;
	activeCata = sCata;
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(6);
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCategory;
	prompt[3] = activeColTopic;
	prompt[4] = activeRowTopic;
	prompt[5] = activeCata;
	vector<vector<string>> vvsVariable = getVariable();
	sRef.pullVariable(prompt, vvsVariable);
}
void SCDAwidget::processEventCategory(vector<string> vsCategory)
{
	vsCategory.insert(vsCategory.begin(), "[Choose a topical category]");
	cbjCategory->addCB(vsCategory);
	cbjCategory->vCB[0]->changed().connect(this, &SCDAwidget::cbCategoryChanged);
	cbjCategory->setVisible(1);
	cbjCategory->highlight(1);
	widgetMobile();
}
void SCDAwidget::processEventDemographic(vector<vector<string>> vvsDemo)
{
	vvsDemographic = vvsDemo;
	resetVariables(1);
	if (vvsDemo.size() > 1)
	{
		addDemographic(vvsDemo);
	}
	else
	{
		Wt::WApplication* app = Wt::WApplication::instance();
		vector<string> prompt = { app->sessionId() };
		vector<string> vsCata;
		vsCata.assign(vvsDemo[0].begin() + 1, vvsDemo[0].end());
		sRef.pullDifferentiator(prompt, vsCata);
	}
}
void SCDAwidget::processEventMap(vector<string> vsRegion, vector<vector<vector<double>>> vvvdArea, vector<vector<double>> vvdData)
{
	boxMap->clear();
	auto popupItems = popupUnit->items();
	for (int ii = 0; ii < popupItems.size(); ii++)
	{
		popupUnit->removeItem(popupItems[ii]);
	}
	auto wtMapUnique = make_unique<WTPAINT>();
	wtMap = boxMap->addWidget(move(wtMapUnique));
	populateTextLegend(boxTextLegend);

	Wt::WString wsUnit;
	Wt::WString wsUnitOld = textUnit->text();
	string sUnitOld = wsUnitOld.toUTF8();
	size_t pos1 = sUnitOld.find(':');
	if (pos1 < sUnitOld.size())
	{
		sUnitOld = sUnitOld.substr(pos1 + 2);
	}
	string sUnit = getUnit();
	function<void()> fnUnit1 = bind(&SCDAwidget::updateUnit, this, sUnit);
	popupUnit->addItem(sUnit)->triggered().connect(fnUnit1);
	if (sUnit == "# of persons")
	{
		string temp = "% of population";
		function<void()> fnUnit2 = bind(&SCDAwidget::updateUnit, this, temp);
		popupUnit->addItem(temp)->triggered().connect(fnUnit2);

		if (sUnitOld == sUnit)  // Keep this unit if it had been chosen previously.
		{
			wtMap->setUnit(sUnit, { 0 });
			wsUnit = "Unit: " + sUnit;
		}
		else  // Default to this unit.
		{
			wtMap->setUnit(sUnit, { 0, 1 });
			wsUnit = "Unit: " + temp;
		}
	}
	else 
	{ 
		wtMap->setUnit(sUnit, { 0 }); 
		wsUnit = "Unit: " + sUnit;
	}
	textUnit->setText(wsUnit);

	vector<Wt::WPolygonArea*> area = wtMap->drawMap(vvvdArea, vsRegion, vvdData);
	for (int ii = 0; ii < area.size(); ii++)
	{
		function<void()> fnArea = bind(&SCDAwidget::mapAreaClicked, this, ii);
		area[ii]->clicked().connect(fnArea);
	}
	tabData->setTabEnabled(2, 1);
	//tabData->setTabEnabled(3, 1);
	if (first) { tabData->setCurrentIndex(2); }	
	first = 0;
	widgetMobile();
	loadingStop();
}
void SCDAwidget::processEventParameter(vector<vector<string>> vvsVariable, vector<vector<string>> vvsCata, vector<string> vsDIMIndex)
{
	// vvsVariable has form [variable index][MID0, MID1, ..., variable title].
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

		Wt::WString wTemp;
		if (!index) { vvsParameter = vvsVariable; }
		else
		{
			for (int ii = 0; ii < varMID.size(); ii++)
			{
				wTemp = varMID[ii]->currentText();
				if (wTemp != "") { break; }
				else if (ii == varMID.size() - 1)
				{
					if (cbDemographic != nullptr) { resetVariables(1); }
					else { resetVariables(0); }
					activeCata = vvsCata[0][1];
					vvsParameter = vvsVariable;
				}
			}
		}

		vector<string> vsTemp(2);
		for (int ii = 0; ii < vvsVariable.size(); ii++)
		{
			vsTemp[0] = vvsVariable[ii].back();  // Title.
			vsTemp[1] = vvsVariable[ii][0];  // Default MID.
			addVariable(vsTemp);
		}
		for (int ii = 0; ii < varTitle.size(); ii++)
		{
			varTitle[ii]->setEnabled(0);
		}
	}
	else
	{
		// Multiple catalogues satisfy the Year, Category, Row, and Column criteria.
		// Check vvsVariable[0] for the type of differentiation to employ.
		if (vvsVariable[0].size() != 1) { jf.err("Invalid vvsCandidate-SCDAwidget.pDE(Variable)"); }
		addVariable(vvsVariable);
		int bbq = 1;
	}

	// If there are no more unspecified variables, populate the region tree tab.
	if (varPanel.size() == numVar)
	{
		Wt::WApplication* app = Wt::WApplication::instance();
		app->processEvents();
		activeYear = vvsCata[0][0];
		activeCata = vvsCata[0][1];
		vector<string> promptTree(3);
		promptTree[0] = app->sessionId();
		promptTree[1] = vvsCata[0][0];
		promptTree[2] = vvsCata[0][1];
		sRef.pullTree(promptTree);
	}
}
void SCDAwidget::processEventTable(vector<vector<string>> vvsTable, vector<string> vsCol, vector<string> vsRow)
{
	if (vsCol.size() < 3 || vsRow.size() < 3) { jf.err("Missing col/row titles-SCDAwidget.processEventTable"); }
	if (vvsTable.size() < 1) { jf.err("Missing table data-SCDAwidget.processEventTable"); }

	auto app = Wt::WApplication::instance();
	activeColTopic = vsCol.back();
	vsCol.pop_back();
	activeRowTopic = vsRow.back();
	vsRow.pop_back();
	tabData->setTabEnabled(1, 1);
	string temp = "Data Table (" + vsCol[0] + ")";
	Wt::WString wTemp = Wt::WString::fromUTF8(temp);
	auto tab = tabData->itemAt(1);
	tab->setText(wTemp);

	// Populate the table widget.
	tableData->reset(vvsTable.size(), vvsTable[0].size() + 1);
	tableData->modelSetTopLeft(vsCol[0]);
	vsCol.erase(vsCol.begin());
	int colMaxIndent = tableData->modelSetTop(vsCol);
	int rowMaxIndent = tableData->modelSetLeft(vsRow);
	tableData->modelSetCore(vvsTable);
	tableData->display();

	// Populate cbColTopicSel and cbRowTopicSel if necessary.
	if (cbRowTopicSel->isHidden() || cbColTopicSel->isHidden())
	{
		cbColTopicSel->setHidden(0);
		pbSubsectionFilterCol->setHidden(0);
		cbRenew(cbColTopicSel, vsCol);

		cbRowTopicSel->setHidden(0);
		pbSubsectionFilterRow->setHidden(0);
		cbRenew(cbRowTopicSel, vsRow);
	}

	// Add a tooltip to the table.
	wTemp = mapTooltip.at("table");
	tableData->setToolTip(wTemp);

	// Experimental...
	if (jsEnabled)
	{
		app->doJavaScript(app->javaScriptClass() + ".getInfo();");
		double dBoxTableWidth = boxData->maximumWidth().value();
		dBoxTableWidth += boxTableSlider->padding(Wt::Side::Left).value() + boxTableSlider->padding(Wt::Side::Right).value();
		boxTableWidth = int(ceil(dBoxTableWidth));
		app->doJavaScript(app->javaScriptClass() + ".scrollTo(0);");
		app->triggerUpdate();
	}

	// Connect table cell signals/slots.
	if (!tableData->vClick().isConnected())
	{
		tableData->vClick().connect(this, std::bind(&SCDAwidget::virtualClick, this, std::placeholders::_1));
	}
	//tabData->setCurrentIndex(1);
	app->processEvents();
	cbColRowSelClicked();
	cbColRowSelChanged();  // Triggers table current selection. 
}
void SCDAwidget::processEventTopic(vector<string> vsRowTopic, vector<string> vsColTopic)
{
	Wt::WApplication* app = Wt::WApplication::instance();
	string rowDefault = "[Choose a row topic]";
	if (vsRowTopic.size() == 1)
	{
		activeRowTopic = vsRowTopic[0];
	}
	vsRowTopic.insert(vsRowTopic.begin(), rowDefault);
	cbjTopicRow->setCB(vsRowTopic, 0, activeRowTopic);

	string colDefault = "[Choose a column topic]";
	if (vsColTopic.size() == 1)
	{
		activeColTopic = vsColTopic[0];
	}
	vsColTopic.insert(vsColTopic.begin(), colDefault);
	cbjTopicCol->setCB(vsColTopic, 0, activeColTopic);

	cbjCategory->highlight(0);
	if (vsColTopic.size() == 1 && vsRowTopic.size() == 1)
	{
		cbjTopicCol->highlight(0);
		cbjTopicRow->highlight(0);
		vector<vector<string>> vvsVariable;
		vector<string> prompt(5);
		prompt[0] = app->sessionId();
		prompt[1] = activeYear;
		prompt[2] = activeCategory;
		prompt[3] = activeColTopic;
		prompt[4] = activeRowTopic;
		sRef.pullVariable(prompt, vvsVariable);
	}
	else if (vsColTopic.size() == 1)
	{
		cbjTopicCol->highlight(0);
		cbjTopicRow->highlight(1);
	}
	else if (vsRowTopic.size() == 1)
	{
		cbjTopicCol->highlight(1);
		cbjTopicRow->highlight(0);
	}
	else
	{
		cbjTopicCol->highlight(1);
		cbjTopicRow->highlight(1);
	}
	widgetMobile();
}
void SCDAwidget::processEventTree(JTREE jt)
{
	jtRegion.clear();
	jtRegion = jt;
	string sRoot = jtRegion.getRootName();
	Wt::WString wTemp = Wt::WString::fromUTF8(sRoot);
	auto treeRootUnique = make_unique<Wt::WTreeNode>(wTemp);
	treeRootUnique->setLoadPolicy(Wt::ContentLoading::Eager);
	auto treeRoot = treeRootUnique.get();
	populateTree(jtRegion, treeRoot);
	treeRegion->setSelectionMode(Wt::SelectionMode::Single);
	treeRegion->setTreeRoot(move(treeRootUnique));
	tabData->setTabEnabled(0, 1);
	treeRoot = treeRegion->treeRoot();
	treeRoot->expand();
	treeRegion->select(treeRoot);
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
void SCDAwidget::resetMap()
{
	boxMap->clear();
	tabData->setTabEnabled(2, 0);
	//tabData->setTabEnabled(3, 0);
	tabData->setCurrentIndex(0);
}
void SCDAwidget::resetTable()
{
	if (tableData == nullptr) { return; }
	tableData->reset();
	Wt::WString wTemp("Data Table");
	auto tab = tabData->itemAt(1);
	tab->setText(wTemp);
	tabData->setTabEnabled(1, 0);
	tabData->setCurrentIndex(0);
}
void SCDAwidget::resetTopicSel()
{
	panelRowTopic->setHidden(0);
	cbRowTopicSel->clear();
	cbRowTopicSel->setHidden(1);
	pbSubsectionFilterRow->setHidden(1);
	panelColTopic->setHidden(0);
	cbColTopicSel->clear();
	cbColTopicSel->setHidden(1);
	pbSubsectionFilterCol->setHidden(1);
}
void SCDAwidget::resetTree()
{
	jtRegion.clear();
	treeRegion->setTreeRoot(make_unique<Wt::WTreeNode>(""));
}
void SCDAwidget::resetVariables()
{
	// Remove all "variable" panels and clear all "variable" buffers.
	int plus = 0;
	resetVariables(plus);
}
void SCDAwidget::resetVariables(int plus)
{
	// Remove all "variable" panels and clear all "variable" buffers.
	int numKeep = numPreVariable + plus;
	auto boxChildren = boxConfig->children();
	for (int ii = numKeep; ii < boxChildren.size(); ii++)
	{
		boxConfig->removeWidget(boxChildren[ii]);
	}
	activeCata.clear();
	vvsParameter.clear();
	varPanel.clear();
	varTitle.clear();
	varMID.clear();
	mapVarIndex.clear();
	if (plus < 1) 
	{ 
		vvsDemographic.clear();
		cbDemographic = nullptr;
	}
	Wt::WApplication* app = Wt::WApplication::instance();
	app->processEvents();
}

void SCDAwidget::setMap(int iRow, int iCol, string sRegion)
{
	// Assumes that all widgets have been updated already.
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(4);
	prompt[0] = app->sessionId();
	prompt[1] = activeYear;
	prompt[2] = activeCata;
	prompt[3] = sRegion;  // Parent region name. 

	vector<vector<string>> variable = getVariable();
	Wt::WString wsTemp = cbRowTopicSel->currentText();
	string temp = wsTemp.toUTF8();
	variable.push_back({ activeRowTopic, temp });
	variable.push_back({ activeColTopic, to_string(iCol) });
	sRef.pullMap(prompt, variable);
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
}

void SCDAwidget::tableCBUpdate(int iRow, int iCol)
{
	int count;
	Wt::WText *wText;
	Wt::WString wsCB, wsTable;
	if (iRow < 0) { cbRowTopicSel->setCurrentIndex(0); }
	else { cbRowTopicSel->setCurrentIndex(iRow); }
	if (iCol < 0) { cbColTopicSel->setCurrentIndex(0); }
	else { cbColTopicSel->setCurrentIndex(iCol); }
}
void SCDAwidget::tableReceiveDouble(const double& width)
{
	cout << "tableReceiveDouble: " << to_string(width) << endl;
	tableWidth = int(ceil(width));
}
void SCDAwidget::tableReceiveString(const string& sInfo)
{
	cout << "tableReceiveString: " << sInfo << endl;
	auto phantom = this->findById(sInfo);
	int bbq = 1;
}

void SCDAwidget::toggleMobile()
{
	if (mobile)
	{
		mobile = 0;
		boxConfig->setMaximumSize(len200p, wlAuto);
		boxData->setMaximumSize(len800p, wlAuto);
		widgetMobile();
		auto hLayout = make_unique<Wt::WHBoxLayout>();
		auto boxConfigUnique = this->removeWidget(boxConfig);
		auto boxDataUnique = this->removeWidget(boxData);
		hLayout->addWidget(move(boxConfigUnique));
		hLayout->addWidget(move(boxDataUnique));
		this->setLayout(move(hLayout));
	}
	else
	{
		mobile = 1;
		boxConfig->setMaximumSize(wlAuto, wlAuto);
		boxData->setMaximumSize(wlAuto, wlAuto);
		widgetMobile();
		auto vLayout = make_unique<Wt::WVBoxLayout>();
		auto boxConfigUnique = this->removeWidget(boxConfig);
		auto boxDataUnique = this->removeWidget(boxData);
		vLayout->addWidget(move(boxConfigUnique));
		vLayout->addWidget(move(boxDataUnique));
		this->setLayout(move(vLayout));
	}
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
void SCDAwidget::updateTextCata(int numCata)
{
	Wt::WString wsTemp;
	if (numCata < 1) { jf.err("Less than one catalogue remains-SCDAwidget.updateTextCata"); }
	else if (numCata == 1)
	{
		wsTemp = "Chosen catalogue is being displayed.";
	}
	else
	{
		wsTemp = to_string(numCata) + " catalogues remain - more parameters must be specified.";
	}
	textCata->setText(wsTemp);
}
void SCDAwidget::updateUnit(string sUnit)
{
	Wt::WString wTemp = Wt::WString::fromUTF8("Unit: " + sUnit);
	textUnit->setText(wTemp);
	vector<int> viIndex;
	if (sUnit == "% of population") { viIndex = { 0, 1 }; }
	else { viIndex = { 0 }; }
	wtMap->updateDisplay(viIndex);
}
void SCDAwidget::virtualClick(const int& vcType)
{
	switch (vcType)
	{
	case 0:
	{
		cbColRowSelChanged();
		break;
	}
	}
}
void SCDAwidget::widgetMobile()
{
	if (mobile)
	{
		for (int ii = 0; ii < allCB.size(); ii++)
		{
			if (allCB[ii] != nullptr)
			{
				allCB[ii]->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
				allCB[ii]->setMinimumSize(len300p, len200p);
			}
		}
		for (int ii = 0; ii < allPB.size(); ii++)
		{
			if (allPB[ii] != nullptr)
			{
				allPB[ii]->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
				allPB[ii]->setMinimumSize(wlAuto, len50p);
			}
		}
		for (int ii = 0; ii < varTitle.size(); ii++)
		{
			if (varTitle[ii] != nullptr)
			{
				varTitle[ii]->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
				varTitle[ii]->setMinimumSize(wlAuto, len50p);
			}
			if (varMID[ii] != nullptr)
			{
				varMID[ii]->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
				varMID[ii]->setMinimumSize(wlAuto, len50p);
			}
			if (varPanel[ii] != nullptr)
			{
				varPanel[ii]->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
				varPanel[ii]->setMinimumSize(wlAuto, len50p);
			}
		}
		for (int ii = 0; ii < allPanel.size(); ii++)
		{
			if (allPanel[ii] != nullptr)
			{
				allPanel[ii]->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
				allPanel[ii]->setMinimumSize(wlAuto, len50p);
			}
		}
		if (cbDemographic != nullptr)
		{
			panelDemographic->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
			panelDemographic->setMinimumSize(wlAuto, len50p);
			cbDemographic->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
			cbDemographic->setMinimumSize(wlAuto, len50p);
		}
		if (tabData != nullptr)
		{
			tabData->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
			tabData->setMinimumSize(wlAuto, len50p);
		}
		if (leTest != nullptr)
		{
			leTest->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
			leTest->setMinimumSize(len300p, len200p);
		}
	}
	else
	{
		for (int ii = 0; ii < allCB.size(); ii++)
		{
			if (allCB[ii] != nullptr)
			{
				allCB[ii]->decorationStyle().font().setSize(Wt::FontSize::Medium);
				allCB[ii]->setMinimumSize(wlAuto, wlAuto);
			}
		}
		for (int ii = 0; ii < allPB.size(); ii++)
		{
			if (allPB[ii] != nullptr)
			{
				allPB[ii]->decorationStyle().font().setSize(Wt::FontSize::Medium);
				allPB[ii]->setMinimumSize(wlAuto, wlAuto);
			}
		}
		for (int ii = 0; ii < varTitle.size(); ii++)
		{
			if (varTitle[ii] != nullptr)
			{
				varTitle[ii]->decorationStyle().font().setSize(Wt::FontSize::Medium);
				varTitle[ii]->setMinimumSize(wlAuto, wlAuto);
			}
			if (varMID[ii] != nullptr)
			{
				varMID[ii]->decorationStyle().font().setSize(Wt::FontSize::Medium);
				varMID[ii]->setMinimumSize(wlAuto, wlAuto);
			}
			if (varPanel[ii] != nullptr)
			{
				varPanel[ii]->decorationStyle().font().setSize(Wt::FontSize::Medium);
				varPanel[ii]->setMinimumSize(wlAuto, wlAuto);
			}
		}
		for (int ii = 0; ii < allPanel.size(); ii++)
		{
			if (allPanel[ii] != nullptr)
			{
				allPanel[ii]->decorationStyle().font().setSize(Wt::FontSize::Medium);
				allPanel[ii]->setMinimumSize(wlAuto, wlAuto);
			}
		}
		if (cbDemographic != nullptr && cbDemographic->count() > 0)
		{
			cbDemographic->decorationStyle().font().setSize(Wt::FontSize::Medium);
			cbDemographic->setMinimumSize(wlAuto, wlAuto);
		}
		if (tabData != nullptr)
		{
			tabData->decorationStyle().font().setSize(Wt::FontSize::Medium);
			tabData->setMinimumSize(wlAuto, wlAuto);
		}
		if (leTest != nullptr)
		{
			leTest->decorationStyle().font().setSize(Wt::FontSize::Medium);
			leTest->setMinimumSize(wlAuto, wlAuto);
		}
	}
}
