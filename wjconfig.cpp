#include "wjconfig.h"

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
	auto style = cwTitle->decorationStyle();
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

	wcSelectedWeak.setRgb(200, 200, 255);
	wcWhite.setRgb(255, 255, 255, 255);
}
void WJPANEL::unhighlight(int widgetIndex)
{
	// The chosen CB has its borders become solid, and the title gains a white background.
	Wt::WContainerWidget* cwTitle = titleBarWidget();
	auto style = cwTitle->decorationStyle();
	if (widgetIndex < 2)
	{
		style.setBackgroundColor(wcWhite);
		cwTitle->setDecorationStyle(style);
	}
	switch (widgetIndex)
	{
	case 0:
		if (cbTitle->count() > 0)
		{
			style = cbTitle->decorationStyle();
			style.setBorder(Wt::WBorder(Wt::BorderStyle::Solid));
			cbTitle->setDecorationStyle(style);
		}
		break;
	case 1:
		if (cbMID->count() > 0)
		{
			style = cbMID->decorationStyle();
			style.setBorder(Wt::WBorder(Wt::BorderStyle::Solid));
			cbMID->setDecorationStyle(style);
		}
		break;
	case 2:  // Filter button.
		style = pbDialog->decorationStyle();
		style.setBackgroundColor(wcWhite);
		pbDialog->setDecorationStyle(style);
		break;
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
	switch (cbIndex)
	{
	case 0:
	{
		cbTitle->clear();
		mapTitleIndex.clear();
		for (int ii = 0; ii < vsList.size(); ii++)
		{
			mapTitleIndex.emplace(vsList[ii], ii);
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
		for (int ii = 0; ii < vsList.size(); ii++)
		{
			mapMIDIndex.emplace(vsList[ii], ii);
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
	switch (cbIndex)
	{
	case 0:
	{
		cbTitle->clear();
		mapTitleIndex.clear();
		int index = 0;
		for (int ii = 0; ii < vsList.size(); ii++)
		{
			if (vsList[ii] == sActive) { index = ii; }
			mapTitleIndex.emplace(vsList[ii], ii);
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
		int index = 0;
		for (int ii = 0; ii < vsList.size(); ii++)
		{
			if (vsList[ii] == sActive) { index = ii; }
			mapMIDIndex.emplace(vsList[ii], ii);
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
	int index = varWJP.size();

	auto wjpUnique = make_unique<WJPANEL>();
	varWJP.push_back(wjpUnique.get());
	string temp = wjpUnique->id();
	mapVarIndex.emplace(temp, index);
	varWJP[index]->setTitle("Parameter");

	string sTitle = "[None selected]";
	vsDiff.insert(vsDiff.begin(), sTitle);
	varWJP[index]->setCB(0, vsDiff);

	function<void()> fnDiffTitle = bind(&WJCONFIG::diffTitleChanged, this, temp);
	varWJP[index]->cbTitle->changed().connect(fnDiffTitle);

	varWJP[index]->highlight(0);
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

	wjpCategory->unhighlight(0);
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

	vector<string> prompt(3);
	prompt[0] = "";
	vector<string> vsCata = getDemo();
	if (vsCata.size() < 1) { return; }
	else if (vsCata.size() == 1)
	{
		size_t pos1 = vsCata[0].find('@');
		prompt[1] = vsCata[0].substr(0, pos1);
		prompt[2] = vsCata[0].substr(pos1 + 1);
		vector<vector<string>> vvsDummy;
		setPrompt(prompt, vvsDummy);
		pullSignal_.emit(4);  // Pull variable.
	}
	else
	{
		prompt.resize(1);
		vector<vector<string>> vvsCata = getDemoSplit(vsCata);
		setPrompt(prompt, vvsCata);
		pullSignal_.emit(1);  // Pull differentiator.
	}
}
void WJCONFIG::diffTitleChanged(string id)
{
	int index = mapVarIndex.at(id);
	Wt::WString wsTemp = varWJP[index]->cbTitle->currentText();
	if (wsTemp == wsNoneSel)
	{
		varWJP[index]->highlight(0);
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
void WJCONFIG::getPrompt(vector<string>& vsP)
{
	lock_guard<mutex> lg(m_prompt);
	vsP = vsPrompt;
}
void WJCONFIG::getPrompt(vector<string>& vsP, vector<vector<string>>& vvsP)
{
	lock_guard<mutex> lg(m_prompt);
	vsP = vsPrompt;
	vvsP = vvsPrompt;
}
vector<Wt::WString> WJCONFIG::getTextLegend()
{
	// Read all the CB panels, and return a list of the displayed options. 
	vector<Wt::WString> vwsLegend(4);
	vwsLegend[0] = wjpYear->title() + ": " + wjpYear->cbTitle->currentText();
	vwsLegend[1] = wjpCategory->title() + ": " + wjpCategory->cbTitle->currentText();
	vwsLegend[2] = wjpTopicCol->title() + ": " + wjpTopicCol->cbTitle->currentText();
	vwsLegend[3] = wjpTopicRow->title() + ": " + wjpTopicRow->cbTitle->currentText();

	Wt::WString wsTemp;
	if (wjpDemo != nullptr)
	{
		wsTemp = wjpDemo->title() + ": " + wjpDemo->cbTitle->currentText();
		vwsLegend.push_back(wsTemp);
	}

	for (int ii = 0; ii < varWJP.size(); ii++)
	{
		wsTemp = varWJP[ii]->title() + ": " + varWJP[ii]->cbTitle->currentText();
		vwsLegend.push_back(wsTemp);
	}

	return vwsLegend;  // THIS IS MISSING ORDERED-MIDs !
}
vector<vector<string>> WJCONFIG::getVariable()
{
	// Checks the active state of each "variable" panel and returns each title and MID.
	int numVar = varWJP.size();
	vector<vector<string>> vvsVariable(numVar);
	string sTitle, sMID;
	Wt::WString wsTemp;
	for (int ii = 0; ii < numVar; ii++)
	{
		wsTemp = varWJP[ii]->cbTitle->currentText();
		sTitle = wsTemp.toUTF8();
		wsTemp = varWJP[ii]->cbMID->currentText();
		sMID = wsTemp.toUTF8();
		if (sMID == "") { sMID = "*"; }
		vvsVariable[ii] = { sTitle, sMID };
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
	unique_ptr<Wt::WPanel> year = makePanelYear(vsYear);
	layout->addWidget(move(year));
	unique_ptr<Wt::WPanel> category = makePanelCategory();
	layout->addWidget(move(category));
	unique_ptr<Wt::WPanel> topicCol = makePanelTopicCol();
	layout->addWidget(move(topicCol));
	unique_ptr<Wt::WPanel> topicRow = makePanelTopicRow();
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
	
}
unique_ptr<Wt::WPanel> WJCONFIG::makePanelCategory()
{
	auto category = make_unique<WJPANEL>();
	category->setTitle("Topical Category");
	wjpCategory = category.get();
	allWJP.push_back(wjpCategory);
	wjpCategory->setHidden(1);
	return category;
}
unique_ptr<Wt::WPanel> WJCONFIG::makePanelTopicCol()
{
	auto topicCol = make_unique<WJPANEL>();
	topicCol->setTitle("Table Column Headers");
	wjpTopicCol = topicCol.get();
	allWJP.push_back(wjpTopicCol);
	wjpTopicCol->setHidden(1);
	return topicCol;
}
unique_ptr<Wt::WPanel> WJCONFIG::makePanelTopicRow()
{
	auto topicRow = make_unique<WJPANEL>();
	topicRow->setTitle("Table Row Headers");
	wjpTopicRow = topicRow.get();
	allWJP.push_back(wjpTopicRow);
	wjpTopicRow->setHidden(1);
	return topicRow;
}
unique_ptr<Wt::WPanel> WJCONFIG::makePanelYear(vector<string> vsYear)
{
	auto year = make_unique<WJPANEL>();
	year->setTitle("Census Year");
	wjpYear = year.get();
	allWJP.push_back(wjpYear);
	wjpYear->cbTitle->changed().connect(this, std::bind(&WJCONFIG::yearChanged, this));
	wjpYear->setHidden(0);
	wjpYear->cbTitle->setEnabled(1);
	return year;
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
		varWJP[ii]->clear();
		varWJP.pop_back();
	}

	// Remove the demographic widgets.
	if (wjpDemo != nullptr && plus < 1)
	{
		wjpDemo->clear();
		wjpDemo = nullptr;
		vvsDemographic.clear();
	}	
}
void WJCONFIG::setPrompt(vector<string>& vsP)
{
	lock_guard<mutex> lg(m_prompt);
	vsPrompt = vsP;
	vvsPrompt.clear();
}
void WJCONFIG::setPrompt(vector<string>& vsP, vector<vector<string>>& vvsP)
{
	lock_guard<mutex> lg(m_prompt);
	vsPrompt = vsP;
	vvsPrompt = vvsP;
}
void WJCONFIG::topicSelChanged()
{
	// Update the GUI using the current CB status.
	int iRow = wjpTopicRow->cbMID->currentIndex();
	int iCol = wjpTopicCol->cbMID->currentIndex();
	tableSignal_.emit(iRow, iCol);
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
