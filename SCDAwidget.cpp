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
		wjConfig->pullSignal().connect(this, bind(&SCDAwidget::incomingPullSignal, this, placeholders::_1));
		wjConfig->resetSignal().connect(this, bind(&SCDAwidget::incomingResetSignal, this, placeholders::_1));
		wjConfig->pbMobile->clicked().connect(this, &SCDAwidget::toggleMobile);
		wjConfig->headerSignal().connect(this, bind(&SCDAwidget::incomingHeaderSignal, this, placeholders::_1, placeholders::_2));
		wjMap->pbPin->clicked().connect(this, bind(&SCDAwidget::seriesAddToGraph, this));
		wjMap->pbPinReset->clicked().connect(this, bind(&SCDAwidget::resetBarGraph, this));
		wjDownload->previewSignal().connect(this, bind(&SCDAwidget::incomingPreviewSignal, this, placeholders::_1));
		if (filtersEnabled)
		{
			//wjConfig->wjpTopicRow->filterSignal().connect(this, bind(&SCDAwidget::incomingFilterSignal, this));
			//wjConfig->wjpTopicCol->filterSignal().connect(this, bind(&SCDAwidget::incomingFilterSignal, this));
			//wjTableBox->pbFilterRow->clicked().connect(wjConfig->wjpTopicRow, &WJPANEL::dialogFilter);
			//wjTableBox->pbFilterCol->clicked().connect(wjConfig->wjpTopicCol, &WJPANEL::dialogFilter);
		}
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

void SCDAwidget::incomingHeaderSignal(const int& iRow, const int& iCol)
{
	// Either the config CBs or the data table is reporting a change in selected headers. 
	long long timeTable = tableData->jf.timerReport();
	long long timeConfigCol = wjConfig->wjpTopicCol->jf.timerReport();
	long long timeConfigRow = wjConfig->wjpTopicRow->jf.timerReport();
	long long timeConfig = min(timeConfigCol, timeConfigRow);

	// Update the CB widgets or the data table's selected cell, then load a map. 
	string sRegion = tableData->getCell(-1, 0);
	int iRowSel, iColSel;
	if (timeConfig < timeTable)  // CB values override table values. 
	{
		if (iRow < 0) { iRowSel = wjConfig->wjpTopicRow->getIndexMID(1); }
		else { iRowSel = iRow; }
		if (iCol < 0) { iColSel = wjConfig->wjpTopicCol->getIndexMID(1); }
		else { iColSel = iCol; }
		tableData->cellSelect(iRowSel, iColSel + 1);
		setMap(iRowSel, iColSel, sRegion);
	}
	else  // Table values override CB values. 
	{ 
		wjConfig->wjpTopicRow->setIndexMID(iRow);
		if (iCol > 0)  // A row header was not clicked. 
		{ 
			wjConfig->wjpTopicCol->setIndexMID(iCol - 1); 
			setMap(iRow, iCol - 1, sRegion);
		}
		else  // A row header was clicked. 
		{
			iColSel = wjConfig->wjpTopicCol->getIndexMID(1);
			setMap(iRow, iColSel, sRegion);
		}
	}
}
void SCDAwidget::incomingPreviewSignal(const int& type)
{
	switch (type)
	{
	case 2:
	{
		vector<int> vChanged;
		vector<string> vsParameter = wjConfig->getTextLegend(vChanged);
		wjDownload->displayPDFmap(vsParameter, vChanged);
		break;
	}
	case 3:
	{
		string tableCSV = wjTableBox->makeCSV();
		wjDownload->displayCSV(tableCSV);
		break;
	}
	}
}
void SCDAwidget::incomingPullSignal(const int& pullType)
{
	auto app = Wt::WApplication::instance();
	string sessionID = app->sessionId();
	string sPrompt;
	Wt::WString wsTemp;
	vector<int> viMID;
	vector<string> vsPrompt, vsDIMtitle;
	vector<vector<string>> vvsCata, vvsPrompt;
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
		wjConfig->getPrompt(sPrompt, vvsCata, vvsPrompt);
		sPrompt = sessionID;
		sRef.pullDifferentiator(sPrompt, vvsCata, vvsPrompt);
		break;
	case 2:  // Pull table.
	{
		wjConfig->getPrompt(vsPrompt, vsDIMtitle, viMID);
		vsPrompt[0] = sessionID;
		vsPrompt[2] = activeCata;
		int geoCode;
		treeClicked(geoCode, vsPrompt[4]);
		vsPrompt[3] = to_string(geoCode);
		sRef.pullTable(vsPrompt, vsDIMtitle, viMID);
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
		resetBarGraph();
		break;
	case 1:
		resetDownload();
		break;
	case 2:
		resetMap();
		break;
	case 3:
		resetTabAll();
		break;
	case 4:
		resetTable();
		break;
	case 5:
		resetTree();
		break;
	}

}
void SCDAwidget::incomingVarSignal()
{
	// A Variable WJPANEL has changed its values. Load a new table and map.
	if (activeCata.size() < 1) { jf.err("No activeCata-SCDAwidget.incomingVarSignal"); }
	auto app = Wt::WApplication::instance();
	vector<string> vsDIMtitle;
	vector<int> viMID;
	wjConfig->getVariable(vsDIMtitle, viMID);
	vector<string> vsPrompt(5);
	vsPrompt[0] = app->sessionId();
	Wt::WString wsTemp = wjConfig->wjpYear->cbTitle->currentText();
	vsPrompt[1] = wsTemp.toUTF8();
	vsPrompt[2] = activeCata;
	int geoCode;
	treeClicked(geoCode, vsPrompt[4]);
	vsPrompt[3] = to_string(geoCode);
	sRef.pullTable(vsPrompt, vsDIMtitle, viMID);
}

void SCDAwidget::init()
{
	this->clear();
	screenWidth = getWidth();
	screenHeight = getHeight();
	wlDataFrameWidth = Wt::WLength(screenWidth - 485);
	wlDataFrameHeight = Wt::WLength(screenHeight - 130);
	wlTableWidth = Wt::WLength(screenWidth - 515);
	if (filtersEnabled) { wlTableHeight = Wt::WLength(screenHeight - 180); }
	else { wlTableHeight = Wt::WLength(screenHeight - 140); }
	vector<string> vsYear = { "2016" };

	auto hLayout = make_unique<Wt::WHBoxLayout>();
	auto wjConfigUnique = make_unique<WJCONFIG>(vsYear);
	wjConfig = wjConfigUnique.get();
	hLayout->addWidget(move(wjConfigUnique));
	auto boxDataUnique = makeBoxData();
	boxData = hLayout->addWidget(move(boxDataUnique));
	this->setLayout(move(hLayout));

	connect();
	initUI();
	initMaps();
	if (screenHeight > screenWidth) { toggleMobile(); }
	else { widgetMobile(); }

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
	wsTooltip = "Displayed region does not match the pinned region.";
	mapTooltip.emplace("pinRegion", wsTooltip);
	wsTooltip = "Displayed data unit does not match the pinned data unit.";
	mapTooltip.emplace("pinUnit", wsTooltip);
	wsTooltip = "Displayed region and data unit do not match the\npinned region and data unit.";
	mapTooltip.emplace("pinRegionUnit", wsTooltip);

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
	wcWhite.setRgb(255, 255, 255);

	// Widget names.
	boxData->setObjectName("boxData");
	tabData->setObjectName("tabData");
	treeRegion->setObjectName("treeRegion");

	// Initial values for box sizes.
	boxData->setMaximumSize(screenWidth - 470, wlAuto);

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

shared_ptr<Wt::WMemoryResource> SCDAwidget::loadCSS(vector<unsigned char>& binCSS)
{
	auto css = make_shared<Wt::WMemoryResource>("binCSS");
	int cssSize = binCSS.size();
	css->setData(&binCSS[0], cssSize);
	return css;
}
shared_ptr<Wt::WMemoryResource> SCDAwidget::loadIcon(vector<unsigned char>& binIcon)
{
	auto icon = make_shared<Wt::WMemoryResource>("binIcon");
	int binSize = binIcon.size();
	icon->setData(&binIcon[0], binSize);
	return icon;
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

	auto wjBoxTableUnique = make_unique<WJTABLEBOX>();
	wjTableBox = wjBoxTableUnique.get();
	tabData->addTab(move(wjBoxTableUnique), "Data Table", Wt::ContentLoading::Eager);

	auto wjMapUnique = make_unique<WJMAP>();
	wjMap = wjMapUnique.get();
	tabData->addTab(move(wjMapUnique), "Data Map", Wt::ContentLoading::Eager);

	auto wjBarGraphUnique = make_unique<WJBARGRAPH>(wlDataFrameHeight);
	wjBarGraph = wjBarGraphUnique.get();
	tabData->addTab(move(wjBarGraphUnique), "Bar Graph", Wt::ContentLoading::Eager);

	auto wjDownloadUnique = make_unique<WJDOWNLOAD>(wlDataFrameWidth, wlDataFrameHeight);
	wjDownload = wjDownloadUnique.get();
	tabData->addTab(move(wjDownloadUnique), "Download", Wt::ContentLoading::Eager);

	stackedTabData = tabData->contentsStack();
	layout->addWidget(move(tabDataUnique));
	boxDataUnique->setLayout(move(layout));
	return boxDataUnique;
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
	string sID = jtRegion.mapRegion.at(sRegionClicked);
	Wt::WTreeNode* nodeSel = (Wt::WTreeNode*)treeRegion->findById(sID);
	if (!treeRegion->isSelected(nodeSel)) { treeRegion->select(nodeSel); }
}
void SCDAwidget::populateTextLegend(WJLEGEND*& wjLegend)
{
	Wt::WString wsTemp;
	vector<int> viChanged;
	wjLegend->vsParameter = wjConfig->getTextLegend(viChanged);
	wjLegend->setColour(viChanged);
	wjLegend->display(1);
}
void SCDAwidget::populateTree(JTREE& jt, Wt::WTreeNode*& node)
{
	// Recursive function that takes an existing node and makes its children.
	// NOTE: This variant is meant for regions with unique names !
	vector<string> vsChildren;
	vector<int> viChildren;
	Wt::WString wTemp = node->label()->text();
	string sNode = wTemp.toUTF8();
	if (jt.isExpanded(sNode)) { node->expand(); }
	string sID = node->id();
	jt.mapRegion.emplace(sNode, sID);
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
		processEventDifferentiation(event.get_list(), event.getTitle());
		break;
	case 5:  // Map: display it on the painter widget.
		processEventMap(event.get_list(), event.get_areas(), event.getRegionData());
		break;
	case 6:  // Parameter: create new panels with options for the user to specify.
		updateTextCata(event.getNumCata());
		processEventParameter(event.getParameter(), event.getCata());
		break;
	case 7:  // Table: populate the widget with data.
		tableCore = event.getTable();
		tableCol = event.getCol();
		tableRow = event.getRow();
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
	// Initialize custom icons.
	wjConfig->linkIconChevronDown = Wt::WLink(iconChevronDown);
	wjConfig->linkIconChevronRight = Wt::WLink(iconChevronRight);
	wjConfig->wjpTopicCol->initStackedPB(wjConfig->linkIconChevronDown, wjConfig->linkIconChevronRight);
	wjConfig->wjpTopicRow->initStackedPB(wjConfig->linkIconChevronDown, wjConfig->linkIconChevronRight);

	wjConfig->wjpCategory->setHidden(0);
	vsCategory.insert(vsCategory.begin(), "[Choose a topical category]");
	wjConfig->wjpCategory->setCB(vsCategory);
	wjConfig->wjpCategory->highlight(0);
	wjConfig->setMobile(mobile);
}
void SCDAwidget::processEventDemographic(vector<vector<string>> vvsDemo)
{
	wjConfig->vvsDemographic = vvsDemo;
	wjConfig->resetVariables(1);
	if (vvsDemo.size() > 0) { wjConfig->addDemographic(vvsDemo); }
	else { jf.err("Invalid Demographic list-SCDAwidget.processEventDemographic"); }
}
void SCDAwidget::processEventDifferentiation(vector<string> vsDiff, string sTitle)
{
	// If the list contains DIM titles, then its last element must be "sTitle". Otherwise, 
	// the list contains MID elements for an existing Diff title.
	if (sTitle == "sTitle") { wjConfig->addDifferentiator(vsDiff); }
	else { wjConfig->addDifferentiator(vsDiff, sTitle); }
}
void SCDAwidget::processEventMap(vector<string> vsRegion, vector<vector<vector<double>>> vvvdArea, vector<vector<double>> vvdData)
{
	wjMap->boxMap->clear();
	wjMap->resetMenu();
	auto wtMapUnique = make_unique<WTPAINT>();
	wtMap = wjMap->boxMap->addWidget(move(wtMapUnique));
	populateTextLegend(wjMap->wjlMap);

	int legendBarDouble, legendTickLines;
	Wt::WString wsUnit;
	Wt::WString wsUnitOld = wjMap->textUnit->text();
	string sUnitOld = wsUnitOld.toUTF8();
	size_t pos1 = sUnitOld.find(':');
	if (pos1 < sUnitOld.size())
	{
		sUnitOld = sUnitOld.substr(pos1 + 2);
	}
	string sUnit = getUnit();
	function<void()> fnUnit1 = bind(&SCDAwidget::updateUnit, this, sUnit);
	wjMap->popupUnit->addItem(sUnit)->triggered().connect(fnUnit1);
	if (sUnit == "# of persons")
	{
		string temp = "% of population";
		function<void()> fnUnit2 = bind(&SCDAwidget::updateUnit, this, temp);
		wjMap->popupUnit->addItem(temp)->triggered().connect(fnUnit2);

		if (sUnitOld == sUnit)  // Keep this unit if it had been chosen previously.
		{
			wtMap->setUnit(sUnit, { 0 });
			wsUnit = "Unit: " + sUnit;
			updatePinButtons(sUnit, vsRegion[0]);  // Enable or disable as appropriate.
			legendBarDouble = wjMap->getLegendBarDouble(vsRegion, sUnit, 1);
		}
		else  // Default to this unit.
		{
			wtMap->setUnit(sUnit, { 0, 1 });
			wsUnit = "Unit: " + temp;
			updatePinButtons(temp, vsRegion[0]);  // Enable or disable as appropriate.
			legendBarDouble = wjMap->getLegendBarDouble(vsRegion, sUnit, 2);
		}
	}
	else 
	{ 
		wtMap->setUnit(sUnit, { 0 }); 
		wsUnit = "Unit: " + sUnit;
		updatePinButtons(sUnit, vsRegion[0]);  // Enable or disable as appropriate.
		legendBarDouble = wjMap->getLegendBarDouble(vsRegion, sUnit, 1);
	}
	legendTickLines = wjMap->getLegendTickLines(sUnit);
	wjMap->textUnit->setText(wsUnit);

	wtMap->legendBarDouble = legendBarDouble;
	wtMap->legendTickLines = legendTickLines;
	vector<Wt::WPolygonArea*> area = wtMap->drawMap(vvvdArea, vsRegion, vvdData);
	for (int ii = 0; ii < area.size(); ii++)
	{
		function<void()> fnArea = bind(&SCDAwidget::mapAreaClicked, this, ii);
		area[ii]->clicked().connect(fnArea);
	}
	wjDownload->setMap(vvvdArea, vvdData);  // Store the raw data, for potential PDF rendering.
	wjDownload->legendBarDouble = legendBarDouble;
	wjDownload->legendTickLines = legendTickLines;

	tabData->setTabEnabled(2, 1);
	updateDownloadTab();
	if (first) { tabData->setCurrentIndex(2); }	
	else if (tabData->currentIndex() == 3) { tabData->setCurrentIndex(2); }
	first = 0;
}
void SCDAwidget::processEventParameter(vector<vector<vector<string>>> vvvsParameter, vector<vector<string>> vvsCata)
{
	// vvvsParameter has form [variable index][MID1, MID2, ..., DIM title][MID, sVal, Ancestor0, ...].
	int numCata = 0;
	for (int ii = 0; ii < vvsCata.size(); ii++)
	{
		numCata += vvsCata[ii].size() - 1;
	}
	if (numCata != 1) { jf.err("Multiple catalogues remain-SCDAwidget.processEventParameter"); }

	int sizeVar = wjConfig->varWJP.size();
	activeCata = vvsCata[0][1];
	jf.removeBlanks(vvvsParameter);

	/*
	if (mapNumVar.count(vvsCata[0][1]))
	{
		numVar = mapNumVar.at(vvsCata[0][1]);
	}
	else
	{
		numVar = vsDIMIndex.size() - 2;
		mapNumVar.emplace(vvsCata[0][1], numVar);
	}
	*/

	Wt::WString wsTemp;
	WJPANEL* wjp = nullptr;
	if (!sizeVar) { wjConfig->vvvsParameter = vvvsParameter; }
	else
	{
		for (int ii = 0; ii < sizeVar; ii++)
		{
			wjp = wjConfig->varWJP[ii];
			wsTemp = wjp->selMID;
			if (wsTemp != "") { break; }
			else if (ii == sizeVar - 1)
			{
				if (wjConfig->wjpDemo != nullptr) { wjConfig->resetVariables(1); }
				else { wjConfig->resetVariables(0); }
				activeCata = vvsCata[0][1];
				wjConfig->vvvsParameter = vvvsParameter;
			}
		}
	}

	string sTitle;
	for (int ii = 0; ii < vvvsParameter.size(); ii++)
	{
		sTitle = vvvsParameter[ii][vvvsParameter[ii].size() - 1][0];  // DIM title.
		vvvsParameter[ii].pop_back();
		wjConfig->addVariable(sTitle, vvvsParameter[ii]);
	}

	wjConfig->removeDifferentiators();
	wjConfig->wjpTopicCol->unhighlight(0);
	wjConfig->wjpTopicRow->unhighlight(0);
	widgetMobile();

	// Given there are no more unspecified variables, populate the region tree tab.
	Wt::WApplication* app = Wt::WApplication::instance();
	app->processEvents();
	activeCata = vvsCata[0][1];
	vector<string> promptTree(3);
	promptTree[0] = app->sessionId();
	promptTree[1] = vvsCata[0][0];
	promptTree[2] = vvsCata[0][1];
	sRef.pullTree(promptTree);
}
void SCDAwidget::processEventTable(vector<vector<string>>& vvsTable, vector<vector<string>>& vvsCol, vector<vector<string>>& vvsRow, string& sRegion)
{
	if (vvsCol.size() < 3 || vvsRow.size() < 3) { jf.err("Missing col/row titles-SCDAwidget.processEventTable"); }
	if (vvsTable.size() < 1) { jf.err("Missing table data-SCDAwidget.processEventTable"); }

	auto app = Wt::WApplication::instance();
	string sTitleCol = vvsCol[vvsCol.size() - 1][0];  // Currently unused.
	string sTitleRow = vvsRow[vvsRow.size() - 1][0];  // Currently unused.
	vvsCol.pop_back();
	vvsRow.pop_back();
	jf.removeBlanks(vvsCol);
	jf.removeBlanks(vvsRow);

	// Populate ColTopicSel and RowTopicSel if necessary.
	if (wjConfig->wjpTopicRow->boxMID->isHidden() || wjConfig->wjpTopicCol->boxMID->isHidden())
	{
		wjConfig->wjpTopicRow->boxMID->setHidden(0);
		wjConfig->wjpTopicRow->setTree(vvsRow);

		wjConfig->wjpTopicCol->boxMID->setHidden(0);
		wjConfig->wjpTopicCol->setTree(vvsCol);
	}

	// Prepare the table tab.
	tabData->setTabEnabled(1, 1);
	string temp = "Data Table (" + sRegion + ")";
	Wt::WString wTemp = Wt::WString::fromUTF8(temp);
	auto tab = tabData->itemAt(1);
	tab->setText(wTemp);

	// Make a new table widget.
	tableData = wjTableBox->setTable(vvsTable, vvsCol, vvsRow, sRegion);
	tableData->setMaximumSize(wlTableWidth, wlTableHeight);
	tableData->headerSignal().connect(this, bind(&SCDAwidget::incomingHeaderSignal, this, placeholders::_1, placeholders::_2));

	// Add a helper tip, if necessary.
	if (!setTip.count("tableWidth"))
	{
		if (wjTableBox->linkIconClose.isNull()) { wjTableBox->linkIconClose = Wt::WLink(iconClose); }
		wjTableBox->addTipWidth();
		wjTableBox->tipSignal().connect(this, std::bind(&SCDAwidget::setTipAdd, this, std::placeholders::_1));
	}

	// Add a tooltip to the table.
	wTemp = mapTooltip.at("table");
	tableData->setToolTip(wTemp);

	// Experimental...
	if (jsEnabled)
	{
		app->doJavaScript(app->javaScriptClass() + ".getInfo();");
		double dBoxTableWidth = boxData->maximumWidth().value();
		dBoxTableWidth += wjTableBox->padding(Wt::Side::Left).value() + wjTableBox->padding(Wt::Side::Right).value();
		boxTableWidth = int(ceil(dBoxTableWidth));
		app->doJavaScript(app->javaScriptClass() + ".scrollTo(0);");
		app->triggerUpdate();
	}

	// Select the correct table cell.
	app->processEvents();
	wjConfig->wjpTopicRow->panelClicked();
	wjConfig->wjpTopicCol->panelClicked();
	wjConfig->topicSelChanged();  // Triggers table current selection. 

	// Update the download tab.
	updateDownloadTab();
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
	wjConfig->wjpTopicRow->setCB(vsRowTopic);
	if (numRowTopic == 1) { wjConfig->wjpTopicRow->cbTitle->setCurrentIndex(1); }

	string colDefault = "[Choose a column topic]";
	vsColTopic.insert(vsColTopic.begin(), colDefault);
	wjConfig->wjpTopicCol->setCB(vsColTopic);
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

void SCDAwidget::resetBarGraph()
{
	if (!tabData->isTabEnabled(3)) { return; }
	string sRegion;
	if (tableData == nullptr) { sRegion = ""; }
	else { sRegion = tableData->getCell(-1, 0); }
	wjBarGraph->reset();
	Wt::WString wsTemp = wjMap->textUnit->text();
	string mapUnit = wsTemp.toUTF8();
	size_t pos1 = mapUnit.find(':');
	if (pos1 < mapUnit.size()) { updatePinButtons(mapUnit.substr(pos1 + 2), sRegion); }
	else { updatePinButtons(mapUnit, sRegion); }
	tabData->setTabEnabled(3, 0);
}
void SCDAwidget::resetDownload()
{
	//if (wjDownload != nullptr) { wjDownload->clear(); }
	tabData->setTabEnabled(4, 0);
}
void SCDAwidget::resetMap()
{
	if (!tabData->isTabEnabled(2)) { return; }
	first = 1;
	wjMap->boxMap->clear();
	tabData->setTabEnabled(2, 0);
	tabData->setCurrentIndex(0);
}
void SCDAwidget::resetTabAll()
{
	resetDownload();
	resetBarGraph();
	resetMap();
	resetTable();
	resetTree();
}
void SCDAwidget::resetTable()
{
	if (!tabData->isTabEnabled(1)) { return; }
	tableData->hide();
	Wt::WString wTemp("Data Table");
	auto tab = tabData->itemAt(1);
	tab->setText(wTemp);
	updateDownloadTab();
	tabData->setTabEnabled(1, 0);
	tabData->setCurrentIndex(0);
}
void SCDAwidget::resetTree()
{
	jtRegion.clear();
	treeRegion->setTreeRoot(make_unique<Wt::WTreeNode>(""));
}

void SCDAwidget::seriesAddToGraph()
{
	Wt::WString wsTemp;
	string temp;
	if (activeCata != wjBarGraph->activeCata)
	{
		wjBarGraph->reset();
		wjBarGraph->activeCata = activeCata;
	}
	if (wjBarGraph->unit == "" || wjBarGraph->region == "")
	{
		wsTemp = wjMap->textUnit->text();
		temp = wsTemp.toUTF8();
		size_t pos1 = temp.find(':');
		if (pos1 < temp.size()) { wjBarGraph->unit = temp.substr(pos1 + 2); }
		else { wjBarGraph->unit = temp; }

		string sRegion = tableData->getCell(-1, 0);
		wjBarGraph->region = sRegion;
	}
	if (wjBarGraph->linkIconTrash.isNull())
	{
		wjBarGraph->linkIconTrash = Wt::WLink(iconTrash);
	}
	if (wjBarGraph->linkIconClose.isNull())
	{
		wjBarGraph->linkIconClose = Wt::WLink(iconClose);
	}

	vector<vector<string>> vvsData = wtMap->getGraphData();
	vector<string> vsParameter = wjConfig->getTextLegend();
	wjBarGraph->addDataset(vvsData, vsParameter);
	wjBarGraph->display();

	if (!mobile && !setTip.count("barGraphWheel"))
	{
		wjBarGraph->tipSignal().connect(this, std::bind(&SCDAwidget::setTipAdd, this, std::placeholders::_1));
		wjBarGraph->addTipWheel(1);
	}
	wjBarGraph->ppUnique->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));
	wjBarGraph->ppDiff->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));
	wjBarGraph->ppCommon->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));

	wjMap->pbPinReset->setEnabled(1);
	wjMap->pbPinReset->decorationStyle().setBackgroundColor(wcSelectedWeak);

	tabData->setTabEnabled(3, 1);
	updateDownloadTab();
	tabData->setCurrentIndex(3);
}
void SCDAwidget::seriesRemoveFromGraph(const int& seriesIndex)
{
	int numSeries = wjBarGraph->removeDataset(seriesIndex);
	if (numSeries > 0)
	{
		wjBarGraph->display();
		wjBarGraph->ppUnique->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));
		wjBarGraph->ppDiff->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));
		wjBarGraph->ppCommon->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));
		wjDownload->pbPdfBarGraph->setEnabled(1);
	}
	else
	{
		wjDownload->pbPdfBarGraph->setEnabled(0);
		tabData->setTabEnabled(3, 0);
		tabData->setCurrentIndex(2);
	}
}

void SCDAwidget::setMap(int iRow, int iCol, string sRegion)
{
	// Assumes that all widgets have been updated already.
	// iCol corresponds to CB indices, not table indices!
	Wt::WString wsTemp;
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(6);
	prompt[0] = app->sessionId();
	wsTemp = wjConfig->wjpYear->cbTitle->currentText();
	prompt[1] = wsTemp.toUTF8();
	prompt[2] = activeCata;
	prompt[3] = sRegion;  // Parent region name. 
	prompt[4] = tableData->getUnit(wjConfig->wjpTopicRow->selMID);
	prompt[5] = tableData->getUnit(wjConfig->wjpTopicCol->selMID);

	vector<string> vsDIMtitle;
	vector<int> viMID;
	wjConfig->getVariable(vsDIMtitle, viMID);
	wjConfig->wjpTopicRow->appendSelf(vsDIMtitle, viMID);
	wjConfig->wjpTopicCol->appendSelf(vsDIMtitle, viMID);
	sRef.pullMap(prompt, vsDIMtitle, viMID);
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

	vector<string> vsDIMtitle;
	vector<int> viMID;
	wjConfig->getVariable(vsDIMtitle, viMID);
	sRef.pullTable(prompt, vsDIMtitle, viMID);
}

void SCDAwidget::tabChanged(const int& tabIndex)
{
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
		auto hLayout = make_unique<Wt::WHBoxLayout>();
		auto wjConfigUnique = this->removeWidget(wjConfig);
		auto boxDataUnique = this->removeWidget(boxData);
		wjConfig = hLayout->addWidget(move(wjConfigUnique));
		boxData = hLayout->addWidget(move(boxDataUnique));		
		this->setLayout(move(hLayout));
		wjConfig->setMaximumSize(300.0, wlAuto);
		boxData->setMaximumSize(screenWidth - 480, wlAuto);
		wjBarGraph->setMaximumSize(screenWidth - 490, wlAuto);
		widgetMobile();
	}
	else
	{
		mobile = 1;
		auto vLayout = make_unique<Wt::WVBoxLayout>();
		auto wjConfigUnique = this->removeWidget(wjConfig);
		auto boxDataUnique = this->removeWidget(boxData);
		wjConfig = vLayout->addWidget(move(wjConfigUnique));
		boxData = vLayout->addWidget(move(boxDataUnique));
		this->setLayout(move(vLayout));
		wjConfig->setMaximumSize(wlAuto, wlAuto);
		boxData->setMaximumSize(wlAuto, wlAuto);
		wjBarGraph->setMaximumSize(wlAuto, wlAuto);
		widgetMobile();
	}
	wjConfig->setMobile(mobile);
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
void SCDAwidget::updateDownloadTab()
{
	int numDataset;
	if (tabData->isTabEnabled(1) || tabData->isTabEnabled(2) || tabData->isTabEnabled(3))
	{
		tabData->setTabEnabled(4, 1);
		if (tabData->isTabEnabled(3) && wjBarGraph->model != nullptr)
		{
			//numDataset = wjBarGraph->model->columnCount() - 1;
			//if (numDataset > 0) { wjDownload->pbPdfBarGraph->setEnabled(1); }
			//else { wjDownload->pbPdfBarGraph->setEnabled(0); }
		}
	}
	else { tabData->setTabEnabled(4, 0); }
}
void SCDAwidget::updatePinButtons(string mapUnit)
{
	// This simplified variant is meant for unit changes from the map tab.
	Wt::WString wsTemp;
	string bgUnit = wjBarGraph->unit;
	if (bgUnit == "")
	{
		wjMap->pbPin->setEnabled(1);
		wjMap->pbPin->decorationStyle().setBackgroundColor(wcSelectedWeak);
		wjMap->pbPinReset->setEnabled(0);
		wjMap->pbPinReset->decorationStyle().setBackgroundColor(wcWhite);
	}
	else if (bgUnit == mapUnit)
	{
		wjMap->pbPin->setEnabled(1);
		wjMap->pbPin->decorationStyle().setBackgroundColor(wcSelectedWeak);
		wjMap->pbPinReset->setEnabled(1);
		wjMap->pbPinReset->decorationStyle().setBackgroundColor(wcSelectedWeak);
	}
	else
	{
		wjMap->pbPin->setEnabled(0);
		wjMap->pbPin->decorationStyle().setBackgroundColor(wcWhite);
		wjMap->pbPinReset->setEnabled(1);
		wjMap->pbPinReset->decorationStyle().setBackgroundColor(wcSelectedWeak);
		wsTemp = mapTooltip.at("pinRegionUnit");
		wjMap->pbPin->setToolTip(wsTemp);
	}
}
void SCDAwidget::updatePinButtons(string mapUnit, string sRegion)
{
	Wt::WString wsTemp;
	string bgUnit = wjBarGraph->unit;
	string bgRegion = wjBarGraph->region;
	if (bgUnit == "" || bgRegion == "")
	{
		wjMap->pbPin->setEnabled(1);
		wjMap->pbPin->decorationStyle().setBackgroundColor(wcSelectedWeak);
		wjMap->pbPinReset->setEnabled(0);
		wjMap->pbPinReset->decorationStyle().setBackgroundColor(wcWhite);
	}
	else if (bgUnit == mapUnit && bgRegion == sRegion)
	{
		wjMap->pbPin->setEnabled(1);
		wjMap->pbPin->decorationStyle().setBackgroundColor(wcSelectedWeak);
		wjMap->pbPinReset->setEnabled(1);
		wjMap->pbPinReset->decorationStyle().setBackgroundColor(wcSelectedWeak);
	}
	else
	{
		wjMap->pbPin->setEnabled(0);
		wjMap->pbPin->decorationStyle().setBackgroundColor(wcWhite);
		wjMap->pbPinReset->setEnabled(1);
		wjMap->pbPinReset->decorationStyle().setBackgroundColor(wcSelectedWeak);
		if (bgUnit == mapUnit) { wsTemp = mapTooltip.at("pinRegion"); }
		else if (bgRegion == sRegion) { wsTemp = mapTooltip.at("pinUnit"); }
		else { wsTemp = mapTooltip.at("pinRegionUnit"); }
		wjMap->pbPin->setToolTip(wsTemp);
	}
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
	wjMap->textUnit->setText(wTemp);

	if (wjBarGraph != nullptr) { updatePinButtons(sUnit); }

	vector<int> viIndex;
	if (sUnit == "% of population") { viIndex = { 0, 1 }; }
	else { viIndex = { 0 }; }
	wtMap->updateDisplay(viIndex);
}
void SCDAwidget::widgetMobile()
{
	if (mobile)
	{
		for (int ii = 0; ii < allPB.size(); ii++)
		{
			if (allPB[ii] != nullptr)
			{
				allPB[ii]->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
				allPB[ii]->setMinimumSize(wlAuto, 50.0);
			}
		}
		if (tabData != nullptr)
		{
			tabData->decorationStyle().font().setSize(Wt::FontSize::XLarge);
			tabData->setMinimumSize(wlAuto, 50.0);
		}
	}
	else
	{
		for (int ii = 0; ii < allPB.size(); ii++)
		{
			if (allPB[ii] != nullptr)
			{
				allPB[ii]->decorationStyle().font().setSize(Wt::FontSize::Medium);
				allPB[ii]->setMinimumSize(wlAuto, wlAuto);
			}
		}
		if (tabData != nullptr)
		{
			tabData->decorationStyle().font().setSize(Wt::FontSize::Medium);
			tabData->setMinimumSize(wlAuto, wlAuto);
		}
	}
	wjConfig->widgetMobile(mobile);
	wjTableBox->widgetMobile(mobile);
	wjMap->widgetMobile(mobile);
	wjBarGraph->widgetMobile(mobile);
}
