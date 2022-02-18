#include "SCDAwidget.h"

using namespace std;

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
		wjConfig->headerSignal().connect(this, bind(&SCDAwidget::incomingHeaderSignal, this, placeholders::_1, placeholders::_2));
		tabData->currentChanged().connect(this, bind(&SCDAwidget::tabChanged, this, placeholders::_1));
		wjTableBox->pbPinRow->clicked().connect(this, bind(&SCDAwidget::seriesAddToGraph, this, 0));
		wjTableBox->pbPinCol->clicked().connect(this, bind(&SCDAwidget::seriesAddToGraph, this, 1));
		wjTableBox->pbPinReset->clicked().connect(this, bind(&SCDAwidget::resetBarGraph, this));
		wjMap->pbPin->clicked().connect(this, bind(&SCDAwidget::seriesAddToGraph, this, 2));
		wjMap->pbPinReset->clicked().connect(this, bind(&SCDAwidget::resetBarGraph, this));
		wjDownload->previewSignal().connect(this, bind(&SCDAwidget::incomingPreviewSignal, this, placeholders::_1));
		wjTree->tree->itemSelectionChanged().connect(this, bind(&SCDAwidget::treeClicked, this));
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

void SCDAwidget::err(string message)
{
	string errorMessage = "SCDAwidget error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}

int SCDAwidget::getHeight()
{
	const Wt::WEnvironment& env = Wt::WApplication::instance()->environment();
	int iHeight = env.screenHeight();
	if (iHeight < 0) { err("Failed to obtain widget dimensions-wtpaint.getDimensions"); }
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
	unit = wjUnitPin.sUnitPreference;
	return unit;
}
int SCDAwidget::getWidth()
{
	const Wt::WEnvironment& env = Wt::WApplication::instance()->environment();
	int iWidth = env.screenWidth();
	if (iWidth < 0) { err("Failed to obtain widget dimensions-wtpaint.getDimensions"); }
	return iWidth;
}

void SCDAwidget::incomingHeaderSignal(const int& iRow, const int& iCol)
{
	// Either the config CBs or the data table is reporting a change in selected headers. 
	long long timeTable = tableData->jtime.timerReport();
	long long timeConfigCol = wjConfig->wjpTopicCol->jtime.timerReport();
	long long timeConfigRow = wjConfig->wjpTopicRow->jtime.timerReport();
	long long timeConfig = min(timeConfigCol, timeConfigRow);

	// Update the CB widgets or the data table's selected cell, then load a map. 
	string sRegion = tableData->getCell(-1, 0);
	int iRowSel, iColSel;
	if (timeConfig < timeTable) {  // CB values override table values.
		if (iRow < 0) { iRowSel = wjConfig->wjpTopicRow->getIndexMID(); }
		else { iRowSel = iRow; }
		if (iCol < 0) { iColSel = wjConfig->wjpTopicCol->getIndexMID(); }
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
		else {  // A row header was clicked. 
			iColSel = wjConfig->wjpTopicCol->getIndexMID();
			setMap(iRow, iColSel, sRegion);
		}
	}
}
void SCDAwidget::incomingPreviewSignal(const int& type)
{
	switch (type)
	{
	case 0:
	{
		vector<vector<double>> barGraphColour;
		vector<vector<vector<int>>> barGraphParameterColour(3, vector<vector<int>>());
		vector<vector<vector<string>>> barGraphParameterText(3, vector<vector<string>>());
		wjBarGraph->getParameterAll(barGraphColour, barGraphParameterColour, barGraphParameterText);
		vector<vector<string>> modelData = wjBarGraph->getModelValues();
		string sUnit = wjBarGraph->unit;
		vector<double> minMaxY = wjBarGraph->getMinMaxY();
		wjDownload->displayPDFbargraph(barGraphColour, barGraphParameterColour, barGraphParameterText, sUnit, modelData, minMaxY);
		break;
	}
	case 1:
	{
		vector<int> vChanged;
		vector<vector<string>> vvsParameter = wjConfig->getTextLegend(vChanged);
		wjDownload->displayPDFmap(vvsParameter, vChanged);
		break;
	}
	case 2:
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
		getTreeClicked(geoCode, vsPrompt[4]);
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
	if (activeCata.size() < 1) { err("No activeCata-SCDAwidget.incomingVarSignal"); }
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
	getTreeClicked(geoCode, vsPrompt[4]);
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
	if (filtersEnabled) { wlTableHeight = Wt::WLength(screenHeight - 274); }
	else { wlTableHeight = Wt::WLength(screenHeight - 234); }
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

	auto app = Wt::WApplication::instance();

	/*
	if (jsEnabled)
	{
		string getInfoJS = jsMakeFunctionTableWidth(tableData, "tableData");
		app->declareJavaScriptFunction("getInfo", getInfoJS);
		string scrollToJS = jsMakeFunctionTableScrollTo(tableData);
		app->declareJavaScriptFunction("scrollTo", scrollToJS);
	}
	*/

	jtime.timerStart();
	string sessionID = app->sessionId();
	sRef.pullConnection(sessionID);
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

	Wt::WString wsTooltip = "Click on a table cell to load a new map using that column and row.";
	mapTooltip.emplace("table", wsTooltip);
	wsTooltip = "Displayed region does not match the pinned region.";
	mapTooltip.emplace("pinRegion", wsTooltip);
	wsTooltip = "Displayed data unit does not match the pinned data unit.";
	mapTooltip.emplace("pinUnit", wsTooltip);
	wsTooltip = "Displayed region and data unit do not match the\npinned region and data unit.";
	mapTooltip.emplace("pinRegionUnit", wsTooltip);
	wsTooltip = "There are no currently-pinned data series\non the bar graph which can be reset.";
	mapTooltip.emplace("pinEmpty", wsTooltip);
	wsTooltip = "The existing bar graph has table row data pinned to it.\nReset the bar graph before pinning column or map data.";
	mapTooltip.emplace("pinRow", wsTooltip);
	wsTooltip = "The existing bar graph has table column data pinned to it.\nReset the bar graph before pinning row or map data.";
	mapTooltip.emplace("pinCol", wsTooltip);
	wsTooltip = "The existing bar graph has map data pinned to it.\nReset the bar graph before pinning table data.";
	mapTooltip.emplace("pinMap", wsTooltip);
	wsTooltip = "The existing bar graph has different x-axis names\ncompared to the current selection.";
	mapTooltip.emplace("pinChecksum", wsTooltip);

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
	//auto fnTree = std::bind(static_cast<void(SCDAwidget::*)(void)>(&SCDAwidget::treeClicked), this);
	//wjTree->tree->itemSelectionChanged().connect(fnTree);
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

	auto wjTreeUnique = make_unique<WJTREE>(screenWidth - 485, screenHeight - 130);
	wjTree = wjTreeUnique.get();
	tabData->addTab(move(wjTreeUnique), "Geographic Region", Wt::ContentLoading::Eager);
	//

	auto wjBoxTableUnique = make_unique<WJTABLEBOX>();
	wjTableBox = wjBoxTableUnique.get();
	wjTableBox->configXML = sRef.configXML;
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
unsigned SCDAwidget::makeParamChecksum(vector<vector<string>>& vvsParameter)
{
	// vvsParameter has form [parameter index][DIM title, MID ancestor0, ..., MID selected]
	string params;
	for (int ii = 0; ii < vvsParameter.size(); ii++) {
		for (int jj = 0; jj < vvsParameter[ii].size(); jj++) {
			params += vvsParameter[ii][jj];
		}
	}
	unsigned paramChecksum = jcrc.getChecksum(params);
	return paramChecksum;
}

void SCDAwidget::mapAreaClicked(int areaIndex)
{
	vector<int> vID;
	string sRegionClicked = wtMap->areaClicked(areaIndex);
	if (sRegionClicked == "bgArea") {
		auto selSet = wjTree->tree->selectedNodes();
		if (selSet.size() < 1) { return; }
		auto selIt = selSet.begin();
		auto selNode = *selIt;
		auto wTemp = selNode->label()->text();
		string sRegion = wTemp.toUTF8();
		vID = wjTree->jt.searchData(sRegion, 0);
		if (vID.size() != 1) { err("Failed to determine selected ID-mapAreaClicked"); }
		JNODE jnParent = wjTree->jt.getParent(vID[0]);
		vID[0] = jnParent.ID;
	}
	else {
		vID = wjTree->jt.searchData(sRegionClicked, 0);
		if (vID.size() != 1) { err("Failed to determine sRegion ID-mapAreaClicked"); }
	}	

	Wt::WTreeNode* nodeSel = wjTree->findNode(vID[0]);
	if (nodeSel == nullptr) { err("Find node by name-mapAreaClicked"); }
	if (!wjTree->tree->isSelected(nodeSel)) { wjTree->tree->select(nodeSel); }
}
void SCDAwidget::populateTextLegend(WJLEGEND*& wjLegend)
{
	Wt::WString wsTemp;
	vector<int> viChanged;
	wjLegend->vvsParameter = wjConfig->getTextLegend(viChanged);
	wjLegend->setColour(viChanged);
	wjLegend->display(1);
}
void SCDAwidget::populateTree(JTREE& jt, int parentID, Wt::WTreeNode*& parentNode)
{
	// Recursive function that takes an existing node and makes its children.
	Wt::WTreeNode* child = nullptr;
	unique_ptr<Wt::WText> text = nullptr;
	string sID;
	vector<int> childrenID = jt.getChildrenID(parentID);
	int numChildren = (int)childrenID.size();
	if (numChildren < 1) { return; }
	for (int ii = 0; ii < numChildren; ii++) {
		JNODE jn = jt.getNode(childrenID[ii]);
		sID = to_string(jn.ID);

		auto childUnique = make_unique<Wt::WTreeNode>(jn.vsData[0]);
		child = childUnique.get();
		parentNode->addChildNode(move(childUnique));
		child->setObjectName(sID);
		if (jn.expanded) { child->expand(); }

		populateTree(jt, childrenID[ii], child);
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
		break;
	case 2:  // Connection.
		updateTextCata(event.getNumCata());
		processEventConnection();
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
		processEventMap(event.get_list(), event.getFrames(), event.getAreas(), event.getRegionData());
		break;
	case 6:  // Parameter: create new panels with options for the user to specify.
		updateTextCata(event.getNumCata());
		processEventParameter(event.getParameter(), event.getCata());
		break;
	case 7:  // Table: populate the widget with data.
		tableCore = event.getTable();
		tableCol = event.getCol();
		tableRow = event.getRow();
		updateRegion(event.getNamePop());
		processEventTable(tableCore, tableCol, tableRow);
		break;
	case 8:  // Topic: update the GUI with row/column options.
		updateTextCata(event.getNumCata());
		processEventTopic(event.getListRow(), event.getListCol());
		break;
	case 9:  // Tree: populate the tree tab using the JTREE object.
		updateTextCata(event.getNumCata());
		wjTree->jt = event.getTree();
		processEventTree();
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
	wjConfig->wjpCategory->setCB(vsCategory);
	wjConfig->wjpCategory->highlight(0);
}
void SCDAwidget::processEventConnection()
{
	// Initialize the checksum object.
	auto app = Wt::WApplication::instance();
	string sessionID = app->sessionId();
	jcrc.init(sessionID);

	// Initialize custom icons.
	wjConfig->linkIconChevronDown = Wt::WLink(iconChevronDown);
	wjConfig->linkIconChevronRight = Wt::WLink(iconChevronRight);
	wjConfig->wjpTopicCol->initStackedPB(wjConfig->linkIconChevronDown, wjConfig->linkIconChevronRight);
	wjConfig->wjpTopicRow->initStackedPB(wjConfig->linkIconChevronDown, wjConfig->linkIconChevronRight);

	// Get the active year, and pull the category list from the server.
	Wt::WString wsYear = wjConfig->wjpYear->cbTitle->currentText();
	vector<string> prompt = { sessionID, wsYear.toUTF8() };
	sRef.pullCategory(prompt);
}
void SCDAwidget::processEventDemographic(vector<vector<string>> vvsDemo)
{
	wjConfig->vvsDemographic = vvsDemo;
	wjConfig->resetVariables(1);
	if (vvsDemo.size() > 0) { wjConfig->addDemographic(vvsDemo); }
	else { err("Invalid Demographic list-SCDAwidget.processEventDemographic"); }
}
void SCDAwidget::processEventDifferentiation(vector<string> vsDiff, string sTitle)
{
	// If the list contains DIM titles, then its last element must be "sTitle". Otherwise, 
	// the list contains MID elements for an existing Diff title.
	if (sTitle == "sTitle") { wjConfig->addDifferentiator(vsDiff); }
	else { wjConfig->addDifferentiator(vsDiff, sTitle); }
}
void SCDAwidget::processEventMap(vector<string> vsRegion, vector<vector<vector<double>>> vvvdFrame, vector<vector<vector<double>>> vvvdArea, vector<vector<double>> vvdData)
{
	wjMap->boxMap->clear();  // Erase the previous map.
	auto wtMapUnique = make_unique<WTPAINT>();
	wtMap = wjMap->boxMap->addWidget(move(wtMapUnique));
	populateTextLegend(wjMap->wjlMap);
	wtMap->jsb.addDataset(vvdData);
	if (wjMap->linkIconClose.isNull())
	{
		wjMap->linkIconClose = Wt::WLink(iconClose);
		wjMap->tipSignal().connect(this, std::bind(&SCDAwidget::setTipAdd, this, std::placeholders::_1));
		wjMap->addTipPin(2);
	}

	// Store the raw data, for potential PDF rendering.
	wjDownload->initMap(vsRegion, vvvdFrame, vvvdArea, vvdData);  

	// Determine the active unit.
	string unit0 = "# of persons";
	string unit1 = "% of population";
	string sUnitTable = getUnit();
	updateUnit(sUnitTable);
	int legendBarDouble, legendTickLines;
	Wt::WString wsUnit;
	Wt::WString wsUnitOld = wjMap->textUnit->text();
	if (sUnitTable == unit0 || sUnitTable == unit1)
	{
		wtMap->jsb.setUnit(0, unit0);
		wtMap->jsb.setUnit(1, "population");
		int indexJsb = wtMap->jsb.makeDataset({ 0, 1 }, '/');
		wtMap->jsb.setUnit(indexJsb, unit1);
		if (sUnitTable == unit0)
		{
			wjDownload->setUnit(unit0, { 0 });
			wtMap->jsb.activeIndex = 0;
			legendBarDouble = wjMap->getLegendBarDouble(vsRegion, unit0, 1);
		}
		else if (sUnitTable == unit1)
		{
			wjDownload->setUnit(unit1, { 0, 1 });
			wtMap->jsb.activeIndex = 2;
			legendBarDouble = wjMap->getLegendBarDouble(vsRegion, unit1, 2);
		}
		else { err("sUnitTable not recognized-SCDAwidget.processEventMap"); }
	}
	else
	{ 
		wjDownload->setUnit(sUnitTable, { 0 });
		wtMap->jsb.setUnit(0, sUnitTable);
		wtMap->jsb.activeIndex = 0; 
		legendBarDouble = wjMap->getLegendBarDouble(vsRegion, sUnitTable, 1);
	}
	legendTickLines = wjMap->getLegendTickLines(sUnitTable);

	// Paint the map widget.
	wtMap->legendBarDouble = legendBarDouble;
	wtMap->legendTickLines = legendTickLines;
	vector<Wt::WPolygonArea*> area = wtMap->drawMap(vsRegion, vvvdFrame, vvvdArea);
	for (int ii = 0; ii < area.size(); ii++)
	{
		function<void()> fnArea = bind(&SCDAwidget::mapAreaClicked, this, ii);
		area[ii]->clicked().connect(fnArea);
	}

	tabData->setTabEnabled(2, 1);
	updateDownloadTab();
	if (first) { tabData->setCurrentIndex(2); }	
	else { tabData->setCurrentIndex(tabRecent); }
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
	if (numCata != 1) { err("Multiple catalogues remain-SCDAwidget.processEventParameter"); }

	int sizeVar = wjConfig->varWJP.size();
	activeCata = vvsCata[0][1];

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
void SCDAwidget::processEventTable(vector<vector<string>>& vvsTable, vector<vector<string>>& vvsCol, vector<vector<string>>& vvsRow)
{
	if (vvsCol.size() < 3 || vvsRow.size() < 3) { err("Missing col/row titles-SCDAwidget.processEventTable"); }
	if (vvsTable.size() < 1) { err("Missing table data-SCDAwidget.processEventTable"); }
	if (wjUnitPin.activeRegion.size() < 1) { err("Missing table region-SCDAwidget.processEventTable"); }

	auto app = Wt::WApplication::instance();
	string sTitleCol = vvsCol[vvsCol.size() - 1][0];  // Currently unused.
	string sTitleRow = vvsRow[vvsRow.size() - 1][0];  // Currently unused.
	vvsCol.pop_back();
	vvsRow.pop_back();

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
	string temp = "Data Table (" + wjUnitPin.activeRegion + ")";
	Wt::WString wTemp = Wt::WString::fromUTF8(temp);
	auto tab = tabData->itemAt(1);
	tab->setText(wTemp);

	// Make a new table widget.
	vector<string> vsNamePop = { wjUnitPin.activeRegion, wjUnitPin.sRegionPopulation };
	if (wjUnitPin.sUnitPreference == "# of persons") {
		vsNamePop.push_back("# of persons");
	}
	else {
		vsNamePop.push_back("% of population");
	}
	tableData = wjTableBox->setTable(vvsTable, vvsCol, vvsRow, vsNamePop);
	wjTableBox->setTableSize(wlTableWidth, wlTableHeight);
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

	// Update the table and map widgets with the current bar graph mode.
	updatePinButtons();

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
void SCDAwidget::processEventTree()
{
	// Use the JTREE received from the server to populate the tree region widget.

	string sRoot;
	JNODE jnRoot = wjTree->jt.getRoot();
	if (jnRoot.vsData.size() > 0) { sRoot = jnRoot.vsData[0]; }
	else { sRoot = " "; }
	Wt::WString wsTemp = Wt::WString::fromUTF8(sRoot);
	auto treeRootUnique = make_unique<Wt::WTreeNode>(wsTemp);
	treeRootUnique->setLoadPolicy(Wt::ContentLoading::Eager);
	treeRootUnique->setNodeVisible(0);
	auto treeRoot = treeRootUnique.get();
	wjTree->tree->setTreeRoot(move(treeRootUnique));
	populateTree(wjTree->jt, jnRoot.ID, treeRoot);
	tabData->setTabEnabled(0, 1);
	treeRoot = wjTree->tree->treeRoot();
	treeRoot->expand();
	vector<Wt::WTreeNode*> vChildren = treeRoot->childNodes();
	int numChildren = (int)vChildren.size();
	if (numChildren < 1) { err("Root has no children-processEventTree"); }
	for (int ii = 0; ii < numChildren; ii++) {
		vChildren[ii]->expand();
	}
	wjTree->tree->select(vChildren[0]);
}

void SCDAwidget::resetBarGraph()
{
	if (!tabData->isTabEnabled(3)) { return; }
	wjBarGraph->reset();
	updatePinButtons(-1);
	tabData->setTabEnabled(3, 0);
}
void SCDAwidget::resetDownload()
{
	if (wjDownload != nullptr) { wjDownload->clear(); }
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
	//updateDownloadTab();
	tabData->setTabEnabled(1, 0);
	tabData->setCurrentIndex(0);
}
void SCDAwidget::resetTree()
{
	wjTree->jt.reset();
	Wt::WTreeNode* treeRoot = wjTree->tree->treeRoot();
	if (treeRoot == nullptr) { return; }
	vector<Wt::WTreeNode*> vTreeNode = treeRoot->childNodes();
	for (int ii = vTreeNode.size() - 1; ii >= 0; ii--) {
		treeRoot->removeChildNode(vTreeNode[ii]);
	}
}

void SCDAwidget::seriesAddToGraph(int mode)
{
	Wt::WString wsTemp;
	string params, parentRegion, tableHeader;
	if (activeCata != wjBarGraph->activeCata)
	{
		wjBarGraph->reset();
		wjBarGraph->activeCata = activeCata;
	}
	if (wjBarGraph->region == "") {
		wjBarGraph->region = wjUnitPin.activeRegion;
	}
	if (wjBarGraph->unit == "") {
		wjBarGraph->unit = wjUnitPin.activeUnit;
	}
	if (wjBarGraph->linkIconTrash.isNull())
	{
		wjBarGraph->linkIconTrash = Wt::WLink(iconTrash);
	}
	if (wjBarGraph->linkIconClose.isNull())
	{
		wjBarGraph->linkIconClose = Wt::WLink(iconClose);
	}

	vector<int> rowColSel;
	int numSeries = wjBarGraph->getNumSeries();
	vector<vector<string>> vvsData, vvsParameter = wjConfig->getTextLegend();
	if (mode == 0) {
		vvsParameter.erase(vvsParameter.begin() + 2);  // Column parameters ARE the x-axis.
	}
	else if (mode == 1) {
		vvsParameter.erase(vvsParameter.begin() + 3);  // Row parameters ARE the x-axis.
	}
	unsigned paramChecksum = makeParamChecksum(vvsParameter);
	if (wjBarGraph->mapChecksumIndex.count(paramChecksum)) { return; }
	else {
		wjBarGraph->mapChecksumIndex.emplace(paramChecksum, numSeries);
		wjBarGraph->vChecksum.push_back(paramChecksum);
	}

	switch (mode)
	{
	case 0:  // Table row data.
		vvsData = wjTableBox->getBargraphRow();
		break;
	case 1:  // Table column data.
		vvsData = wjTableBox->getBargraphCol();
		break;
	case 2:  // Map data.
		vvsData = wtMap->getGraphData();
		break;
	}

	if (numSeries == 0)
	{
		switch (mode)
		{
		case 2:
		{
			parentRegion = tableData->getCell(-1, 0);
			wjBarGraph->region = parentRegion;
			vector<string> parentRow;
			for (int ii = 0; ii < vvsData.size(); ii++)
			{
				if (vvsData[ii][0] == parentRegion)
				{
					parentRow = vvsData[ii];
					vvsData.erase(vvsData.begin() + ii);
					jsort.sortAlphabetically(vvsData, 0);
					vvsData.insert(vvsData.begin(), parentRow);
					break;
				}
			}
			break;
		}
		}

		string xNames;
		for (int ii = 0; ii < vvsData.size(); ii++) {
			xNames += vvsData[ii][0];
		}
		wjUnitPin.xChecksum = jcrc.getChecksum(xNames);
		wjBarGraph->xChecksum = wjUnitPin.xChecksum;
	}
	wjBarGraph->addDataset(vvsData, vvsParameter);
	wjBarGraph->display();

	if (!setTip.count("barGraphWheel"))
	{
		wjBarGraph->tipSignal().connect(this, std::bind(&SCDAwidget::setTipAdd, this, std::placeholders::_1));
		wjBarGraph->addTipWheel(1);
	}
	wjBarGraph->ppUnique->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));
	wjBarGraph->ppDiff->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));
	wjBarGraph->ppCommon->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));

	updatePinButtons(mode);

	tabData->setTabEnabled(3, 1);
	updateDownloadTab();
	tabData->setCurrentIndex(3);
}
void SCDAwidget::seriesRemoveFromGraph(const int& seriesIndex)
{
	if (seriesIndex >= wjBarGraph->mapChecksumIndex.size()) { err("Bar graph parameter index not found-SCDAwidget.seriesRemoveFromGraph"); }
	unsigned checksum = wjBarGraph->vChecksum[seriesIndex];
	wjBarGraph->vChecksum.erase(wjBarGraph->vChecksum.begin() + seriesIndex);
	auto it = wjBarGraph->mapChecksumIndex.find(checksum);
	wjBarGraph->mapChecksumIndex.erase(it);
	int index;
	for (int ii = seriesIndex; ii < wjBarGraph->vChecksum.size(); ii++) {
		checksum = wjBarGraph->vChecksum[seriesIndex];
		index = wjBarGraph->mapChecksumIndex.at(checksum);
		index--;
	}

	int numSeries = wjBarGraph->removeDataset(seriesIndex);
	if (numSeries > 0)
	{
		wjBarGraph->display();
		wjBarGraph->ppUnique->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));
		wjBarGraph->ppDiff->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));
		wjBarGraph->ppCommon->deleteSignal().connect(this, std::bind(&SCDAwidget::seriesRemoveFromGraph, this, std::placeholders::_1));
	}
	else
	{
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
	switch (tabIndex)
	{
	case 1:
		tabRecent = 1;
		break;
	case 2:
		tabRecent = 2;
		break;
	case 4:
	{
		if (wjDownload->wbGroup == nullptr) { break; }
		Wt::WRadioButton* wrb0 = wjDownload->wbGroup->button(0);
		if (tabData->isTabEnabled(3)) {
			wrb0->setEnabled(1);
		}
		else {
			wrb0->setEnabled(0);
		}
		Wt::WRadioButton* wrb1 = wjDownload->wbGroup->button(1);
		Wt::WRadioButton* wrb2 = wjDownload->wbGroup->button(2);
		if (tabData->isTabEnabled(2)) {
			wrb1->setEnabled(1);
			wrb2->setEnabled(1);
		}
		else {
			wrb1->setEnabled(0);
			wrb2->setEnabled(0);
		}
		int selIndex = wjDownload->getRadioIndex();
		incomingPreviewSignal(selIndex);
		break;
	}
	}
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

void SCDAwidget::treeClicked()
{
	// This variant automatically launches a new table pull.
	auto selSet = wjTree->tree->selectedNodes();
	if (selSet.size() < 1) { return; }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	string sRegion = wTemp.toUTF8();
	vector<int> vID = wjTree->jt.searchData(sRegion, 0);
	if (vID.size() != 1) { err("Failed to determine sRegion ID-treeClicked"); }
	JNODE jn = wjTree->jt.getNode(vID[0]);
	int geoCode = stoi(jn.vsData[1]);
	setTable(geoCode, sRegion);
}
void SCDAwidget::getTreeClicked(int& geoCode, string& sRegion)
{
	// This variant simply returns the geoCode and name of the selected region. 
	auto selSet = wjTree->tree->selectedNodes();
	if (selSet.size() < 1) { return; }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	sRegion = wTemp.toUTF8();
	vector<int> vID = wjTree->jt.searchData(sRegion, 0);
	if (vID.size() != 1) { err("Failed to determine sRegion ID-treeClicked"); }
	JNODE jn = wjTree->jt.getNode(vID[0]);
	geoCode = stoi(jn.vsData[1]);
}

void SCDAwidget::updateDownloadTab()
{
	int numDataset;
	if (tabData->isTabEnabled(1) || tabData->isTabEnabled(2) || tabData->isTabEnabled(3))
	{
		tabData->setTabEnabled(4, 1);
	}
	else { tabData->setTabEnabled(4, 0); }
}
void SCDAwidget::updatePinButtons()
{
	int mode = wjBarGraph->bgMode;
	updatePinButtons(mode);
}
void SCDAwidget::updatePinButtons(int mode)
{
	wjBarGraph->bgMode = mode;
	string bgUnit = wjBarGraph->unit;
	string bgRegion = wjBarGraph->region;
	unsigned bgChecksum = wjBarGraph->xChecksum;
	int fracture = 0;
	if (bgUnit.size() > 0 && bgUnit != wjUnitPin.activeUnit) { fracture = 1; }
	if (bgRegion.size() > 0 && bgRegion != wjUnitPin.activeRegion) { fracture = 2; }
	if (bgChecksum > 0 && bgChecksum != wjUnitPin.xChecksum) { fracture = 3; }

	switch (fracture)
	{
	case 0:
	{
		switch (mode)
		{
		case -1:
			wjTableBox->updatePinButtons({ "", "", "pinEmpty" });
			wjMap->updatePinButtons({ "", "pinEmpty" });
			break;
		case 0:
			wjTableBox->updatePinButtons({ "", "pinRow", "" });
			wjMap->updatePinButtons({ "pinRow", "" });
			break;
		case 1:
			wjTableBox->updatePinButtons({ "pinCol", "", "" });
			wjMap->updatePinButtons({ "pinCol", "" });
			break;
		case 2:
			wjTableBox->updatePinButtons({ "pinMap", "pinMap", "" });
			wjMap->updatePinButtons({ "", "" });
			break;
		}
		break;
	}
	case 1:
	{
		switch (mode)
		{
		case -1:
			wjTableBox->updatePinButtons({ "", "", "pinEmpty" });
			wjMap->updatePinButtons({ "", "pinEmpty" });
			break;
		case 0:
			wjTableBox->updatePinButtons({ "pinUnit", "pinRow", "" });
			wjMap->updatePinButtons({ "pinRow", "" });
			break;
		case 1:
			wjTableBox->updatePinButtons({ "pinCol", "pinUnit", "" });
			wjMap->updatePinButtons({ "pinCol", "" });
			break;
		case 2:
			wjTableBox->updatePinButtons({ "pinMap", "pinMap", "" });
			wjMap->updatePinButtons({ "pinUnit", "" });
			break;
		}
		break;
	}
	case 2:
	{
		switch (mode)
		{
		case -1:
			wjTableBox->updatePinButtons({ "", "", "pinEmpty" });
			wjMap->updatePinButtons({ "", "pinEmpty" });
			break;
		case 0:
			wjTableBox->updatePinButtons({ "pinRegion", "pinRow", "" });
			wjMap->updatePinButtons({ "pinRow", "" });
			break;
		case 1:
			wjTableBox->updatePinButtons({ "pinCol", "pinRegion", "" });
			wjMap->updatePinButtons({ "pinCol", "" });
			break;
		case 2:
			wjTableBox->updatePinButtons({ "pinMap", "pinMap", "" });
			wjMap->updatePinButtons({ "pinRegion", "" });
			break;
		}
		break;
	}
	case 3:
	{
		switch (mode)
		{
		case -1:
			wjTableBox->updatePinButtons({ "", "", "pinEmpty" });
			wjMap->updatePinButtons({ "", "pinEmpty" });
			break;
		case 0:
			wjTableBox->updatePinButtons({ "pinChecksum", "pinRow", "" });
			wjMap->updatePinButtons({ "pinRow", "" });
			break;
		case 1:
			wjTableBox->updatePinButtons({ "pinCol", "pinChecksum", "" });
			wjMap->updatePinButtons({ "pinCol", "" });
			break;
		case 2:
			wjTableBox->updatePinButtons({ "pinMap", "pinMap", "" });
			wjMap->updatePinButtons({ "pinChecksum", "" });
			break;
		}
		break;
	}
	}
}
void SCDAwidget::updateRegion(vector<string> vsNamePop)
{
	// vsNamePop has form [region name, sRegionPopulation].
	if (vsNamePop.size() != 2) { err("Invalid vsNamePop-SCDAwidget.updateRegion"); }
	wjUnitPin.activeRegion = vsNamePop[0];
	wjUnitPin.sRegionPopulation = vsNamePop[1];
	try {
		wjUnitPin.regionPopulation = stoi(vsNamePop[1]);
	}
	catch (invalid_argument) { err("stoi-SCDAwidget.updateRegion"); }
}
void SCDAwidget::updateTextCata(int numCata)
{
	Wt::WString wsTemp;
	if (numCata < 1) { err("Less than one catalogue remains-SCDAwidget.updateTextCata"); }
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
	// If the active unit is already correct, no actions are necessary.
	if (sUnit == wjUnitPin.activeUnit) { return; }

	// Re-create the menu (as necessary) with connections to this function.
	string unit0 = "# of persons";
	string unit1 = "% of population";
	vector<Wt::WMenuItem*> vwmItem;
	Wt::WString wsTemp = Wt::WString::fromUTF8("Unit: " + sUnit);
	wjTableBox->textUnit->setText(wsTemp);
	wjMap->textUnit->setText(wsTemp);
	if (sUnit == unit0 || sUnit == unit1) {
		if (wjUnitPin.activeUnit == unit0 || wjUnitPin.activeUnit == unit1) {
			int index;
			vector<string> vsNamePop = { wjUnitPin.activeRegion, wjUnitPin.sRegionPopulation };
			wjUnitPin.activeUnit = sUnit;
			if (sUnit[0] == '#') {
				wjUnitPin.sUnitPreference = unit0;
				vsNamePop.push_back(unit0);
				index = 0;
			}
			else if (sUnit[0] == '%') {
				wjUnitPin.sUnitPreference = unit1;
				vsNamePop.push_back(unit1);
				index = 1;
			}
			else { err("Unknown unit-SCDAwidget.updateUnit"); }
			vector<int> rowColSel;
			tableData = wjTableBox->updateTable(vsNamePop, rowColSel);
			tableData->headerSignal().connect(this, bind(&SCDAwidget::incomingHeaderSignal, this, placeholders::_1, placeholders::_2));
			tableData->tableClickedSimulated(rowColSel[0], rowColSel[1]);
			updatePinButtons();
			wjTableBox->popupUnit->select(index);
			wjMap->popupUnit->select(index);
			return;
		}
		else {
			wjTableBox->resetMenu();
			wjMap->resetMenu();
		}
		wjUnitPin.activeUnit = sUnit;
		function<void()> fnUnit0 = bind(&SCDAwidget::updateUnit, this, unit0);
		function<void()> fnUnit1 = bind(&SCDAwidget::updateUnit, this, unit1);
		wjTableBox->popupUnit->addItem(unit0)->triggered().connect(fnUnit0);
		wjTableBox->popupUnit->addItem(unit1)->triggered().connect(fnUnit1);
		wjMap->popupUnit->addItem(unit0)->triggered().connect(fnUnit0);
		wjMap->popupUnit->addItem(unit1)->triggered().connect(fnUnit1);
	}
	else {
		wjUnitPin.activeUnit = sUnit;
		wjTableBox->resetMenu();
		wjMap->resetMenu();
		function<void()> fnUnit = bind(&SCDAwidget::updateUnit, this, sUnit);
		wjTableBox->popupUnit->addItem(sUnit)->triggered().connect(fnUnit);
		wjMap->popupUnit->addItem(sUnit)->triggered().connect(fnUnit);
	}

	// Update the pin buttons.
	updatePinButtons();

	// Select the active unit.
	if (sUnit == unit1) {
		wjTableBox->popupUnit->select(1);
		wjMap->popupUnit->select(1);
	}
	else {
		wjTableBox->popupUnit->select(0);
		wjMap->popupUnit->select(0);
	}
}
