#include "wjconfig.h"

void WJPANEL::applyFilter(int cbIndex, vector<int>& viFilter)
{
	Wt::WString wsTemp;
	int currentIndex, indent;
	int keepIndex = -1;
	switch (cbIndex)
	{
	case 0:
	{
		currentIndex = cbTitle->currentIndex();
		cbTitle->clear();
		mapTitleIndex.clear();
		mapTitleIndent.clear();
		for (int ii = 0; ii < viFilter.size(); ii++)
		{
			if (viFilter[ii] == currentIndex) { keepIndex = ii; }
			mapTitleIndex.emplace(vsTitle[viFilter[ii]], ii);
			indent = 0;
			while (vsTitle[viFilter[ii]][indent] == '+') { indent++; }
			mapTitleIndent.emplace(ii, indent);
			wsTemp = Wt::WString::fromUTF8(vsTitle[viFilter[ii]]);
			cbTitle->addItem(wsTemp);
		}
		if (keepIndex >= 0) { cbTitle->setCurrentIndex(keepIndex); }
		else { cbTitle->setCurrentIndex(0); }
		cbTitle->setHidden(0);
		break;
	}
	case 1:
	{
		currentIndex = cbMID->currentIndex();
		cbMID->clear();
		mapMIDIndex.clear();
		mapMIDIndent.clear();
		for (int ii = 0; ii < viFilter.size(); ii++)
		{
			if (viFilter[ii] == currentIndex) { keepIndex = ii; }
			mapMIDIndex.emplace(vsMID[viFilter[ii]], ii);
			indent = 0;
			while (vsMID[viFilter[ii]][indent] == '+') { indent++; }
			mapMIDIndent.emplace(ii, indent);
			wsTemp = Wt::WString::fromUTF8(vsMID[viFilter[ii]]);
			cbMID->addItem(wsTemp);
		}
		if (keepIndex >= 0) { cbMID->setCurrentIndex(keepIndex); }
		else { cbMID->setCurrentIndex(0); }
		cbMID->setHidden(0);
		break;
	}
	}
}
void WJPANEL::clear()
{
	mapMIDIndex.clear();
	mapTitleIndex.clear();
	cbMID->clear();
	cbMID = nullptr;
	cbTitle->clear();
	cbTitle = nullptr;
	pbDialog = nullptr;
}
void WJPANEL::highlight(int widgetIndex)
{
	// The chosen CB has its borders become dotted, and the title gains a coloured background.
	this->setHidden(0);
	Wt::WContainerWidget* cwTitle = titleBarWidget();
	Wt::WCssDecorationStyle style = cwTitle->decorationStyle();
	if (widgetIndex < 2)
	{
		style.setBackgroundColor(wcSelectedWeak);
		cwTitle->setDecorationStyle(style);
	}
	switch (widgetIndex)
	{
	case 0:
		if (cbTitle->count() > 0)
		{
			style = cbTitle->decorationStyle();
			style.setBorder(Wt::WBorder(Wt::BorderStyle::Dotted));
			cbTitle->setDecorationStyle(style);
			cbTitle->setHidden(0);
		}
		break;
	case 1:
		if (cbMID->count() > 0)
		{
			style = cbMID->decorationStyle();
			style.setBorder(Wt::WBorder(Wt::BorderStyle::Dotted));
			cbMID->setDecorationStyle(style);
			cbMID->setHidden(0);
		}
		break;
	case 2:  // Filter button.
		style = pbDialog->decorationStyle();
		style.setBackgroundColor(wcSelectedWeak);
		pbDialog->setDecorationStyle(style);
		pbDialog->setHidden(0);
		break;
	}
}
void WJPANEL::init()
{
	auto wBox = make_unique<Wt::WContainerWidget>();
	auto layout = make_unique<Wt::WVBoxLayout>();
	cbTitle = layout->addWidget(make_unique<Wt::WComboBox>());
	cbMID = layout->addWidget(make_unique<Wt::WComboBox>());
	pbDialog = layout->addWidget(make_unique<Wt::WPushButton>());
	wBox->setLayout(move(layout));
	setCentralWidget(move(wBox));

	cbMID->setHidden(1);
	pbDialog->setHidden(1);

	wcBorder.setRgb(204, 204, 204);
	wcOffWhite.setRgb(245, 245, 245);
	wcSelectedWeak.setRgb(200, 200, 255);
	wcWhite.setRgb(255, 255, 255);
	wbDefaultCB = Wt::WBorder(Wt::BorderStyle::Solid, 1.0, wcBorder);
}
void WJPANEL::unhighlight(int widgetIndex)
{
	// The chosen CB has its borders become solid, and the title gains a white background.
	Wt::WContainerWidget* cwTitle = titleBarWidget();
	Wt::WCssDecorationStyle style = cwTitle->decorationStyle();
	if (widgetIndex < 2)
	{
		style.setBackgroundColor(wcOffWhite);
		cwTitle->setDecorationStyle(style);
	}
	switch (widgetIndex)
	{
	case 0:
		if (cbTitle->count() > 0)
		{
			Wt::WCssDecorationStyle& style = cbTitle->decorationStyle();
			style.setBorder(wbDefaultCB);
		}
		break;
	case 1:
		if (cbMID->count() > 0)
		{
			Wt::WCssDecorationStyle& style = cbMID->decorationStyle();
			style.setBorder(wbDefaultCB);
		}
		break;
	case 2:  // Filter button.
		style = pbDialog->decorationStyle();
		style.setBackgroundColor(wcWhite);
		pbDialog->setDecorationStyle(style);
		break;
	}
}
void WJPANEL::resetMapIndex(int cbIndex)
{
	switch (cbIndex)
	{
	case 0:
	{
		mapTitleIndex.clear();
		for (int ii = 0; ii < vsTitle.size(); ii++)
		{
			mapTitleIndex.emplace(vsTitle[ii], ii);
		}
		break;
	}
	case 1:
	{
		mapMIDIndex.clear();
		for (int ii = 0; ii < vsMID.size(); ii++)
		{
			mapMIDIndex.emplace(vsMID[ii], ii);
		}
		break;
	}
	}
}
void WJPANEL::setCB(int cbIndex, vector<string>& vsList)
{
	int index = 0;
	setCB(cbIndex, vsList, index);
}
void WJPANEL::setCB(int cbIndex, vector<string>& vsList, int iActive)
{
	Wt::WString wsTemp;
	int indent;
	switch (cbIndex)
	{
	case 0:
	{
		cbTitle->clear();
		mapTitleIndex.clear();
		mapTitleIndent.clear();
		vsTitle = vsList;
		for (int ii = 0; ii < vsList.size(); ii++)
		{
			mapTitleIndex.emplace(vsList[ii], ii);
			indent = 0;
			while (vsList[ii][indent] == '+') { indent++; }
			mapTitleIndent.emplace(ii, indent);
			wsTemp = Wt::WString::fromUTF8(vsList[ii]);
			cbTitle->addItem(wsTemp);
		}
		cbTitle->setCurrentIndex(iActive);
		cbTitle->setHidden(0);
		break;
	}
	case 1:
	{
		cbMID->clear();
		mapMIDIndex.clear();
		mapMIDIndent.clear();
		vsMID = vsList;
		for (int ii = 0; ii < vsList.size(); ii++)
		{
			mapMIDIndex.emplace(vsList[ii], ii);
			indent = 0;
			while (vsList[ii][indent] == '+') { indent++; }
			mapMIDIndent.emplace(ii, indent);
			wsTemp = Wt::WString::fromUTF8(vsList[ii]);
			cbMID->addItem(wsTemp);
		}
		cbMID->setCurrentIndex(iActive);
		cbMID->setHidden(0);
		break;
	}
	}
}
void WJPANEL::setCB(int cbIndex, vector<string>& vsList, string sActive)
{
	Wt::WString wsTemp;
	int indent;
	switch (cbIndex)
	{
	case 0:
	{
		cbTitle->clear();
		mapTitleIndex.clear();
		mapTitleIndent.clear();
		vsTitle = vsList;
		int index = 0;
		for (int ii = 0; ii < vsList.size(); ii++)
		{
			if (vsList[ii] == sActive) { index = ii; }
			mapTitleIndex.emplace(vsList[ii], ii);
			indent = 0;
			while (vsList[ii][indent] == '+') { indent++; }
			mapTitleIndent.emplace(ii, indent);
			wsTemp = Wt::WString::fromUTF8(vsList[ii]);
			cbTitle->addItem(wsTemp);
		}
		cbTitle->setCurrentIndex(index);
		cbTitle->setHidden(0);
		break;
	}
	case 1:
	{
		cbMID->clear();
		mapMIDIndex.clear();
		mapMIDIndent.clear();
		vsMID = vsList;
		int index = 0;
		for (int ii = 0; ii < vsList.size(); ii++)
		{
			if (vsList[ii] == sActive) { index = ii; }
			mapMIDIndex.emplace(vsList[ii], ii);
			indent = 0;
			while (vsList[ii][indent] == '+') { indent++; }
			mapMIDIndent.emplace(ii, indent);
			wsTemp = Wt::WString::fromUTF8(vsList[ii]);
			cbMID->addItem(wsTemp);
		}
		cbMID->setCurrentIndex(index);
		cbMID->setHidden(0);
		break;
	}
	}
}

void WJCONFIG::addDemographic(vector<vector<string>>& vvsDemo)
{
	// Create a "Demographic" panel from which the user must choose an option,
	// in order to reduce the number of potential catalogues. vvsDemo has
	// form [forWhom index][forWhom, sYear@sCata0, sYear@sCata1, ...].
	auto demoPanelUnique = make_unique<WJPANEL>();
	demoPanelUnique->setTitle("Demographic Group");
	demoPanelUnique->setMaximumSize(wlAuto, 150.0);
	wjpDemo = demoPanelUnique.get();

	vector<string> vsDemo = { "[None selected]" };
	for (int ii = 0; ii < vvsDemo.size(); ii++)
	{
		vsDemo.push_back(vvsDemo[ii][0]);
	}
	wjpDemo->setCB(0, vsDemo);
	wjpDemo->highlight(0);
	wjpDemo->cbTitle->changed().connect(this, &WJCONFIG::demographicChanged);

	vLayout->addWidget(move(demoPanelUnique));
}
void WJCONFIG::addDifferentiator(vector<string> vsDiff)
{
	// Create a normal-looking Parameter panel, containing differentiating 
	// DIM titles to try and pinpoint a catalogue. 
	int index = diffWJP.size();

	auto wjpUnique = make_unique<WJPANEL>();
	diffWJP.push_back(wjpUnique.get());
	string temp = wjpUnique->id();
	mapDiffIndex.emplace(temp, index);
	diffWJP[index]->setTitle("Parameter");

	string sTitle = "[None selected]";
	vsDiff.insert(vsDiff.begin(), sTitle);
	diffWJP[index]->setCB(0, vsDiff);

	function<void()> fnDiffTitle = bind(&WJCONFIG::diffTitleChanged, this, temp);
	diffWJP[index]->cbTitle->changed().connect(fnDiffTitle);

	diffWJP[index]->highlight(0);
	vLayout->addWidget(move(wjpUnique));
}
void WJCONFIG::addVariable(vector<string>& vsVariable)
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

	int index = varWJP.size();
	auto wjpUnique = make_unique<WJPANEL>();
	wjpUnique->setTitle("Parameter");
	varWJP.push_back(wjpUnique.get());
	varWJP[index]->setMaximumSize(wlAuto, 200.0);
	temp = varWJP[index]->id();
	mapVarIndex.emplace(temp, index);

	varWJP[index]->setCB(0, vsTitle, titleIndex);
	function<void()> fnVarTitle = bind(&WJCONFIG::varTitleChanged, this, temp);
	varWJP[index]->cbTitle->changed().connect(fnVarTitle);

	varWJP[index]->setCB(1, vsMID, MIDIndex);
	varWJP[index]->cbMID->changed().connect(this, &WJCONFIG::varMIDChanged);
	varWJP[index]->cbMID->clicked().connect(this, &WJCONFIG::varMIDClicked);

	vLayout->addWidget(move(wjpUnique));
}
void WJCONFIG::categoryChanged()
{
	// Populate the topic widgets with options for the user
	// to choose column and row topics. 
	resetSignal_.emit(0);  // Reset map.
	resetSignal_.emit(1);  // Reset table.
	resetSignal_.emit(2);  // Reset tree.
	resetVariables();
	resetTopicSel();
	Wt::WString wsYear = wjpYear->cbTitle->currentText();
	int index = wjpCategory->cbTitle->currentIndex();
	if (index == 0)
	{
		wjpTopicCol->setHidden(1);
		wjpTopicRow->setHidden(1);
		wjpCategory->highlight(0);
		return;
	}

	//wjpCategory->unhighlight(0);
	vector<string> prompt(5);
	Wt::WString wsTemp = wjpCategory->cbTitle->currentText();
	prompt[0] = "";
	prompt[1] = wsYear.toUTF8();
	prompt[2] = wsTemp.toUTF8();
	prompt[3] = "*";  // Column topic.
	prompt[4] = "*";  // Row topic.

	setPrompt(prompt);
	pullSignal_.emit(3);  // Pull topic.
}
void WJCONFIG::cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem)
{
	// Repopulate the combobox with the given list, showing the top item. 
	cb->clear();
	for (int ii = 0; ii < vsItem.size(); ii++)
	{
		cb->addItem(vsItem[ii]);
	}
	cb->setCurrentIndex(0);
}
void WJCONFIG::cbRenew(Wt::WComboBox*& cb, vector<string>& vsItem, string sActive)
{
	// Repopulate the combobox with the selected item shown. 
	int index = 0;
	cb->clear();
	for (int ii = 0; ii < vsItem.size(); ii++)
	{
		cb->addItem(vsItem[ii]);
		if (vsItem[ii] == sActive) { index = ii; }
	}
	cb->setCurrentIndex(index);
}
void WJCONFIG::demographicChanged()
{
	// Depending on the number of surviving catalogues, launch a "variable" event.
	wjpTopicCol->cbMID->clear();
	wjpTopicCol->cbMID->setHidden(1);
	wjpTopicCol->pbDialog->setHidden(1);

	wjpTopicRow->cbMID->clear();
	wjpTopicRow->cbMID->setHidden(1);
	wjpTopicRow->pbDialog->setHidden(1);

	resetSignal_.emit(0);  // Reset map.
	resetSignal_.emit(1);  // Reset table.
	resetSignal_.emit(2);  // Reset tree.
	resetTopicSel();

	Wt::WString wsTemp = wjpDemo->cbTitle->currentText();
	if (wsTemp == wsNoneSel) { wjpDemo->highlight(0); }
	else { wjpDemo->unhighlight(0); }

	wsTemp = wjpYear->cbTitle->currentText();
	vector<string> prompt = { "", wsTemp.toUTF8() };  // sID, sYear
	vector<string> vsCata = getDemo();
	if (vsCata.size() < 1) { return; }
	else if (vsCata.size() == 1)
	{
		prompt.resize(6);
		size_t pos1 = vsCata[0].find('@');
		prompt[1] = vsCata[0].substr(0, pos1);  // Internal year.
		prompt[5] = vsCata[0].substr(pos1 + 1);  // sCata
		wsTemp = wjpCategory->cbTitle->currentText();
		prompt[2] = wsTemp.toUTF8();
		wsTemp = wjpTopicCol->cbTitle->currentText();
		prompt[3] = wsTemp.toUTF8();
		wsTemp = wjpTopicRow->cbTitle->currentText();
		prompt[4] = wsTemp.toUTF8();
		vector<vector<string>> vvsDummy;
		setPrompt(prompt, vvsDummy);  
		pullSignal_.emit(4);  // Pull variable. Prompt has form [id, year, category, colTopic, rowTopic, activeCata].
	}
	else
	{
		vector<vector<string>> vvsCata = getDemoSplit(vsCata);
		setPrompt(prompt, vvsCata);
		pullSignal_.emit(1);  // Pull differentiator.
	}
}
void WJCONFIG::dialogSubsectionFilterCol()
{
	auto wDialog = make_unique<Wt::WDialog>("Choose a column item to display exclusively with its interior items -");

	string sID;
	jtCol.expandGeneration = -1;  // Expand all nodes automatically.
	string sRoot = jtCol.getRootName();
	Wt::WString wsTemp = Wt::WString::fromUTF8(sRoot);
	auto treeRootUnique = make_unique<Wt::WTreeNode>(wsTemp);
	treeRootUnique->setLoadPolicy(Wt::ContentLoading::Eager);
	auto treeRoot = treeRootUnique.get();
	treeNodeSel = nullptr;
	if (sFilterCol.size() > 0)
	{
		populateTree(jtCol, treeRoot, sFilterCol);
		if (treeNodeSel == nullptr) { jf.err("Failed to locate previous tree node-wjconfig.dialogSubsectionFilterCol"); }
	}
	else { populateTree(jtCol, treeRoot); }
	treeRootUnique->setNodeVisible(0);

	auto tree = make_unique<Wt::WTree>();
	tree->setSelectionMode(Wt::SelectionMode::Single);
	tree->setTreeRoot(move(treeRootUnique));
	if (treeNodeSel != nullptr) { tree->select(treeNodeSel); }

	tree->itemSelectionChanged().connect(this, &WJCONFIG::dialogSubsectionFilterColEnd);
	treeDialogCol = tree.get();
	auto wBox = wDialog->contents();
	wBox->setMaximumSize(wlAuto, 600.0);
	wBox->setOverflow(Wt::Overflow::Auto);
	wBox->addWidget(move(tree));
	wDialog->show();
	swap(wDialog, wdFilter);
}
void WJCONFIG::dialogSubsectionFilterColEnd()
{
	auto selSet = treeDialogCol->selectedNodes();
	if (selSet.size() < 1) { return; }
	if (selSet.size() > 1) { jf.err("Invalid selection-WJCONFIG.dialogColSubsectionFilterEnd"); }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	sFilterCol = wTemp.toUTF8();
	wdFilter->reject();
	unique_ptr<Wt::WDialog> wdDummy = nullptr;
	swap(wdFilter, wdDummy);

	vector<string> vsChildren;
	jtCol.listChildrenAll(sFilterCol, vsChildren);
	wjpTopicCol->resetMapIndex(1);
	m_config.lock();
	viFilterCol.resize(vsChildren.size() + 1);
	viFilterCol[0] = wjpTopicCol->mapMIDIndex.at(sFilterCol);
	for (int ii = 0; ii < vsChildren.size(); ii++)
	{
		viFilterCol[ii + 1] = wjpTopicCol->mapMIDIndex.at(vsChildren[ii]);
	}
	m_config.unlock();
	wjpTopicCol->applyFilter(1, viFilterCol);
	filterSignal_.emit();
}
void WJCONFIG::dialogSubsectionFilterRow()
{
	auto wDialog = make_unique<Wt::WDialog>("Choose a row item to display exclusively with its interior items -");

	string sID;
	jtRow.expandGeneration = -1;  // Expand all nodes automatically.
	string sRoot = jtRow.getRootName();
	Wt::WString wsTemp = Wt::WString::fromUTF8(sRoot);
	auto treeRootUnique = make_unique<Wt::WTreeNode>(wsTemp);
	treeRootUnique->setLoadPolicy(Wt::ContentLoading::Eager);
	auto treeRoot = treeRootUnique.get();
	treeNodeSel = nullptr;
	if (sFilterRow.size() > 0)
	{
		populateTree(jtRow, treeRoot, sFilterRow);
		if (treeNodeSel == nullptr) { jf.err("Failed to locate previous tree node-wjconfig.dialogSubsectionFilterRow"); }
	}
	else { populateTree(jtRow, treeRoot); }
	treeRootUnique->setNodeVisible(0);

	auto tree = make_unique<Wt::WTree>();
	tree->setSelectionMode(Wt::SelectionMode::Single);
	tree->setTreeRoot(move(treeRootUnique));
	if (treeNodeSel != nullptr) { tree->select(treeNodeSel); }

	tree->itemSelectionChanged().connect(this, &WJCONFIG::dialogSubsectionFilterRowEnd);
	treeDialogRow = tree.get();
	auto wBox = wDialog->contents();
	wBox->setMaximumSize(wlAuto, 600.0);
	wBox->setOverflow(Wt::Overflow::Auto);
	wBox->addWidget(move(tree));
	wDialog->show();
	swap(wDialog, wdFilter);
}
void WJCONFIG::dialogSubsectionFilterRowEnd()
{
	auto selSet = treeDialogRow->selectedNodes();
	if (selSet.size() < 1) { return; }
	if (selSet.size() > 1) { jf.err("Invalid selection-WJCONFIG.dialogRowSubsectionFilterEnd"); }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	sFilterRow = wTemp.toUTF8();
	wdFilter->reject();
	unique_ptr<Wt::WDialog> wdDummy = nullptr;
	swap(wdFilter, wdDummy);

	vector<string> vsChildren;
	jtRow.listChildrenAll(sFilterRow, vsChildren);
	wjpTopicRow->resetMapIndex(1);
	m_config.lock();
	viFilterRow.resize(vsChildren.size() + 1);
	viFilterRow[0] = wjpTopicRow->mapMIDIndex.at(sFilterRow);
	for (int ii = 0; ii < vsChildren.size(); ii++)
	{
		viFilterRow[ii + 1] = wjpTopicRow->mapMIDIndex.at(vsChildren[ii]);
	}
	m_config.unlock();
	wjpTopicRow->applyFilter(1, viFilterRow);
	filterSignal_.emit();
}
void WJCONFIG::diffTitleChanged(string id)
{
	int index = mapDiffIndex.at(id);
	Wt::WString wsTemp = diffWJP[index]->cbTitle->currentText();
	if (wsTemp == wsNoneSel)
	{
		diffWJP[index]->highlight(0);
		return;
	}
	
	string sTitle = wsTemp.toUTF8();
	vector<vector<string>> vvsFixed = getVariable();
	vector<string> prompt(vvsFixed.size() + 1);
	prompt[0] = "";
	for (int ii = 0; ii < vvsFixed.size(); ii++)
	{
		prompt[ii + 1] = vvsFixed[ii][0];
	}
	vector<vector<string>> vvsCata = getDemoSplit();
	setPrompt(prompt, vvsCata);
	pullSignal_.emit(1);  // Pull differentiator.
}
vector<string> WJCONFIG::getDemo()
{
	// Return form [sYear0@sCata0, sYear1@sCata1, ...].
	vector<string> vsCata;
	if (vvsDemographic.size() < 1) { jf.err("No Demographic init-SCDAwidget.getDemo"); }
	if (wjpDemo == nullptr)
	{
		vsCata.assign(vvsDemographic[0].begin() + 1, vvsDemographic[0].end());
		return vsCata;
	}
	Wt::WString wsDemo = wjpDemo->cbTitle->currentText();
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
vector<vector<string>> WJCONFIG::getDemoSplit()
{
	// Return form [year index][sYear, sCata0, sCata1, ...].
	vector<string> vsCata = getDemo();
	return getDemoSplit(vsCata);
}
vector<vector<string>> WJCONFIG::getDemoSplit(vector<string>& vsCata)
{
	// Return form [year index][sYear, sCata0, sCata1, ...].
	string sYear, sCata;
	vector<vector<string>> vvsCata(1, vector<string>(2));
	size_t pos1 = vsCata[0].find('@');
	vvsCata[0][0] = vsCata[0].substr(0, pos1);
	vvsCata[0][1] = vsCata[0].substr(pos1 + 1);
	for (int ii = 1; ii < vsCata.size(); ii++)
	{
		pos1 = vsCata[ii].find('@');
		sYear = vsCata[ii].substr(0, pos1);
		sCata = vsCata[ii].substr(pos1 + 1);
		for (int jj = 0; jj < vvsCata.size(); jj++)
		{
			if (vvsCata[jj][0] == sYear)
			{
				vvsCata[jj].push_back(sCata);
				break;
			}
			else if (jj == vvsCata.size() - 1)
			{
				vvsCata.push_back({ sYear, sCata });
			}
		}
	}
	return vvsCata;
}
vector<vector<int>> WJCONFIG::getFilterTable()
{
	// Return form [row, col][permitted indices].
	vector<vector<int>> vviFilter(2, vector<int>());
	m_config.lock();
	vviFilter[0] = viFilterRow;
	vviFilter[1] = viFilterCol;
	m_config.unlock();
	return vviFilter;
}
void WJCONFIG::getPrompt(vector<string>& vsP)
{
	lock_guard<mutex> lg(m_config);
	vsP = vsPrompt;
}
void WJCONFIG::getPrompt(vector<string>& vsP, vector<vector<string>>& vvsP)
{
	lock_guard<mutex> lg(m_config);
	vsP = vsPrompt;
	vvsP = vvsPrompt;
}
vector<Wt::WString> WJCONFIG::getTextLegend()
{
	// Read all the CB panels, and return a list of the displayed options. 
	int numParam = basicWJP.size() + varWJP.size();
	if (wjpDemo != nullptr) { numParam++; }

	size_t pos1;
	int indent, indentMID, indexMID, indexLegend = 0;
	string temp, sTitle;
	Wt::WString wsTitle;
	vector<Wt::WString> vwsMID;
	vector<Wt::WString> vwsLegend(numParam);
	for (int ii = 0; ii < basicWJP.size(); ii++)
	{
		if (ii < 2)
		{
			vwsLegend[ii] = basicWJP[ii]->title() + " | ";
			vwsLegend[ii] += basicWJP[ii]->cbTitle->currentText();
		}
		else
		{
			wsTitle = basicWJP[ii]->cbTitle->currentText();
			vwsLegend[ii] = wsTitle;
			indexMID = basicWJP[ii]->cbMID->currentIndex();
			indentMID = basicWJP[ii]->mapMIDIndent.at(indexMID);
			if (indentMID == 0) { vwsMID = { basicWJP[ii]->cbMID->currentText() }; }
			else
			{
				temp = basicWJP[ii]->cbMID->currentText().toUTF8();
				while (temp[0] == '+') { temp.erase(temp.begin()); }
				vwsMID = { Wt::WString::fromUTF8(temp) };
			}
			for (int jj = indentMID - 1; jj >= 0; jj--)
			{
				indexMID--;
				while (indexMID >= 0)
				{
					indent = basicWJP[ii]->mapMIDIndent.at(indexMID);
					if (indent == jj)
					{
						if (indent > 0)
						{
							temp = basicWJP[ii]->cbMID->itemText(indexMID).toUTF8();
							while (temp[0] == '+') { temp.erase(temp.begin()); }
							vwsMID.push_back(Wt::WString::fromUTF8(temp));
						}
						else { vwsMID.push_back(basicWJP[ii]->cbMID->itemText(indexMID)); }
						break;
					}
					else { indexMID--; }
				}
			}
			for (int jj = vwsMID.size() - 1; jj >= 0; jj--)
			{
				temp = vwsMID[jj].toUTF8();
				pos1 = temp.find("Total -");
				if (pos1 > temp.size()) { vwsLegend[ii] += " | " + vwsMID[jj]; }
				else
				{
					sTitle = wsTitle.toUTF8();
					pos1 = temp.find(sTitle);
					if (pos1 > temp.size()) { vwsLegend[ii] += " | " + vwsMID[jj]; }
					else if (jj == 0) { vwsLegend[ii] += " | Total"; }
				}
			}
		}
		indexLegend++;
	}
	if (wjpDemo != nullptr)
	{
		vwsLegend[indexLegend] = wjpDemo->title() + " | " + wjpDemo->cbTitle->currentText();
		indexLegend++;
	}
	for (int ii = 0; ii < varWJP.size(); ii++)
	{
		wsTitle = varWJP[ii]->cbTitle->currentText();
		vwsLegend[indexLegend] = wsTitle;
		indexMID = varWJP[ii]->cbMID->currentIndex();
		indentMID = varWJP[ii]->mapMIDIndent.at(indexMID);
		if (indentMID == 0) { vwsMID = { varWJP[ii]->cbMID->currentText() }; }
		else
		{
			temp = varWJP[ii]->cbMID->currentText().toUTF8();
			while (temp[0] == '+') { temp.erase(temp.begin()); }
			vwsMID = { Wt::WString::fromUTF8(temp) };
		}
		for (int jj = indentMID - 1; jj >= 0; jj--)
		{
			indexMID--;
			while (indexMID >= 0)
			{
				indent = varWJP[ii]->mapMIDIndent.at(indexMID);
				if (indent == jj)
				{
					if (indent > 0)
					{
						temp = varWJP[ii]->cbMID->itemText(indexMID).toUTF8();
						while (temp[0] == '+') { temp.erase(temp.begin()); }
						vwsMID.push_back(Wt::WString::fromUTF8(temp));
					}
					else { vwsMID.push_back(varWJP[ii]->cbMID->itemText(indexMID)); }
					break;
				}
				else { indexMID--; }
			}
		}
		for (int jj = vwsMID.size() - 1; jj >= 0; jj--)
		{
			temp = vwsMID[jj].toUTF8();
			pos1 = temp.find("Total -");
			if (pos1 > temp.size()) { vwsLegend[indexLegend] += " | " + vwsMID[jj]; }
			else
			{
				sTitle = wsTitle.toUTF8();
				pos1 = temp.find(sTitle);
				if (pos1 > temp.size()) { vwsLegend[indexLegend] += " | " + vwsMID[jj]; }
				else if (jj == 0) { vwsLegend[indexLegend] += " | Total"; }
			}
		}
		indexLegend++;
	}
	return vwsLegend;
}
vector<vector<string>> WJCONFIG::getVariable()
{
	// Checks the active state of each "variable" panel and returns each title and MID.
	vector<vector<string>> vvsVariable;
	string sTitle, sMID;
	Wt::WString wsTemp;
	int numVar;
	if (diffWJP.size() > 0)
	{
		numVar = diffWJP.size();
		vvsVariable.resize(numVar);
		sMID = "*";
		for (int ii = 0; ii < numVar; ii++)
		{
			wsTemp = diffWJP[ii]->cbTitle->currentText();
			sTitle = wsTemp.toUTF8();
			vvsVariable[ii] = { sTitle, sMID };
		}
	}
	else
	{
		numVar = varWJP.size();
		vvsVariable.resize(numVar);
		for (int ii = 0; ii < numVar; ii++)
		{
			wsTemp = varWJP[ii]->cbTitle->currentText();
			sTitle = wsTemp.toUTF8();
			wsTemp = varWJP[ii]->cbMID->currentText();
			sMID = wsTemp.toUTF8();
			if (sMID == "") { sMID = "*"; }
			vvsVariable[ii] = { sTitle, sMID };
		}
	}
	return vvsVariable;
}
void WJCONFIG::init(vector<string> vsYear)
{
	// Initialize colours. 
	wcSelectedWeak.setRgb(200, 200, 255);
	wcSelectedStrong.setRgb(150, 150, 192);
	wcWhite.setRgb(255, 255, 255, 255);

	// Initialize borders. 
	wbDotted = Wt::WBorder(Wt::BorderStyle::Dotted);
	wbSolid = Wt::WBorder(Wt::BorderStyle::Solid);

	// Initialize WCssStyle objects. 
	wcssDefault = Wt::WCssDecorationStyle();
	wcssAttention = wcssDefault;
	wcssAttention.setBorder(wbDotted);
	wcssHighlighted = wcssDefault;
	wcssHighlighted.setBackgroundColor(wcSelectedWeak);

	// Insert the widgets into the layout.
	auto layout = make_unique<Wt::WVBoxLayout>();
	vLayout = layout.get();
	auto pbMobileUnique = make_unique<Wt::WPushButton>("Toggle Mobile Version");
	pbMobile = pbMobileUnique.get();
	layout->addWidget(move(pbMobileUnique));
	auto textCataUnique = make_unique<Wt::WText>();
	textCata = textCataUnique.get();
	layout->addWidget(move(textCataUnique));
	unique_ptr<WJPANEL> year = makePanelYear(vsYear);
	layout->addWidget(move(year));
	unique_ptr<WJPANEL> category = makePanelCategory();
	layout->addWidget(move(category));
	unique_ptr<WJPANEL> topicCol = makePanelTopicCol();
	layout->addWidget(move(topicCol));
	unique_ptr<WJPANEL> topicRow = makePanelTopicRow();
	layout->addWidget(move(topicRow));
	this->setLayout(move(layout));

	// Specify initial size limitations.
	setMaximumSize(300.0, wlAuto);
	wjpYear->setMaximumSize(wlAuto, 150.0);
	wjpCategory->setMaximumSize(wlAuto, 150.0);
	wjpTopicCol->setMaximumSize(wlAuto, 200.0);
	wjpTopicRow->setMaximumSize(wlAuto, 200.0);
	pbMobile->setEnabled(1);
	pbMobile->setMaximumSize(wlAuto, 50.0);

	// Connect signals to functions. 
	string sID;
	wjpCategory->cbTitle->changed().connect(this, &WJCONFIG::categoryChanged);
	sID = wjpTopicCol->id();
	wjpTopicCol->cbTitle->changed().connect(this, std::bind(&WJCONFIG::topicTitleChanged, this, sID));
	sID = wjpTopicRow->id();
	wjpTopicRow->cbTitle->changed().connect(this, std::bind(&WJCONFIG::topicTitleChanged, this, sID));

	wjpTopicCol->cbMID->clicked().connect(this, &WJCONFIG::varMIDClicked);
	wjpTopicCol->cbMID->changed().connect(this, &WJCONFIG::varMIDChanged);
	wjpTopicRow->cbMID->clicked().connect(this, &WJCONFIG::varMIDClicked);
	wjpTopicRow->cbMID->changed().connect(this, &WJCONFIG::varMIDChanged);

	wjpTopicRow->pbDialog->clicked().connect(this, &WJCONFIG::dialogSubsectionFilterRow);
	wjpTopicCol->pbDialog->clicked().connect(this, &WJCONFIG::dialogSubsectionFilterCol);
}
void WJCONFIG::initJTree(vector<string>& vsRow, vector<string>& vsCol)
{
	// Populates the JTREE objects describing the row and column indentation hierarchies.
	int indent;
	string sParent;
	vector<int> indentHistory;
	jtRow.init("No Filter", -1);
	for (int ii = 0; ii < vsRow.size(); ii++)
	{
		indent = wjpTopicRow->mapMIDIndent.at(ii);
		if (indentHistory.size() <= indent) { indentHistory.push_back(ii); }
		else
		{
			indentHistory[indent] = ii;
			indentHistory.resize(indent + 1);
		}

		if (indent == 0) { jtRow.addChild(vsRow[ii], -2, -1); }
		else
		{
			sParent = vsRow[indentHistory[indentHistory.size() - 2]];
			jtRow.addChild(vsRow[ii], -2, sParent);
		}
	}
	wjpTopicRow->pbDialog->clicked().connect(this, &WJCONFIG::dialogSubsectionFilterRow);

	indentHistory.clear();
	jtCol.init("No Filter", -1);
	for (int ii = 0; ii < vsCol.size(); ii++)
	{
		indent = wjpTopicCol->mapMIDIndent.at(ii);
		if (indentHistory.size() <= indent) { indentHistory.push_back(ii); }
		else
		{
			indentHistory[indent] = ii;
			indentHistory.resize(indent + 1);
		}

		if (indent == 0) { jtCol.addChild(vsCol[ii], -2, -1); }
		else
		{
			sParent = vsCol[indentHistory[indentHistory.size() - 2]];
			jtCol.addChild(vsCol[ii], -2, sParent);
		}
	}
	wjpTopicCol->pbDialog->clicked().connect(this, &WJCONFIG::dialogSubsectionFilterCol);
}
unique_ptr<WJPANEL> WJCONFIG::makePanelCategory()
{
	auto category = make_unique<WJPANEL>();
	category->setTitle("Topical Category");
	wjpCategory = category.get();
	basicWJP.push_back(wjpCategory);
	wjpCategory->setHidden(1);
	return category;
}
unique_ptr<WJPANEL> WJCONFIG::makePanelTopicCol()
{
	auto topicCol = make_unique<WJPANEL>();
	topicCol->setTitle("Table Column Headers");
	topicCol->pbDialog->setText("Column Subsection Filter");
	wjpTopicCol = topicCol.get();
	basicWJP.push_back(wjpTopicCol);
	wjpTopicCol->setHidden(1);
	return topicCol;
}
unique_ptr<WJPANEL> WJCONFIG::makePanelTopicRow()
{
	auto topicRow = make_unique<WJPANEL>();
	topicRow->setTitle("Table Row Headers");
	topicRow->pbDialog->setText("Row Subsection Filter");
	wjpTopicRow = topicRow.get();
	basicWJP.push_back(wjpTopicRow);
	wjpTopicRow->setHidden(1);
	return topicRow;
}
unique_ptr<WJPANEL> WJCONFIG::makePanelYear(vector<string> vsYear)
{
	auto year = make_unique<WJPANEL>();
	year->setTitle("Census Year");
	wjpYear = year.get();
	wjpYear->setCB(0, vsYear);
	basicWJP.push_back(wjpYear);
	wjpYear->cbTitle->changed().connect(this, std::bind(&WJCONFIG::yearChanged, this));
	wjpYear->setHidden(0);
	wjpYear->cbTitle->setEnabled(1);
	return year;
}
void WJCONFIG::populateTree(JTREE& jt, Wt::WTreeNode*& node)
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
void WJCONFIG::populateTree(JTREE& jt, Wt::WTreeNode*& node, string sName)
{
	// Recursive function that takes an existing node and makes its children.
	// This variant will return a pointer to the node with name sName.
	vector<string> vsChildren;
	Wt::WString wTemp = node->label()->text();
	string sNode = wTemp.toUTF8();
	if (jt.isExpanded(sNode)) { node->expand(); }
	string sID = node->id();
	jt.mapSID.emplace(sNode, sID);
	jt.listChildren(sNode, vsChildren);
	for (int ii = 0; ii < vsChildren.size(); ii++)
	{
		wTemp = Wt::WString::fromUTF8(vsChildren[ii]);
		auto childUnique = make_unique<Wt::WTreeNode>(wTemp);
		auto child = childUnique.get();
		populateTree(jt, child, sName);
		if (vsChildren[ii] == sName)
		{
			treeNodeSel = node->addChildNode(move(childUnique));
		}
		else { node->addChildNode(move(childUnique)); }
	}
}
void WJCONFIG::removeDifferentiators()
{
	for (int ii = 0; ii < mapDiffIndex.size(); ii++)
	{
		vLayout->removeWidget(diffWJP[ii]);
	}
	diffWJP.clear();
	mapDiffIndex.clear();
}
void WJCONFIG::removeVariable(int varIndex)
{
	// Remove the given variable's panel widget. Update mapVarIndex, varPanel, varWJC;
	WJPANEL* wjp = varWJP[varIndex];
	string sID = wjp->id();
	mapVarIndex.erase(sID);
	vLayout->removeWidget(wjp);
	wjp->clear();

	// Correct the subsequent map values. 
	for (int ii = varIndex; ii < mapVarIndex.size(); ii++)
	{
		sID = varWJP[ii]->id();
		mapVarIndex.erase(sID);
		mapVarIndex.emplace(sID, ii);
	}
}
void WJCONFIG::resetTopicSel()
{
	wjpTopicCol->cbMID->clear();
	wjpTopicCol->cbMID->setHidden(1);
	wjpTopicCol->pbDialog->setHidden(1);

	wjpTopicRow->cbMID->clear();
	wjpTopicRow->cbMID->setHidden(1);
	wjpTopicRow->pbDialog->setHidden(1);
}
void WJCONFIG::resetVariables()
{
	// Remove all "variable" panels and clear all "variable" buffers.
	int plus = 0;
	resetVariables(plus);
}
void WJCONFIG::resetVariables(int plus)
{
	// Remove all "variable" panels and clear all "variable" buffers.
	for (int ii = varWJP.size() - 1; ii >= 0; ii--)
	{
		vLayout->removeWidget(varWJP[ii]);
		varWJP.pop_back();
	}

	// Remove the demographic widgets.
	if (wjpDemo != nullptr && plus < 1)
	{
		vLayout->removeWidget(wjpDemo);
		wjpDemo = nullptr;
		vvsDemographic.clear();
	}	
}
void WJCONFIG::setPrompt(vector<string>& vsP)
{
	lock_guard<mutex> lg(m_config);
	vsPrompt = vsP;
	vvsPrompt.clear();
}
void WJCONFIG::setPrompt(vector<string>& vsP, vector<vector<string>>& vvsP)
{
	lock_guard<mutex> lg(m_config);
	vsPrompt = vsP;
	vvsPrompt = vvsP;
}
void WJCONFIG::topicSelChanged()
{
	// Use the current col/row MID selections to update the table's selected cell.
	if (wjpTopicRow->cbMID->isHidden() || wjpTopicCol->cbMID->isHidden()) { return; }
	int iRow = wjpTopicRow->cbMID->currentIndex();
	int iCol = wjpTopicCol->cbMID->currentIndex();
	headerSignal_.emit(iRow, iCol);
}
void WJCONFIG::topicSelClicked()
{
	jf.timerStart();
}
void WJCONFIG::topicTitleChanged(string id)
{
	// When a column or row topic is specified, obtain an updated listing for
	// its opposite (column/row), and for its mirror (side panel/input table).
	resetVariables();
	vector<string> prompt(5);
	Wt::WString wsTemp;
	prompt[0] = "";
	wsTemp = wjpYear->cbTitle->currentText();
	prompt[1] = wsTemp.toUTF8();
	wsTemp = wjpCategory->cbTitle->currentText();
	prompt[2] = wsTemp.toUTF8();

	int indexCol, indexRow;
	if (id == wjpTopicCol->id())  // The user specified the column topic title.
	{
		indexCol = wjpTopicCol->cbTitle->currentIndex();
		if (indexCol == 0)
		{
			prompt[3] = "*";
			prompt[4] = "*";
		}
		else
		{
			wsTemp = wjpTopicCol->cbTitle->currentText();
			prompt[3] = wsTemp.toUTF8();
			indexRow = wjpTopicRow->cbTitle->currentIndex();
			if (indexRow == 0) { prompt[4] = "*"; }
			else
			{
				wsTemp = wjpTopicRow->cbTitle->currentText();
				prompt[4] = wsTemp.toUTF8();
			}
		}
	}
	else if (id == wjpTopicRow->id())
	{
		indexRow = wjpTopicRow->cbTitle->currentIndex();
		if (indexRow == 0)
		{
			prompt[3] = "*";
			prompt[4] = "*";
		}
		else
		{
			wsTemp = wjpTopicRow->cbTitle->currentText();
			prompt[4] = wsTemp.toUTF8();
			indexCol = wjpTopicCol->cbTitle->currentIndex();
			if (indexCol == 0) { prompt[3] = "*"; }
			else
			{
				wsTemp = wjpTopicCol->cbTitle->currentText();
				prompt[3] = wsTemp.toUTF8();
			}
		}
	}
	else { jf.err("Unknown input id-wjconfig.topicTitleChanged"); }

	if (prompt[3] == "*")
	{
		wjpTopicCol->cbMID->clear();
		wjpTopicCol->cbMID->setHidden(1);
		wjpTopicRow->pbDialog->setHidden(1);
		wjpTopicCol->pbDialog->setHidden(1);
	}
	if (prompt[4] == "*")
	{
		wjpTopicRow->cbMID->clear();
		wjpTopicRow->cbMID->setHidden(1);
		wjpTopicRow->pbDialog->setHidden(1);
		wjpTopicCol->pbDialog->setHidden(1);
	}
	if (prompt[3] == "*" && prompt[4] == "*")
	{
		resetSignal_.emit(0);  // Reset map.
		resetSignal_.emit(1);  // Reset table.
		resetSignal_.emit(2);  // Reset tree.
	}

	// Query the server for the next stage, depending on the current state of specificity.
	if (prompt[3] == "*" || prompt[4] == "*")
	{
		setPrompt(prompt);  // Rows or columns not yet determined.
		pullSignal_.emit(3);  // Pull topic.
	}
	else
	{
		wjpTopicCol->unhighlight(0);
		wjpTopicRow->unhighlight(0);
		vector<vector<string>> vvsDummy;
		setPrompt(prompt, vvsDummy);
		pullSignal_.emit(4);  // Pull variable.
	}
}
void WJCONFIG::varMIDChanged()
{
	vector<string> prompt(5);
	Wt::WString wsTemp = wjpYear->cbTitle->currentText();
	prompt[1] = wsTemp.toUTF8();
	vector<vector<string>> variable = getVariable();
	setPrompt(prompt, variable);
	pullSignal_.emit(2);  // Pull table.
}
void WJCONFIG::varMIDClicked()
{
	jf.timerStart();
}
void WJCONFIG::varTitleChanged(string id)
{
	int removeVar = -1;
	int index = mapVarIndex.at(id);
	Wt::WString wsTemp = varWJP[index]->cbTitle->currentText();
	string sTitleClicked = wsTemp.toUTF8();
	string sTop = "[None selected]";
	string sTitle;
	if (sTitleClicked == sTop)
	{
		varWJP[index]->setTitle("Select a parameter ...");
		varWJP[index]->cbMID->clear();
		varWJP[index]->cbMID->setEnabled(0);
		return;
	}
	else  // If this title already exists in a panel, delete that panel. (There can be only one ... !)
	{
		for (int ii = 0; ii < varWJP.size(); ii++)
		{
			if (ii == index) { continue; }
			wsTemp = varWJP[ii]->cbTitle->currentText();
			sTitle = wsTemp.toUTF8();
			if (sTitle == sTitleClicked)
			{
				removeVar = ii;
				break;
			}
		}
		if (removeVar >= 0) { removeVariable(removeVar); }
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

	varWJP[index]->cbTitle->setEnabled(1);
	varWJP[index]->cbMID->setEnabled(1);
	varWJP[index]->setCB(1, vsMID);
	varWJP[index]->setTitle("Parameter");
	varMIDChanged();
}
