#include "wjconfig.h"

void WJCOMBO::highlight(Wt::WCssDecorationStyle& wcssTitle, Wt::WCssDecorationStyle& wcssMID)
{
	// The comboboxes have their borders become dotted or solid.
	if (wcbTitle != nullptr) { wcbTitle->setDecorationStyle(wcssTitle); }
	if (wcbMID != nullptr) { wcbMID->setDecorationStyle(wcssMID); }
}
void WJCOMBO::init()
{
	auto layout = make_unique<Wt::WVBoxLayout>();
	wcbTitle = layout->addWidget(make_unique<Wt::WComboBox>());
	wcbMID = layout->addWidget(make_unique<Wt::WComboBox>());
	wpbDialog = layout->addWidget(make_unique<Wt::WPushButton>());

	wcbMID->setHidden(1);
	wpbDialog->setHidden(1);

	this->setLayout(move(layout));
}
void WJCOMBO::setMID(vector<string>& vsList)
{
	Wt::WString wsTemp;
	wcbMID->clear();
	mapMIDIndex.clear();
	for (int ii = 0; ii < vsList.size(); ii++)
	{
		mapMIDIndex.emplace(vsList[ii], ii);
		wsTemp = Wt::WString::fromUTF8(vsList[ii]);
		wcbMID->addItem(wsTemp);
	}
}
void WJCOMBO::setMID(vector<string>& vsList, string sActive)
{
	Wt::WString wsTemp;
	wcbMID->clear();
	mapMIDIndex.clear();
	int index = 0;
	for (int ii = 0; ii < vsList.size(); ii++)
	{
		if (vsList[ii] == sActive) { index = ii; }
		mapMIDIndex.emplace(vsList[ii], ii);
		wsTemp = Wt::WString::fromUTF8(vsList[ii]);
		wcbMID->addItem(wsTemp);
	}
	wcbMID->setCurrentIndex(index);
}
void WJCOMBO::setTitle(vector<string>& vsList)
{
	Wt::WString wsTemp;
	wcbTitle->clear();
	mapTitleIndex.clear();
	for (int ii = 0; ii < vsList.size(); ii++)
	{
		mapTitleIndex.emplace(vsList[ii], ii);
		wsTemp = Wt::WString::fromUTF8(vsList[ii]);
		wcbTitle->addItem(wsTemp);
	}
}
void WJCOMBO::setTitle(vector<string>& vsList, string sActive)
{
	Wt::WString wsTemp;
	wcbTitle->clear();
	mapTitleIndex.clear();
	int index = 0;
	for (int ii = 0; ii < vsList.size(); ii++)
	{
		if (vsList[ii] == sActive) { index = ii; }
		mapTitleIndex.emplace(vsList[ii], ii);
		wsTemp = Wt::WString::fromUTF8(vsList[ii]);
		wcbTitle->addItem(wsTemp);
	}
	wcbTitle->setCurrentIndex(index);
}

void WJCONFIG::addDemographic(vector<vector<string>>& vvsDemo)
{
	// Create a "Demographic" panel from which the user must choose an option,
	// in order to reduce the number of potential catalogues. vvsDemo has
	// form [forWhom index][forWhom, sYear@sCata0, sYear@sCata1, ...].
	auto demoPanelUnique = make_unique<Wt::WPanel>();
	demoPanelUnique->setTitle("Demographic Group");
	demoPanelUnique->setMaximumSize(wlAuto, 150.0);
	wpDemo = demoPanelUnique.get();
	highlight(wpDemo, 1);

	auto demoUnique = make_unique<WJCOMBO>();
	wjcDemo = demoUnique.get();

	vector<string> vsDemo = { "[None selected]" };
	for (int ii = 0; ii < vvsDemo.size(); ii++)
	{
		vsDemo.push_back(vvsDemo[ii][0]);
	}
	wjcDemo->setTitle(vsDemo);
	wjcDemo->highlight(wcssAttention, wcssDefault);

	wjcDemo->wcbTitle->changed().connect(this, &WJCONFIG::demographicChanged);

	demoPanelUnique->setCentralWidget(move(demoUnique));
	vLayout->addWidget(move(demoPanelUnique));
}
void WJCONFIG::addDifferentiator(vector<string> vsDiff)
{
	// Create a normal-looking Parameter panel, containing differentiating 
	// DIM titles to try and pinpoint a catalogue. 
	int index = varPanel.size();
	if (index != varWJC.size()) { jf.err("Panel/WJC size mismatch-wjconfig.addDifferentiator"); }

	auto varPanelUnique = make_unique<Wt::WPanel>();
	varPanel.push_back(varPanelUnique.get());
	string temp = varPanelUnique->id();
	mapVarIndex.emplace(temp, index);
	varPanel[index]->setTitle("Parameter");

	auto cbjDiff = make_unique<WJCOMBO>();
	varWJC.push_back(cbjDiff.get());
	temp = cbjDiff->id();
	mapVarIndex.emplace(temp, index);
	vector<string> vsTitle = { "[None selected]" };
	for (int ii = 0; ii < vsDiff.size(); ii++)
	{
		vsTitle.push_back(vsDiff[ii]);
	}
	varWJC[index]->setTitle(vsTitle);

	function<void()> fnDiffTitle = bind(&WJCONFIG::diffTitleChanged, this, temp);
	varWJC[index]->wcbTitle->changed().connect(fnDiffTitle);

	varPanelUnique->setCentralWidget(move(cbjDiff));
	layoutConfig->addWidget(move(varPanelUnique));

	if (cbjDemo != nullptr && cbjDemo->vCB.size() > 0)
	{
		cbjDemo->highlight(1);
	}
	widgetMobile();
}
void WJCONFIG::demographicChanged()
{
	// Depending on the number of surviving catalogues, launch a "variable" event.
	wjcTopicCol->wcbMID->clear();
	wjcTopicCol->wcbMID->setHidden(1);
	wjcTopicCol->wpbDialog->setHidden(1);

	wjcTopicRow->wcbMID->clear();
	wjcTopicRow->wcbMID->setHidden(1);
	wjcTopicRow->wpbDialog->setHidden(1);

	resetMap();
	resetTable();
	resetTree();
	resetVariables(1);
	Wt::WString wsTemp = cbjDemo->vCB[0]->currentText();
	if (wsTemp == wsNoneSel) { cbjDemo->highlight(1); }
	else { cbjDemo->highlight(0); }
	vector<string> vsCata = getDemo();
	if (vsCata.size() < 1) { return; }

	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(5);
	prompt[0] = app->sessionId();
	prompt[2] = activeCategory;
	prompt[3] = activeColTopic;
	prompt[4] = activeRowTopic;

	if (vsCata.size() == 1)
	{
		size_t pos1 = vsCata[0].find('@');
		prompt[1] = vsCata[0].substr(0, pos1);
		prompt.push_back(vsCata[0].substr(pos1 + 1));
		vector<vector<string>> vvsDummy;
		sRef.pullVariable(prompt, vvsDummy);
	}
	else
	{
		prompt.resize(1);
		sRef.pullDifferentiator(prompt, vsCata);
	}
}
void WJCONFIG::diffTitleChanged(string id)
{
	int index = mapVarIndex.at(id);
	WJCOMBO* cbjDiff = (WJCOMBO*)varPanel[index]->centralWidget();
	Wt::WString wTemp = cbjDiff->vCB[0]->currentText();
	if (wTemp == wsNoneSel)
	{
		vector<int> viEn = { 1, 1 };
		for (int ii = 1; ii < cbjDiff->vCB.size(); ii++)
		{
			viEn.push_back(0);
		}
		cbjDiff->setEnabled(viEn);
		return;
	}
	string sTitle = wTemp.toUTF8();

	vector<vector<string>> vvsFixed = getVariable();
	Wt::WApplication* app = Wt::WApplication::instance();
	vector<string> prompt(vvsFixed.size() + 1);
	prompt[0] = app->sessionId();
	for (int ii = 0; ii < vvsFixed.size(); ii++)
	{
		prompt[ii + 1] = vvsFixed[ii][0];
	}
	vector<string> vsCata = getDemo();
	sRef.pullDifferentiator(prompt, vsCata);
}
void WJCONFIG::highlight(Wt::WPanel*& wpHL, bool HL)
{
	// The panel's title bar gains a "highlighted" background colour.
	if (HL) { wpHL->setDecorationStyle(wcssHighlighted); }
	else { wpHL->setDecorationStyle(wcssDefault); }
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

	// Insert the four necessary panels, each with a WJCOMBO.
	auto layout = make_unique<Wt::WVBoxLayout>();
	vLayout = layout.get();
	unique_ptr<Wt::WPanel> year = makePanelYear(vsYear);
	layout->addWidget(move(year));
	unique_ptr<Wt::WPanel> category = makePanelCategory();
	layout->addWidget(move(category));
	unique_ptr<Wt::WPanel> topicCol = makePanelTopicCol();
	layout->addWidget(move(topicCol));
	unique_ptr<Wt::WPanel> topicRow = makePanelTopicRow();
	layout->addWidget(move(topicRow));

	this->setLayout(move(layout));
}
unique_ptr<Wt::WPanel> WJCONFIG::makePanelCategory()
{
	auto category = make_unique<Wt::WPanel>();
	category->setTitle("Topical Category");
	category->setMaximumSize(wlAuto, 150.0);
	wpCategory = category.get();
	allPanel.push_back(wpCategory);
	auto wjc = make_unique<WJCOMBO>();
	wjcCategory = wjc.get();
	allWJC.push_back(wjcCategory);
	category->setCentralWidget(move(wjc));
	wpCategory->setHidden(1);
	return category;
}
unique_ptr<Wt::WPanel> WJCONFIG::makePanelTopicCol()
{
	auto topicCol = make_unique<Wt::WPanel>();
	topicCol->setTitle("Table Column Headers");
	wpTopicCol = topicCol.get();
	allPanel.push_back(wpTopicCol);
	auto wjc = make_unique<WJCOMBO>();
	wjcTopicCol = wjc.get();
	allWJC.push_back(wjcTopicCol);
	topicCol->setCentralWidget(move(wjc));
	wpTopicCol->setHidden(1);
	return topicCol;
}
unique_ptr<Wt::WPanel> WJCONFIG::makePanelTopicRow()
{
	auto topicRow = make_unique<Wt::WPanel>();
	topicRow->setTitle("Table Row Header");
	wpTopicRow = topicRow.get();
	allPanel.push_back(wpTopicRow);
	auto wjc = make_unique<WJCOMBO>();
	wjcTopicRow = wjc.get();
	allWJC.push_back(wjcTopicRow);
	topicRow->setCentralWidget(move(wjc));
	wpTopicRow->setHidden(1);
	return topicRow;
}
unique_ptr<Wt::WPanel> WJCONFIG::makePanelYear(vector<string> vsYear)
{
	auto year = make_unique<Wt::WPanel>("Census Year");
	wpYear = year.get();
	allPanel.push_back(wpYear);
	year->setMaximumSize(wlAuto, 150.0);
	auto wjc = make_unique<WJCOMBO>();
	wjc->setTitle(vsYear);
	wjcYear = wjc.get();
	allWJC.push_back(wjcYear);
	wjcYear->wcbTitle->changed().connect(this, std::bind(&WJCONFIG::yearChanged, this));
	wjcYear->setHidden(0);
	wjcYear->wcbTitle->setEnabled(1);
	year->setCentralWidget(move(wjc));
	return year;
}
