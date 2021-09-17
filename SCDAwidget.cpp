#include "SCDAwidget.h"

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
		wjConfig->headerSignal().connect(this, bind(&SCDAwidget::incomingHeaderSignal, this, placeholders::_1, placeholders::_2));
		wjConfig->pullSignal().connect(this, bind(&SCDAwidget::incomingPullSignal, this, placeholders::_1));
		wjConfig->resetSignal().connect(this, bind(&SCDAwidget::incomingResetSignal, this, placeholders::_1));
		wjConfig->pbMobile->clicked().connect(this, &SCDAwidget::toggleMobile);
		wjConfig->filterSignal().connect(this, bind(&SCDAwidget::incomingFilterSignal, this));
		if (jsEnabled)
		{
			this->jsTWidth.connect(this, std::bind(&SCDAwidget::tableReceiveDouble, this, std::placeholders::_1));
			this->jsInfo.connect(this, std::bind(&SCDAwidget::tableReceiveString, this, std::placeholders::_1));
		}
	}
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
		wjConfig->textCata->setText(wsCata);
	}
}
void SCDAwidget::downloadMap()
{
	auto app = Wt::WApplication::instance();
	string fileRoot = app->docRoot();
	string filePath = fileRoot + "/MapTest.pdf";
	wtMap->printPDF(filePath);
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
int SCDAwidget::getWidth()
{
	const Wt::WEnvironment& env = Wt::WApplication::instance()->environment();
	int iWidth = env.screenWidth();
	if (iWidth < 0) { jf.err("Failed to obtain widget dimensions-wtpaint.getDimensions"); }
	return iWidth;
}

void SCDAwidget::incomingFilterSignal()
{
	vector<vector<string>> vvsTable;
	vector<string> vsCol, vsRow;
	int indexCol, indexRow;
	bool filterCol = 0, filterRow = 0;
	vector<vector<int>> vviFilter = wjConfig->getFilterTable();
	if (vviFilter[0].size() > 0)
	{
		filterRow = 1;
		vsRow.resize(vviFilter[0].size());
		indexRow = 0;
		for (int ii = 0; ii < vsRow.size(); ii++)
		{
			vsRow[ii] = tableRow[vviFilter[0][indexRow]];
			indexRow++;
		}
	}
	else { vsRow = tableRow; }

	if (vviFilter[1].size() > 0)
	{
		filterCol = 1;
		vsCol.resize(vviFilter[1].size());
		indexCol = 0;
		for (int ii = 0; ii < vsCol.size(); ii++)
		{
			vsCol[ii] = tableCol[vviFilter[1][indexCol]];
			indexCol++;
		}
	}
	else { vsCol = tableCol; }

	vvsTable.resize(vsRow.size(), vector<string>(vsCol.size()));
	if (filterRow && filterCol)
	{
		indexRow = 0;
		for (int ii = 0; ii < vsRow.size(); ii++)
		{
			indexCol = 0;
			for (int jj = 0; jj < vsCol.size(); jj++)
			{
				vvsTable[ii][jj] = tableCore[vviFilter[0][indexRow]][vviFilter[1][indexCol]];
				indexCol++;
			}
			indexRow++;
		}
	}
	else if (filterCol)
	{
		for (int ii = 0; ii < vsRow.size(); ii++)
		{
			indexCol = 0;
			for (int jj = 0; jj < vsCol.size(); jj++)
			{
				vvsTable[ii][jj] = tableCore[ii][vviFilter[1][indexCol]];
				indexCol++;
			}
		}
	}
	else if (filterRow)
	{
		indexRow = 0;
		for (int ii = 0; ii < vsRow.size(); ii++)
		{
			vvsTable[ii] = tableCore[vviFilter[0][indexRow]];
			indexRow++;
		}
	}
	else { vvsTable = tableCore; }

	boxTable->clear();
	auto tableDataUnique = make_unique<WJTABLE>(vvsTable, vsCol, vsRow, tableRegion);
	tableData = tableDataUnique.get();
	boxTable->addWidget(move(tableDataUnique));
	tableData->setMaximumSize(780.0, screenHeight - 100);
	tableData->headerSignal().connect(this, bind(&SCDAwidget::incomingHeaderSignal, this, placeholders::_1, placeholders::_2));
}
void SCDAwidget::incomingHeaderSignal(const int& iRow, const int& iCol)
{
	// Either the config CBs or the data table is reporting a change in selected headers. 
	long long timeConfig = wjConfig->jf.timerReport();
	long long timeTable = tableData->jf.timerReport();

	// Update the CB widgets or the data table's selected cell, then load a map. 
	string sRegion = tableData->getCell(-1, 0);
	if (timeConfig < timeTable)  // CB values override table values. 
	{
		tableData->cellSelect(iRow, iCol + 1);
		setMap(iRow, iCol, sRegion);
	}
	else  // Table values override CB values. 
	{ 
		wjConfig->wjpTopicRow->cbMID->setCurrentIndex(iRow);
		if (iCol > 0)  // A row header was not clicked. 
		{ 
			wjConfig->wjpTopicCol->cbMID->setCurrentIndex(iCol - 1); 
			setMap(iRow, iCol - 1, sRegion);
		}
		else  // A row header was clicked. 
		{
			setMap(iRow, wjConfig->wjpTopicCol->cbMID->currentIndex(), sRegion);
		}
	}
}
void SCDAwidget::incomingPullSignal(const int& pullType)
{
	auto app = Wt::WApplication::instance();
	string sessionID = app->sessionId();
	Wt::WString wsTemp;
	vector<string> vsPrompt;
	vector<vector<string>> vvsPrompt;
	switch (pullType)
	{
	case 0:  // Pull category.
		vsPrompt.resize(2);
		vsPrompt[0] = sessionID;
		wsTemp = wjConfig->wjpYear->cbTitle->currentText();
		vsPrompt[1] = wsTemp.toUTF8();
		sRef.pullCategory(vsPrompt);
		break;
	case 1:  // Pull differentiator.
		wjConfig->getPrompt(vsPrompt, vvsPrompt);
		vsPrompt[0] = sessionID;
		sRef.pullDifferentiator(vsPrompt, vvsPrompt);
		break;
	case 2:  // Pull table.
	{
		wjConfig->getPrompt(vsPrompt, vvsPrompt);
		vsPrompt[0] = sessionID;
		vsPrompt[2] = activeCata;
		int geoCode;
		treeClicked(geoCode, vsPrompt[4]);
		vsPrompt[3] = to_string(geoCode);
		sRef.pullTable(vsPrompt, vvsPrompt);
		break;
	}
	case 3:  // Pull topic.
		wjConfig->getPrompt(vsPrompt);
		vsPrompt[0] = sessionID;
		sRef.pullTopic(vsPrompt);
		break;
	case 4:  // Pull variable.
		wjConfig->getPrompt(vsPrompt, vvsPrompt);
		vsPrompt[0] = sessionID;
		sRef.pullVariable(vsPrompt, vvsPrompt);
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
		resetTree();
		break;
	}
}
void SCDAwidget::init()
{
	this->clear();
	screenWidth = getWidth();
	screenHeight = getHeight();
	vector<string> vsYear = { "2016" };

	auto hLayout = make_unique<Wt::WHBoxLayout>();
	auto wjConfigUnique = make_unique<WJCONFIG>(vsYear);
	wjConfig = wjConfigUnique.get();
	hLayout->addWidget(move(wjConfigUnique));
	auto boxDataUnique = makeBoxData();
	hLayout->addWidget(move(boxDataUnique));
	this->setLayout(move(hLayout));

	connect();
	initUI();
	initMaps();
	if (screenHeight > screenWidth) { toggleMobile(); }

	auto app = Wt::WApplication::instance();
	if (jsEnabled)
	{
		string getInfoJS = jsMakeFunctionTableWidth(tableData, "tableData");
		app->declareJavaScriptFunction("getInfo", getInfoJS);
		string scrollToJS = jsMakeFunctionTableScrollTo(tableData);
		app->declareJavaScriptFunction("scrollTo", scrollToJS);
	}

	jf.timerStart();
	vector<string> prompt = { app->sessionId(), vsYear[0] };
	sRef.pullCategory(prompt);
}
void SCDAwidget::initMaps()
{
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
}
void SCDAwidget::initUI()
{
	Wt::WApplication* app = Wt::WApplication::instance();

	// Colourful things.
	wcGrey.setRgb(210, 210, 210);
	wcSelectedWeak.setRgb(200, 200, 255);
	wcSelectedStrong.setRgb(150, 150, 192);
	wcWhite.setRgb(255, 255, 255, 255);

	// Widget names.
	boxData->setObjectName("boxData");
	tabData->setObjectName("tabData");
	treeRegion->setObjectName("treeRegion");

	// Initial values for box sizes.
	boxData->setMaximumSize(len800p, wlAuto);

	// Initialize often-used decoration styles.
	wcssDefault = Wt::WCssDecorationStyle();
	Wt::WBorder wBorder = Wt::WBorder(Wt::BorderStyle::Dotted);
	wcssAttention.setBorder(wBorder);
	wcssHighlighted.setBackgroundColor(wcSelectedWeak);

	// Initial values for the tab widget.
	int numTab = tabData->count();
	for (int ii = 0; ii < numTab; ii++)
	{
		tabData->setTabEnabled(ii, 0);
	}
	tabData->setCurrentIndex(0);

	// Initial values for the region tree widget.
	auto fnTree = std::bind(static_cast<void(SCDAwidget::*)(void)>(&SCDAwidget::treeClicked), this);
	treeRegion->itemSelectionChanged().connect(fnTree);

	// Initial values for the table tab.
	boxTable->setMaximumSize(790.0, screenHeight - 90);
	boxTable->setOverflow(Wt::Overflow::Hidden, Wt::Orientation::Horizontal | Wt::Orientation::Vertical);

	// Initial values for the map tab.
	textUnit->setTextAlignment(Wt::AlignmentFlag::Middle);
	boxMap->setContentAlignment(Wt::AlignmentFlag::Center);

	// Initial values for the download tab.
	//pbDownloadPDF->clicked().connect(this, std::bind(&SCDAwidget::downloadMap, this));

	// Initial values for the miscellaneous buttons.


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

unique_ptr<Wt::WContainerWidget> SCDAwidget::makeBoxData()
{
	auto boxDataUnique = make_unique<Wt::WContainerWidget>();
	boxData = boxDataUnique.get();
	auto layout = make_unique<Wt::WVBoxLayout>();

	auto tabDataUnique = make_unique<Wt::WTabWidget>();
	tabData = tabDataUnique.get();

	auto treeRegionUnique = make_unique<Wt::WTree>();
	treeRegion = treeRegionUnique.get();
	tabData->addTab(move(treeRegionUnique), "Geographic Region", Wt::ContentLoading::Eager);

	auto boxTableUnique = make_unique<Wt::WContainerWidget>();
	boxTable = boxTableUnique.get();
	tabData->addTab(move(boxTableUnique), "Data Table", Wt::ContentLoading::Eager);

	auto boxMapUnique = makeBoxMap();
	tabData->addTab(move(boxMapUnique), "Data Map", Wt::ContentLoading::Eager);

	auto boxDownloadUnique = makeBoxDownload();
	tabData->addTab(move(boxDownloadUnique), "Download", Wt::ContentLoading::Eager);

	stackedTabData = tabData->contentsStack();
	layout->addWidget(move(tabDataUnique));
	boxDataUnique->setLayout(move(layout));
	return boxDataUnique;
}
unique_ptr<Wt::WContainerWidget> SCDAwidget::makeBoxDownload()
{
	auto boxDownloadUnique = make_unique<Wt::WContainerWidget>();
	boxDownload = boxDownloadUnique.get();
	auto pbDownloadPDFUnique = make_unique<Wt::WPushButton>("PDF");
	pbDownloadPDF = pbDownloadPDFUnique.get();
	boxDownloadUnique->addWidget(move(pbDownloadPDFUnique));
	return boxDownloadUnique;
}
unique_ptr<Wt::WContainerWidget> SCDAwidget::makeBoxMap()
{
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
	vLayoutMap->addStretch(1);
	boxMapAllUnique->setLayout(move(vLayoutMap));

	return boxMapAllUnique;
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
	bool grey = 0;
	auto layout = make_unique<Wt::WVBoxLayout>();
	vector<Wt::WString> vwsLegend = wjConfig->getTextLegend();
	for (int ii = 0; ii < vwsLegend.size(); ii++)
	{
		auto wText = make_unique<Wt::WText>(vwsLegend[ii]);
		wText->setTextAlignment(Wt::AlignmentFlag::Left);
		wText->decorationStyle().font().setSize(Wt::FontSize::Large);
		if (grey)
		{
			wText->decorationStyle().setBackgroundColor(wcGrey);
			grey = 0;
		}
		else { grey = 1; }
		layout->addWidget(move(wText));
	}
	boxTextLegend->setLayout(move(layout));
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
		wjConfig->addDifferentiator(event.get_list());
		break;
	case 5:  // Map: display it on the painter widget.
		processEventMap(event.get_list(), event.get_areas(), event.getRegionData());
		break;
	case 6:  // Parameter: create new panels with options for the user to specify.
		updateTextCata(event.getNumCata());
		processEventParameter(event.getVariable(), event.getCata(), event.get_list());
		break;
	case 7:  // Table: populate the widget with data.
		tableCore = event.getTable();
		tableCol = event.getListCol();
		tableRow = event.getListRow();
		tableRegion = event.getSRegion();
		processEventTable(tableCore, tableCol, tableRow, tableRegion);
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
	activeCata = sCata;
	Wt::WString wsTemp;
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(6);
	prompt[0] = app->sessionId();
	prompt[1] = sYear;
	wsTemp = wjConfig->wjpCategory->cbTitle->currentText();
	prompt[2] = wsTemp.toUTF8();
	wsTemp = wjConfig->wjpTopicCol->cbTitle->currentText();
	prompt[3] = wsTemp.toUTF8();
	wsTemp = wjConfig->wjpTopicRow->cbTitle->currentText();
	prompt[4] = wsTemp.toUTF8();
	prompt[5] = activeCata;
	vector<vector<string>> vvsVariable = wjConfig->getVariable();
	sRef.pullVariable(prompt, vvsVariable);
}
void SCDAwidget::processEventCategory(vector<string> vsCategory)
{
	wjConfig->wjpCategory->setHidden(0);
	vsCategory.insert(vsCategory.begin(), "[Choose a topical category]");
	wjConfig->wjpCategory->setCB(0, vsCategory);
	wjConfig->wjpCategory->highlight(0);
}
void SCDAwidget::processEventDemographic(vector<vector<string>> vvsDemo)
{
	wjConfig->vvsDemographic = vvsDemo;
	wjConfig->resetVariables(1);
	if (vvsDemo.size() > 1) { wjConfig->addDemographic(vvsDemo); }
	else
	{
		Wt::WApplication* app = Wt::WApplication::instance();
		vector<string> prompt = { app->sessionId() };
		vector<string> vsCata;
		//vsCata.assign(vvsDemo[0].begin() + 1, vvsDemo[0].end());
		sRef.pullDifferentiator(prompt, vvsDemo);
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
}
void SCDAwidget::processEventParameter(vector<vector<string>> vvsVariable, vector<vector<string>> vvsCata, vector<string> vsDIMIndex)
{
	// vvsVariable has form [variable index][MID0, MID1, ..., variable title].
	int numCata = 0;
	for (int ii = 0; ii < vvsCata.size(); ii++)
	{
		numCata += vvsCata[ii].size() - 1;
	}

	int sizeVar = wjConfig->varWJP.size();
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

		Wt::WString wsTemp;
		WJPANEL* wjp = nullptr;
		if (!sizeVar) { wjConfig->vvsParameter = vvsVariable; }
		else
		{
			for (int ii = 0; ii < sizeVar; ii++)
			{
				wjp = wjConfig->varWJP[ii];
				wsTemp = wjp->cbMID->currentText();
				if (wsTemp != "") { break; }
				else if (ii == sizeVar - 1)
				{
					if (wjConfig->wjpDemo != nullptr) { wjConfig->resetVariables(1); }
					else { wjConfig->resetVariables(0); }
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
			wjConfig->addVariable(vsTemp);
		}
		sizeVar = wjConfig->varWJP.size();
		for (int ii = 0; ii < sizeVar; ii++)
		{
			wjConfig->varWJP[ii]->cbTitle->setEnabled(0);
		}

		wjConfig->removeDifferentiators();
	}
	else
	{
		// Multiple catalogues satisfy the Year, Category, Row, and Column criteria.
		// Check vvsVariable[0] for the type of differentiation to employ.
		if (vvsVariable[0].size() != 1) { jf.err("Invalid vvsCandidate-SCDAwidget.pDE(Variable)"); }
		//wjConfig->addVariable(vvsVariable);
		int bbq = 1;
	}

	// If there are no more unspecified variables, populate the region tree tab.
	if (sizeVar == numVar)
	{
		Wt::WApplication* app = Wt::WApplication::instance();
		app->processEvents();
		activeCata = vvsCata[0][1];
		vector<string> promptTree(3);
		promptTree[0] = app->sessionId();
		promptTree[1] = vvsCata[0][0];
		promptTree[2] = vvsCata[0][1];
		sRef.pullTree(promptTree);
	}
}
void SCDAwidget::processEventTable(vector<vector<string>>& vvsTable, vector<string>& vsCol, vector<string>& vsRow, string& sRegion)
{
	if (vsCol.size() < 3 || vsRow.size() < 3) { jf.err("Missing col/row titles-SCDAwidget.processEventTable"); }
	if (vvsTable.size() < 1) { jf.err("Missing table data-SCDAwidget.processEventTable"); }

	auto app = Wt::WApplication::instance();
	//activeColTopic = vsCol.back();
	vsCol.pop_back();
	//activeRowTopic = vsRow.back();
	vsRow.pop_back();

	// Prepare the table tab.
	tabData->setTabEnabled(1, 1);
	string temp = "Data Table (" + sRegion + ")";
	Wt::WString wTemp = Wt::WString::fromUTF8(temp);
	auto tab = tabData->itemAt(1);
	tab->setText(wTemp);

	// Make a new table widget.
	boxTable->clear();
	auto tableDataUnique = make_unique<WJTABLE>(vvsTable, vsCol, vsRow, sRegion);
	tableData = tableDataUnique.get();
	boxTable->addWidget(move(tableDataUnique));
	tableData->setMaximumSize(780.0, screenHeight - 100);
	tableData->headerSignal().connect(this, bind(&SCDAwidget::incomingHeaderSignal, this, placeholders::_1, placeholders::_2));

	// Populate cbColTopicSel and cbRowTopicSel if necessary.
	if (wjConfig->wjpTopicRow->cbMID->isHidden() || wjConfig->wjpTopicCol->cbMID->isHidden())
	{
		wjConfig->wjpTopicRow->cbMID->setHidden(0);
		wjConfig->wjpTopicRow->setCB(1, vsRow);

		wjConfig->wjpTopicCol->cbMID->setHidden(0);
		wjConfig->wjpTopicCol->setCB(1, vsCol);

		wjConfig->wjpTopicRow->pbDialog->setHidden(0);
		wjConfig->wjpTopicCol->pbDialog->setHidden(0);
		wjConfig->initJTree(vsRow, vsCol);
	}

	// Add a tooltip to the table.
	wTemp = mapTooltip.at("table");
	tableData->setToolTip(wTemp);

	// Experimental...
	if (jsEnabled)
	{
		app->doJavaScript(app->javaScriptClass() + ".getInfo();");
		double dBoxTableWidth = boxData->maximumWidth().value();
		dBoxTableWidth += boxTable->padding(Wt::Side::Left).value() + boxTable->padding(Wt::Side::Right).value();
		boxTableWidth = int(ceil(dBoxTableWidth));
		app->doJavaScript(app->javaScriptClass() + ".scrollTo(0);");
		app->triggerUpdate();
	}

	// Select the correct table cell.
	app->processEvents();
	wjConfig->varMIDClicked();
	wjConfig->topicSelChanged();  // Triggers table current selection. 
}
void SCDAwidget::processEventTopic(vector<string> vsRowTopic, vector<string> vsColTopic)
{
	Wt::WApplication* app = Wt::WApplication::instance();
	Wt::WString wsTemp;
	int numRowTopic = vsRowTopic.size();
	int numColTopic = vsColTopic.size();
	wjConfig->wjpTopicRow->setHidden(0);
	wjConfig->wjpTopicCol->setHidden(0);

	string rowDefault = "[Choose a row topic]";
	vsRowTopic.insert(vsRowTopic.begin(), rowDefault);
	wjConfig->wjpTopicRow->setCB(0, vsRowTopic);
	if (numRowTopic == 1) { wjConfig->wjpTopicRow->cbTitle->setCurrentIndex(1); }

	string colDefault = "[Choose a column topic]";
	vsColTopic.insert(vsColTopic.begin(), colDefault);
	wjConfig->wjpTopicCol->setCB(0, vsColTopic);
	if (numColTopic == 1) { wjConfig->wjpTopicCol->cbTitle->setCurrentIndex(1); }

	wjConfig->wjpCategory->unhighlight(0);
	if (numRowTopic == 1 && numColTopic == 1)
	{
		wjConfig->wjpTopicRow->unhighlight(0);
		wjConfig->wjpTopicCol->unhighlight(0);
		vector<vector<string>> vvsVariable;
		vector<string> prompt(5);
		prompt[0] = app->sessionId();
		wsTemp = wjConfig->wjpYear->cbTitle->currentText();
		prompt[1] = wsTemp.toUTF8();
		wsTemp = wjConfig->wjpCategory->cbTitle->currentText();
		prompt[2] = wsTemp.toUTF8();
		wsTemp = wjConfig->wjpTopicCol->cbTitle->currentText();
		prompt[3] = wsTemp.toUTF8();
		wsTemp = wjConfig->wjpTopicRow->cbTitle->currentText();
		prompt[4] = wsTemp.toUTF8();
		sRef.pullVariable(prompt, vvsVariable);
	}
	else
	{
		if (wjConfig->wjpDemo == nullptr) { wjConfig->resetVariables(0); }
		else { wjConfig->resetVariables(1); }

		if (vsColTopic.size() == 1)
		{
			wjConfig->wjpTopicRow->highlight(0);
			wjConfig->wjpTopicCol->unhighlight(0);
		}
		else if (vsRowTopic.size() == 1)
		{
			wjConfig->wjpTopicRow->unhighlight(0);
			wjConfig->wjpTopicCol->highlight(0);
		}
		else
		{
			wjConfig->wjpTopicRow->highlight(0);
			wjConfig->wjpTopicCol->highlight(0);
		}
	}
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
	tableData->hide();
	Wt::WString wTemp("Data Table");
	auto tab = tabData->itemAt(1);
	tab->setText(wTemp);
	tabData->setTabEnabled(1, 0);
	tabData->setCurrentIndex(0);
}
void SCDAwidget::resetTree()
{
	jtRegion.clear();
	treeRegion->setTreeRoot(make_unique<Wt::WTreeNode>(""));
}

void SCDAwidget::setMap(int iRow, int iCol, string sRegion)
{
	// Assumes that all widgets have been updated already.
	// iCol corresponds to CB indices, not table indices!
	Wt::WString wsTemp;
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(4);
	prompt[0] = app->sessionId();
	wsTemp = wjConfig->wjpYear->cbTitle->currentText();
	prompt[1] = wsTemp.toUTF8();
	prompt[2] = activeCata;
	prompt[3] = sRegion;  // Parent region name. 

	vector<vector<string>> variable = wjConfig->getVariable();
	wsTemp = wjConfig->wjpTopicRow->cbTitle->currentText();
	string rowTitle = wsTemp.toUTF8();
	wsTemp = wjConfig->wjpTopicRow->cbMID->currentText();
	string rowMID = wsTemp.toUTF8();
	wsTemp = wjConfig->wjpTopicCol->cbTitle->currentText();
	string colTitle = wsTemp.toUTF8();
	variable.push_back({ rowTitle, rowMID });
	variable.push_back({ colTitle, to_string(iCol + 1) });
	sRef.pullMap(prompt, variable);
}
void SCDAwidget::setTable(int geoCode, string sRegion)
{
	// This variant loads a table using a selected region from the tree. 
	Wt::WApplication* app = Wt::WApplication::instance();
	Wt::WString wsTemp;
	vector<string> prompt(5);
	prompt[0] = app->sessionId();
	wsTemp = wjConfig->wjpYear->cbTitle->currentText();
	prompt[1] = wsTemp.toUTF8();
	prompt[2] = activeCata;
	prompt[3] = to_string(geoCode);
	prompt[4] = sRegion;

	vector<vector<string>> variable = wjConfig->getVariable();
	sRef.pullTable(prompt, variable);
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
		//widgetMobile();
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
		//widgetMobile();
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
	// This variant automatically launches a new table pull.
	auto selSet = treeRegion->selectedNodes();
	if (selSet.size() < 1) { return; }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	string sRegion = wTemp.toUTF8();
	int geoCode = jtRegion.getIName(sRegion);
	setTable(geoCode, sRegion);
}
void SCDAwidget::treeClicked(int& geoCode, string& sRegion)
{
	// This variant simply returns the geoCode and name of the selected region. 
	auto selSet = treeRegion->selectedNodes();
	if (selSet.size() < 1) { return; }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	sRegion = wTemp.toUTF8();
	geoCode = jtRegion.getIName(sRegion);
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
	wjConfig->textCata->setText(wsTemp);
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

