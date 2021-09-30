#include "wjconfig.h"

void WJPANEL::clear()
{
	mapFilterMID.clear();
	mapIndexTitle.clear();
	cbTitle->clear();
	jtMID.clear();
	textMID->setText("");
	viFilter.clear();
	boxMID->setHidden(1);
}
void WJPANEL::dialogFilter()
{
	// Permit only one dialog window at a time.
	if (wdTree != nullptr)
	{
		Wt::WContainerWidget* cw = wdTree->contents();
		Wt::WTree* wt = (Wt::WTree*)cw->widget(0);
		wt->itemSelectionChanged().emit();
		return;
	}
	auto wDialog = make_unique<Wt::WDialog>("Choose a header item to display exclusively with its interior items");
	wDialog->setClosable(1);

	string sRoot = jtMID.getRootName();
	Wt::WString wsTemp = Wt::WString::fromUTF8(sRoot);
	auto treeRootUnique = make_unique<Wt::WTreeNode>(wsTemp);
	treeRootUnique->setLoadPolicy(Wt::ContentLoading::Eager);
	treeRootUnique->decorationStyle().font().setSize(Wt::FontSize::Large);
	auto treeRoot = treeRootUnique.get();

	jtMID.setExpandGeneration(16);  // Expand generations without displaying more items than this.
	treeNodeSel = nullptr;
	if (viFilter.size() > 0)
	{
		string sFilterParent = vsMID[viFilter[0]];
		populateTree(jtMID, treeRoot, sFilterParent);
		if (treeNodeSel == nullptr) { jf.err("Failed to locate previous tree node-wjpanel.dialogFilter"); }
	}
	else { populateTree(jtMID, treeRoot); }
	treeRootUnique->setNodeVisible(0);

	auto tree = make_unique<Wt::WTree>();
	tree->setSelectionMode(Wt::SelectionMode::Single);
	tree->setTreeRoot(move(treeRootUnique));
	if (treeNodeSel != nullptr) { tree->select(treeNodeSel); }

	tree->itemSelectionChanged().connect(this, &WJPANEL::dialogFilterEnd);
	treeDialog = tree.get();
	auto wBox = wDialog->contents();
	wBox->setMaximumSize(wlAuto, wlAuto);
	wBox->setOverflow(Wt::Overflow::Auto);
	wBox->addWidget(move(tree));
	wDialog->show();
	swap(wDialog, wdTree);
}
void WJPANEL::dialogFilterEnd()
{
	auto selSet = treeDialog->selectedNodes();
	if (selSet.size() < 1) { return; }
	if (selSet.size() > 1) { jf.err("Invalid selection-wjpanel.dialogFilterEnd"); }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	string sFilterParent = wTemp.toUTF8();
	wdTree->accept();
	unique_ptr<Wt::WDialog> wdDummy = nullptr;
	swap(wdTree, wdDummy);

	vector<string> vsChildren;
	jtMID.listChildrenAll(sFilterParent, vsChildren);
	m_config.lock();
	viFilter.resize(vsChildren.size() + 1);
	viFilter[0] = jtMID.getIndex(sFilterParent) - 1;
	for (int ii = 0; ii < vsChildren.size(); ii++)
	{
		viFilter[ii + 1] = jtMID.getIndex(vsChildren[ii]) - 1;
	}
	setFilter(viFilter);
	m_config.unlock();

	filterSignal_.emit();
	jf.timerStart();
}
void WJPANEL::dialogMID()
{
	// Close all other open dialogs. They keep their existing item selections.
	string sObjectName = objectName();
	dialogOpenSignal_.emit(sObjectName);

	// Generate a tree-structured selection menu for the user to select a MID.
	auto wDialog = make_unique<Wt::WDialog>();
	wDialog->setTitleBarEnabled(0);
	wDialog->setModal(0);
	wDialog->setMovable(0);
	wDialog->setClosable(0);
	wDialog->positionAt(this, Wt::Orientation::Horizontal);

	jtMID.setExpandGeneration(16);  // Expand generations without displaying more items than this.
	string sRoot = jtMID.getRootName();
	Wt::WString wsTemp = Wt::WString::fromUTF8(sRoot);
	auto treeRootUnique = make_unique<Wt::WTreeNode>(wsTemp);
	treeRootUnique->setLoadPolicy(Wt::ContentLoading::Eager);
	treeRootUnique->decorationStyle().font().setSize(Wt::FontSize::Large);

	Wt::WTreeNode* treeRoot = nullptr;
	if (viFilter.size() < 1) { treeRoot = treeRootUnique.get(); }
	else
	{
		sRoot = jtMID.treePL[viFilter[0] + 1];
		wsTemp = Wt::WString::fromUTF8(sRoot);
		auto childUnique = make_unique<Wt::WTreeNode>(wsTemp);
		treeRoot = treeRootUnique->addChildNode(move(childUnique));
	}
	treeNodeSel = nullptr;
	if (selMID.size() > 0)
	{
		populateTree(jtMID, treeRoot, selMID);
		if (treeNodeSel == nullptr) { jf.err("Failed to locate previous tree node-wjpanel.dialogMID"); }
	}
	else { populateTree(jtMID, treeRoot); }
	treeRootUnique->setNodeVisible(0);

	auto tree = make_unique<Wt::WTree>();
	tree->setSelectionMode(Wt::SelectionMode::Single);
	tree->setTreeRoot(move(treeRootUnique));
	if (treeNodeSel != nullptr) { tree->select(treeNodeSel); }
	tree->itemSelectionChanged().connect(this, &WJPANEL::panelClicked);
	tree->itemSelectionChanged().connect(this, &WJPANEL::dialogMIDEnd);
	treeDialog = tree.get();

	double dGuess = (double)jtMID.maxLength * 10.0;
	tree->setMinimumSize(dGuess, wlAuto);

	auto wBox = wDialog->contents();
	wBox->setMaximumSize(wlAuto, wlAuto);
	wBox->setOverflow(Wt::Overflow::Auto);
	wBox->addWidget(move(tree));

	swap(wdTree, wDialog);
	wdTree->show();
	wdTree->raiseToFront();
}
void WJPANEL::dialogMIDEnd()
{
	if (wdTree == nullptr) { return; }
	auto selSet = treeDialog->selectedNodes();
	if (selSet.size() < 1) { return; }
	if (selSet.size() > 1) { jf.err("Invalid selection-wjpanel.dialogMIDEnd"); }
	auto selIt = selSet.begin();
	auto selNode = *selIt;
	auto wTemp = selNode->label()->text();
	selMID = wTemp.toUTF8();
	setTextMID(selMID);
	wdTree->accept();
	unique_ptr<Wt::WDialog> wdDummy = nullptr;
	swap(wdTree, wdDummy);
	stackedPB->setCurrentIndex(0);

	string sPanel = objectName();
	size_t pos1 = sPanel.find("Variable");
	if (pos1 < sPanel.size()) { varSignal_.emit(); }  // Member of varWJP
	else { topicSignal_.emit(); }  // Member of basicWJP
	jf.timerStart();
}
void WJPANEL::dialogMIDToggle(int stackedIndex)
{
	// stackedIndex is the triggering button.
	switch (stackedIndex)
	{
	case 0:
	{
		stackedPB->setCurrentIndex(1);
		dialogMID();
		break;
	}
	case 1:
	{
		stackedPB->setCurrentIndex(0);
		dialogOpenSignal_.emit("-1");
		break;
	}
	}
}
void WJPANEL::highlight(int widgetIndex)
{
	// The chosen widget has its borders become dotted, and the title gains a coloured background.
	int numItem;
	this->setHidden(0);
	Wt::WContainerWidget* cwTitle = titleBarWidget();
	Wt::WCssDecorationStyle& stylePanel = cwTitle->decorationStyle();
	stylePanel.setBackgroundColor(wcSelectedWeak);
	switch (widgetIndex)
	{
	case 0:
		numItem = cbTitle->count();
		if (numItem > 0)
		{
			Wt::WCssDecorationStyle& style = cbTitle->decorationStyle();
			style.setBorder(Wt::WBorder(Wt::BorderStyle::Dotted));
			cbTitle->setHidden(0);
		}
		break;
	case 1:
		numItem = jtMID.getCount();
		if (numItem > 0)
		{
			Wt::WCssDecorationStyle& style = textMID->decorationStyle();
			style.setBorder(Wt::WBorder(Wt::BorderStyle::Dotted));
			textMID->setHidden(0);
		}
		break;
	}
}
int WJPANEL::getIndexMID(int mode)
{
	// Returns the (header/CB) index for the current MID text.
	// mode  0 = true index, 1 = filtered index.
	int index = jtMID.getIndex(selMID) - 1;
	if (mode == 1 && viFilter.size() > 0)
	{
		index = mapFilterMID.at(index);
	}
	return index;
}
string WJPANEL::getTextLegend()
{
	string textLegend = getTextLegend(0);
	return textLegend;
}
string WJPANEL::getTextLegend(int mode)
{
	// Return a string representing this panel's detailed parameter listing.
	// Modes:  0 = standard, 1 = odd-numbered entries are HTML-italicized
	string sLegend, sTitle, temp;
	Wt::WString wsTemp;
	if (boxMID->isHidden())
	{
		wsTemp = title();
		sLegend = wsTemp.toUTF8() + " | ";
		wsTemp = cbTitle->currentText();
		switch (mode)
		{
		case 0:
			sLegend += wsTemp.toUTF8();
			break;
		case 1:
			sLegend += "<i>" + wsTemp.toUTF8() + "</i>";
			break;
		}		
		return sLegend;
	}
	
	size_t pos1;
	wsTemp = cbTitle->currentText();
	sTitle = wsTemp.toUTF8();
	sLegend = sTitle;
	int path, index = jtMID.getIndex(selMID);  // JTREE index.
	vector<int> viAncestry = jtMID.treeSTanc[index];
	viAncestry.push_back(index);
	for (int ii = 0; ii < viAncestry.size(); ii++)
	{
		temp = jtMID.treePL[viAncestry[ii]];
		if (temp == "") { continue; }
		while (temp[0] == '+') { temp.erase(temp.begin()); }
		pos1 = temp.find("Total -");

		if (mode == 1 && ii % 2 == 1) { path = 1; }
		else { path = 0; }
		switch (path)
		{
		case 0:
		{
			if (pos1 > temp.size()) { sLegend += " | " + temp; }
			else
			{
				pos1 = temp.find(sTitle);
				if (pos1 > temp.size()) { sLegend += " | " + temp; }
				else { sLegend += " | Total"; }
			}
			break;
		}
		case 1:
		{
			if (pos1 > temp.size()) { sLegend += " | <i>" + temp + "</i>"; }
			else
			{
				pos1 = temp.find(sTitle);
				if (pos1 > temp.size()) { sLegend += " | <i>" + temp + "</i>"; }
				else { sLegend += " | <i>Total</i>"; }
			}
			break;
		}
		}

	}
	return sLegend;
}
void WJPANEL::init()
{
	wcBorder.setRgb(204, 204, 204);
	wcGrey.setRgb(210, 210, 210);
	wcOffWhite.setRgb(245, 245, 245);
	wcSelectedWeak.setRgb(200, 200, 255);
	wcWhite.setRgb(255, 255, 255);
	wbDefaultCB = Wt::WBorder(Wt::BorderStyle::Solid, 1.0, wcBorder);

	auto wBox = make_unique<Wt::WContainerWidget>();
	auto layout = make_unique<Wt::WVBoxLayout>();
	layout->setContentsMargins(4.0, 4.0, 4.0, 4.0);
	cbTitle = layout->addWidget(make_unique<Wt::WComboBox>());

	auto boxMIDUnique = make_unique<Wt::WContainerWidget>();
	boxMIDUnique->setPadding(0.0);
	auto boxTextUnique = make_unique<Wt::WContainerWidget>();
	auto textUnique = make_unique<Wt::WText>();
	boxTextUnique->setPadding(6.0);
	textMID = boxTextUnique->addWidget(move(textUnique));


	auto stacked = make_unique<Wt::WStackedWidget>();
	auto animation = Wt::WAnimation(Wt::AnimationEffect::Fade);
	stacked->setTransitionAnimation(animation, 1);
	pbMIDclosed = stacked->addNew<Wt::WPushButton>();
	pbMIDclosed->setMinimumSize(40.0, 40.0);
	pbMIDclosed->setMaximumSize(40.0, 40.0);
	pbMIDopened = stacked->addNew<Wt::WPushButton>();
	pbMIDopened->setMinimumSize(40.0, 40.0);
	pbMIDopened->setMaximumSize(40.0, 40.0);
	stacked->setCurrentIndex(0);

	auto hLayout = make_unique<Wt::WHBoxLayout>();
	hLayout->addWidget(move(boxTextUnique), 1);
	stackedPB = hLayout->addWidget(move(stacked), 0, Wt::AlignmentFlag::Middle);
	boxMIDUnique->setLayout(move(hLayout));

	/*
	auto pbUnique = make_unique<Wt::WPushButton>();
	pbUnique->setMinimumSize(40.0, 40.0);
	pbUnique->setMaximumSize(40.0, 40.0);
	auto hLayout = make_unique<Wt::WHBoxLayout>();
	hLayout->addWidget(move(boxTextUnique), 1);
	pbMID = hLayout->addWidget(move(pbUnique), 0, Wt::AlignmentFlag::Middle);
	boxMIDUnique->setLayout(move(hLayout));
	*/

	boxMID = layout->addWidget(move(boxMIDUnique));
	wBox->setLayout(move(layout));
	setCentralWidget(move(wBox));

	cbTitle->clicked().connect(this, &WJPANEL::panelClicked);
	
	boxMID->setHidden(1);
}
void WJPANEL::initStackedPB(Wt::WLink wlClosed, Wt::WLink wlOpened)
{
	// Set the button background images and connect its signals.
	Wt::WFlags<Wt::Orientation> flags;
	Wt::WCssDecorationStyle& styleClosed = pbMIDclosed->decorationStyle();
	styleClosed.setBackgroundImage(wlClosed, flags, Wt::Side::CenterX | Wt::Side::CenterY);
	pbMIDclosed->clicked().connect(this, std::bind(&WJPANEL::dialogMIDToggle, this, 0));

	Wt::WCssDecorationStyle& styleOpened = pbMIDopened->decorationStyle();
	styleOpened.setBackgroundImage(wlOpened, flags, Wt::Side::CenterX | Wt::Side::CenterY);
	pbMIDopened->clicked().connect(this, std::bind(&WJPANEL::dialogMIDToggle, this, 1));
}
void WJPANEL::populateTree(JTREE& jt, Wt::WTreeNode*& node)
{
	// Recursive function that takes an existing node and makes its children.
	string sName = "-1";
	populateTree(jt, node, sName);
}
void WJPANEL::populateTree(JTREE& jt, Wt::WTreeNode*& node, string sName)
{
	// Recursive function that takes an existing node and makes its children.
	// If sName is specified, then a pointer to the node with that name will be saved.
	vector<string> vsChildren;
	Wt::WString wsTemp = node->label()->text();
	string sNode = wsTemp.toUTF8();
	if (sNode == sName) { treeNodeSel = node; }
	if (jt.isExpanded(sNode)) { node->expand(); }

	jt.listChildren(sNode, vsChildren);
	for (int ii = 0; ii < vsChildren.size(); ii++)
	{
		wsTemp = Wt::WString::fromUTF8(vsChildren[ii]);
		auto childUnique = make_unique<Wt::WTreeNode>(wsTemp);
		auto child = childUnique.get();
		if (jt.fontRootSize >= 0)  // Apply decreasing font sizes.
		{
			child->decorationStyle().font().setSize(Wt::FontSize::Smaller);
		}
		populateTree(jt, child, sName);
		node->addChildNode(move(childUnique));
	}
}
void WJPANEL::resetMapIndex()
{
	mapIndexTitle.clear();
	for (int ii = 0; ii < vsTitle.size(); ii++)
	{
		mapIndexTitle.emplace(vsTitle[ii], ii);
	}
}
void WJPANEL::setCB(vector<string>& vsList)
{
	int index = 0;
	setCB(vsList, index);
}
void WJPANEL::setCB(vector<string>& vsList, int iActive)
{
	Wt::WString wsTemp;
	cbTitle->clear();
	vsTitle = vsList;
	resetMapIndex();
	for (int ii = 0; ii < vsTitle.size(); ii++)
	{
		wsTemp = Wt::WString::fromUTF8(vsTitle[ii]);
		cbTitle->addItem(wsTemp);
	}
	cbTitle->setCurrentIndex(iActive);
	cbTitle->setHidden(0);
}
void WJPANEL::setCB(vector<string>& vsList, string sActive)
{
	Wt::WString wsTemp;
	cbTitle->clear();
	vsTitle = vsList;
	resetMapIndex();
	int index = 0;
	for (int ii = 0; ii < vsTitle.size(); ii++)
	{
		if (vsTitle[ii] == sActive) { index = ii; }
		wsTemp = Wt::WString::fromUTF8(vsTitle[ii]);
		cbTitle->addItem(wsTemp);
	}
	cbTitle->setCurrentIndex(index);
	cbTitle->setHidden(0);
}
void WJPANEL::setFilter(vector<int>& filter)
{
	viFilter = filter;
	mapFilterMID.clear();
	for (int ii = 0; ii < viFilter.size(); ii++)
	{
		mapFilterMID.emplace(viFilter[ii], ii);
	}

	// If the previous MID item is outside the new filter, default to filter index zero.
	int index = jtMID.getIndex(selMID) - 1;
	if (!mapFilterMID.count(index))
	{
		setIndexMID(viFilter[0]);
	}
}
void WJPANEL::setIndexMID(int indexMID)
{
	selMID = jtMID.treePL[indexMID + 1];
	setTextMID(selMID);
}
void WJPANEL::setTextMID(string sMID)
{
	while (sMID[0] == '+') { sMID.erase(sMID.begin()); }
	Wt::WString wsTemp = Wt::WString::fromUTF8(sMID);
	textMID->setText(wsTemp);
}
void WJPANEL::setTree(vector<string>& vsList)
{
	// Populate the panel's JTREE object, and its textMID widget. 
	int indent;
	string sParent;
	vector<int> indentHistory;
	jtMID.init("", -1);
	for (int ii = 0; ii < vsList.size(); ii++)
	{
		indent = 0;
		while (vsList[ii][indent] == '+') { indent++; }

		if (indentHistory.size() <= indent) { indentHistory.push_back(ii); }
		else
		{
			indentHistory[indent] = ii;
			indentHistory.resize(indent + 1);
		}

		if (indent == 0) { jtMID.addChild(vsList[ii], -2, -1); }
		else
		{
			sParent = vsList[indentHistory[indentHistory.size() - 2]];
			jtMID.addChild(vsList[ii], -2, sParent);
		}
	}

	// Display the first MID item as a default setting.
	selMID = vsList[0];
	setTextMID(selMID);

	boxMID->setHidden(0);
}
void WJPANEL::toggleMobile(bool mobile)
{
	if (mobile)
	{
		Wt::WPanel::decorationStyle().font().setSize(Wt::FontSize::XXLarge);
		cbTitle->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
		if (!textMID->isHidden()) { textMID->decorationStyle().font().setSize(Wt::FontSize::XXLarge); }
		if (!textMID->isHidden())
		{ 
			Wt::WPanel::setMinimumSize(wlAuto, 275.0);
			textMID->setMinimumSize(wlAuto, 75.0);
			cbTitle->setMinimumSize(wlAuto, 75.0);
		}
		else if (!cbTitle->isHidden()) 
		{ 
			Wt::WPanel::setMinimumSize(wlAuto, 200.0); 
			cbTitle->setMinimumSize(wlAuto, 75.0);
		}
	}
	else
	{
		Wt::WPanel::decorationStyle().font().setSize(Wt::FontSize::Medium);
		cbTitle->decorationStyle().font().setSize(Wt::FontSize::Medium);
		if (!textMID->isHidden()) { textMID->decorationStyle().font().setSize(Wt::FontSize::Medium); }
		if (!textMID->isHidden())
		{ 
			Wt::WPanel::setMinimumSize(wlAuto, wlAuto); 
			textMID->setMinimumSize(wlAuto, wlAuto);
			cbTitle->setMinimumSize(wlAuto, wlAuto);
		}
		else if (!cbTitle->isHidden()) 
		{ 
			Wt::WPanel::setMinimumSize(wlAuto, wlAuto);
			cbTitle->setMinimumSize(wlAuto, wlAuto);
		}
	}
}
void WJPANEL::unhighlight(int widgetIndex)
{
	// The chosen widget has its borders become solid, and the title gains a white background.
	int numItem;
	Wt::WContainerWidget* cwTitle = titleBarWidget();
	Wt::WCssDecorationStyle& stylePanel = cwTitle->decorationStyle();
	stylePanel.setBackgroundColor(wcOffWhite);
	switch (widgetIndex)
	{
	case 0:
		numItem = cbTitle->count();
		if (numItem > 0)
		{
			Wt::WCssDecorationStyle& style = cbTitle->decorationStyle();
			style.setBorder(wbDefaultCB);
		}
		break;
	case 1:
		numItem = jtMID.getCount();
		if (numItem > 0)
		{
			Wt::WCssDecorationStyle& style = textMID->decorationStyle();
			style.setBorder(wbDefaultCB);
		}
		break;
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
	if (vvsDemo.size() == 1) { wjpDemo->setCB(vsDemo, 1); }
	else { wjpDemo->setCB(vsDemo); }
	wjpDemo->highlight(0);
	wjpDemo->cbTitle->changed().connect(this, &WJCONFIG::demographicChanged);

	int numItem = vLayout->count();
	vLayout->insertWidget(numItem - 1, move(demoPanelUnique));
	if (vvsDemo.size() == 1) 
	{ 
		wjpDemo->setHidden(1); 
		demographicChanged();
	}
	widgetMobile(mobile);
}
void WJCONFIG::addDifferentiator(vector<string> vsDiff)
{
	// Create a normal-looking Parameter panel, containing differentiating 
	// DIM titles to try and pinpoint a catalogue. 
	Wt::WString wsTemp;
	int index = diffWP.size();
	auto wpUnique = make_unique<Wt::WPanel>();
	diffWP.push_back(wpUnique.get());
	string sID = wpUnique->id();
	mapDiffIndex.emplace(sID, index);
	diffWP[index]->setTitle("Parameter");

	auto wBox = make_unique<Wt::WContainerWidget>();
	auto layout = make_unique<Wt::WVBoxLayout>();
	auto cbTitle = make_unique<Wt::WComboBox>();
	string sTitle = "[None selected]";
	vsDiff.insert(vsDiff.begin(), sTitle);
	for (int ii = 0; ii < vsDiff.size(); ii++)
	{
		wsTemp = Wt::WString::fromUTF8(vsDiff[ii]);
		cbTitle->addItem(wsTemp);
	}
	cbTitle->setCurrentIndex(0);

	function<void()> fnDiffTitle = bind(&WJCONFIG::diffChanged, this, sID);
	cbTitle->changed().connect(fnDiffTitle);

	layout->addWidget(move(cbTitle));
	wBox->setLayout(move(layout));
	diffWP[index]->setCentralWidget(move(wBox));
	highlightPanel(diffWP[index], 0);

	int numItem = vLayout->count();
	vLayout->insertWidget(numItem - 1, move(wpUnique));
	widgetMobile(mobile);
}
void WJCONFIG::addDifferentiator(vector<string> vsDiff, string sTitle)
{
	string sDefaultMID = "[None selected]";
	vsDiff.insert(vsDiff.begin(), sDefaultMID);

	// Try to add this list of MIDs to an existing Diff panel with selected DIM title "sTitle".
	Wt::WString wsTemp;
	Wt::WString wsTitle = Wt::WString::fromUTF8(sTitle);
	Wt::WContainerWidget* wBox = nullptr;
	Wt::WComboBox* wCB = nullptr;
	Wt::WVBoxLayout* layout = nullptr;
	Wt::WLayoutItem* layoutItem = nullptr;
	for (int ii = 0; ii < diffWP.size(); ii++)
	{
		wBox = (Wt::WContainerWidget*)diffWP[ii]->centralWidget();
		layout = (Wt::WVBoxLayout*)wBox->layout();
		layoutItem = (Wt::WLayoutItem*)layout->itemAt(0);
		wCB = (Wt::WComboBox*)layoutItem->widget();
		wsTemp = wCB->currentText();
		if (wsTemp == wsTitle)
		{
			auto cbMID = make_unique<Wt::WComboBox>();
			for (int jj = 0; jj < vsDiff.size(); jj++)
			{
				wsTemp = Wt::WString::fromUTF8(vsDiff[jj]);
				cbMID->addItem(wsTemp);
			}
			cbMID->setCurrentIndex(0);
			layout = (Wt::WVBoxLayout*)wBox->layout();
			layout->addWidget(move(cbMID));
			return;
		}
	}

	// If no existing Diff panel matches the given sTitle, then make a new one. 
	int index = diffWP.size();
	auto wpUnique = make_unique<Wt::WPanel>();
	diffWP.push_back(wpUnique.get());
	string sID = wpUnique->id();
	mapDiffIndex.emplace(sID, index);
	diffWP[index]->setTitle("Parameter");

	auto box = make_unique<Wt::WContainerWidget>();
	auto boxLayout = make_unique<Wt::WVBoxLayout>();
	auto cbTitle = make_unique<Wt::WComboBox>();
	auto cbMID = make_unique<Wt::WComboBox>();

	wsTemp = Wt::WString::fromUTF8(sTitle);
	cbTitle->addItem(wsTemp);
	cbTitle->setCurrentIndex(0);
	cbTitle->setEnabled(0);

	for (int ii = 0; ii < vsDiff.size(); ii++)
	{
		wsTemp = Wt::WString::fromUTF8(vsDiff[ii]);
		cbMID->addItem(wsTemp);
	}
	cbMID->setCurrentIndex(0);

	function<void()> fnDiffMID = bind(&WJCONFIG::diffChanged, this, sID);
	cbMID->changed().connect(fnDiffMID);

	boxLayout->addWidget(move(cbTitle));
	boxLayout->addWidget(move(cbMID));
	box->setLayout(move(boxLayout));
	wpUnique->setCentralWidget(move(box));

	int numItem = vLayout->count();
	vLayout->insertWidget(numItem - 1, move(wpUnique));

	highlightPanel(diffWP[index], 1);
	if (index > 0)
	{
		unhighlightPanel(diffWP[index - 1], 1);
		unhighlightPanel(diffWP[index - 1], 0);
	}
	widgetMobile(mobile);
}
void WJCONFIG::addVariable(string& sTitle, vector<string>& vsMID)
{
	// Creates a new WJPANEL for a single DIM (sTitle) and its list of MIDs.
	int index = varWJP.size();
	auto wjpUnique = makePanelVariable();
	varWJP.push_back(wjpUnique.get());
	string sObjectName = varWJP[index]->objectName();
	mapVarIndex.emplace(sObjectName, index);

	vector<string> vsTitle = { sTitle };
	varWJP[index]->setCB(vsTitle);
	function<void()> fnVarTitle = bind(&WJCONFIG::varTitleChanged, this, sObjectName);
	varWJP[index]->cbTitle->changed().connect(fnVarTitle);
	varWJP[index]->cbTitle->setEnabled(0);

	varWJP[index]->setTree(vsMID);
	varWJP[index]->varSignal().connect(this, &WJCONFIG::varMIDChanged);
	varWJP[index]->dialogOpenSignal().connect(this, std::bind(&WJCONFIG::dialogHighlander, this, std::placeholders::_1));

	int numItem = vLayout->count();
	vLayout->insertWidget(numItem - 1, move(wjpUnique));
}
void WJCONFIG::categoryChanged()
{
	// Populate the topic widgets with options for the user
	// to choose column and row topics. 
	resetSignal_.emit(3);  // Reset all tabs.
	dialogHighlander("-1");
	resetVariables();
	wjpTopicCol->clear();
	wjpTopicRow->clear();
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
	resetSignal_.emit(3);  // Reset all tabs.
	resetVariables(1);
	resetTopicMID();

	Wt::WString wsTemp = wjpDemo->cbTitle->currentText();
	if (wsTemp == wsNoneSel) { wjpDemo->highlight(0); }
	else { wjpDemo->unhighlight(0); }

	wsTemp = wjpYear->cbTitle->currentText();
	vector<string> prompt = { "" };  // sID
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
		string temp = "";
		vector<vector<string>> vvsCata = getDemoSplit(vsCata);
		vector<vector<string>> vvsDummy;
		setPrompt(temp, vvsCata, vvsDummy);
		pullSignal_.emit(1);  // Pull differentiator.
	}
}
void WJCONFIG::dialogHighlander(const string& sObjectName)
{
	// Close all open dialogs except that of sObjectName.
	string sName = wjpTopicCol->objectName();
	if (sName != sObjectName) { wjpTopicCol->dialogMIDEnd(); }
	sName = wjpTopicRow->objectName();
	if (sName != sObjectName) { wjpTopicRow->dialogMIDEnd(); }
	for (int ii = 0; ii < varWJP.size(); ii++)
	{
		sName = varWJP[ii]->objectName();
		if (sName != sObjectName) { varWJP[ii]->dialogMIDEnd(); }
	}
}
void WJCONFIG::diffChanged(string id)
{
	int index = mapDiffIndex.at(id);
	Wt::WContainerWidget* cwTemp = (Wt::WContainerWidget*)diffWP[index]->centralWidget();
	Wt::WVBoxLayout* layout = (Wt::WVBoxLayout*)cwTemp->layout();
	Wt::WLayoutItem* layoutItem = layout->itemAt(0);
	Wt::WComboBox* cbTemp = (Wt::WComboBox*)layoutItem->widget();
	Wt::WString wsTemp = cbTemp->currentText();
	if (wsTemp == wsNoneSel)
	{
		for (int ii = diffWP.size() - 1; ii >= 0; ii--)
		{
			vLayout->removeWidget(diffWP[ii]);
			diffWP.pop_back();
		}		
	}
	
	string sTitle = wsTemp.toUTF8();
	vector<vector<string>> vvsDiff = getVariable();
	string prompt = "";
	vector<vector<string>> vvsCata = getDemoSplit();
	setPrompt(prompt, vvsCata, vvsDiff);
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
				break;
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
	vviFilter[0] = wjpTopicRow->viFilter;
	vviFilter[1] = wjpTopicCol->viFilter;
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
void WJCONFIG::getPrompt(string& sP, vector<vector<string>>& vvsP1, vector<vector<string>>& vvsP2)
{
	lock_guard<mutex> lg(m_config);
	sP = sPrompt;
	vvsP1 = vvsCata;
	vvsP2 = vvsPrompt;
}
vector<string> WJCONFIG::getTextLegend()
{
	vector<string> vsTextLegend = getTextLegend(0);
	return vsTextLegend;
}
vector<string> WJCONFIG::getTextLegend(int mode)
{
	// Read all the CB panels, and return a list of the displayed options. 
	int numParam = basicWJP.size() + varWJP.size();
	if (wjpDemo != nullptr) { numParam++; }

	int index = 0;
	vector<string> vsLegend(numParam);
	for (int ii = 0; ii < basicWJP.size(); ii++)
	{
		vsLegend[index] = basicWJP[ii]->getTextLegend(mode);
		index++;
	}
	if (wjpDemo != nullptr)
	{
		vsLegend[index] = wjpDemo->getTextLegend(mode);
		index++;
	}
	for (int ii = 0; ii < varWJP.size(); ii++)
	{
		vsLegend[index] = varWJP[ii]->getTextLegend(mode);
		index++;
	}
	return vsLegend;
}
vector<vector<string>> WJCONFIG::getVariable()
{
	// Checks the active state of each "variable" panel and returns each title and MID.
	vector<vector<string>> vvsVariable;
	string sTitle, sMID;
	int numCB, numVar;
	Wt::WString wsTemp;
	Wt::WContainerWidget* cwTemp = nullptr;
	Wt::WVBoxLayout* layout = nullptr;
	Wt::WLayoutItem* layoutItem = nullptr;
	Wt::WComboBox* cbTemp = nullptr;
	if (diffWP.size() > 0)
	{
		numVar = diffWP.size();
		vvsVariable.resize(numVar);		
		for (int ii = 0; ii < numVar; ii++)
		{
			cwTemp = (Wt::WContainerWidget*)diffWP[ii]->centralWidget();
			layout = (Wt::WVBoxLayout*)cwTemp->layout();
			layoutItem = layout->itemAt(0);
			cbTemp = (Wt::WComboBox*)layoutItem->widget();
			wsTemp = cbTemp->currentText();
			sTitle = wsTemp.toUTF8();

			numCB = layout->count();
			if (numCB < 2) { sMID = "*"; }
			else
			{
				layoutItem = layout->itemAt(1);
				cbTemp = (Wt::WComboBox*)layoutItem->widget();
				wsTemp = cbTemp->currentText();
				sMID = wsTemp.toUTF8();
			}
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
			vvsVariable[ii] = { wsTemp.toUTF8(), varWJP[ii]->selMID };
		}
	}
	return vvsVariable;
}
void WJCONFIG::highlightPanel(Wt::WPanel*& wPanel, int widgetIndex)
{
	// This highlighting function is made for standard WPanel widgets (containing
	// WComboBoxes), rather than WJPANEL widgets. 
	int numItem;
	this->setHidden(0);
	Wt::WContainerWidget* cwTitle = wPanel->titleBarWidget();
	Wt::WCssDecorationStyle& stylePanel = cwTitle->decorationStyle();
	stylePanel.setBackgroundColor(wcSelectedWeak);

	Wt::WContainerWidget* wBox = (Wt::WContainerWidget*)wPanel->centralWidget();
	int numCB = wBox->count();
	if (widgetIndex >= numCB) { return; }
	Wt::WComboBox* wCB = (Wt::WComboBox*)wBox->widget(widgetIndex);
	Wt::WCssDecorationStyle& styleCB = wCB->decorationStyle();
	styleCB.setBorder(Wt::WBorder(Wt::BorderStyle::Dotted));
	wCB->setHidden(0);
}
void WJCONFIG::init(vector<string> vsYear)
{
	// Initialize colours. 
	wcBlack.setRgb(0, 0, 0);
	wcBorder.setRgb(204, 204, 204);
	wcOffWhite.setRgb(245, 245, 245);
	wcSelectedWeak.setRgb(200, 200, 255);
	wcSelectedStrong.setRgb(150, 150, 192);
	wcWhite.setRgb(255, 255, 255, 255);

	// Initialize borders. 
	wbDotted = Wt::WBorder(Wt::BorderStyle::Dotted);
	wbSolid = Wt::WBorder(Wt::BorderStyle::Solid);
	wbDefaultCB = Wt::WBorder(Wt::BorderStyle::Solid, 1.0, wcBorder);

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
	layout->addStretch(1);
	this->setLayout(move(layout));

	pbMobile->setEnabled(1);

	// Connect signals to functions. 
	string sID;
	wjpCategory->cbTitle->changed().connect(this, &WJCONFIG::categoryChanged);
	sID = wjpTopicCol->id();
	wjpTopicCol->cbTitle->changed().connect(this, std::bind(&WJCONFIG::topicTitleChanged, this, sID));
	sID = wjpTopicRow->id();
	wjpTopicRow->cbTitle->changed().connect(this, std::bind(&WJCONFIG::topicTitleChanged, this, sID));
	wjpTopicCol->topicSignal().connect(this, &WJCONFIG::topicSelChanged);
	wjpTopicRow->topicSignal().connect(this, &WJCONFIG::topicSelChanged);
	wjpTopicCol->dialogOpenSignal().connect(this, std::bind(&WJCONFIG::dialogHighlander, this, std::placeholders::_1));
	wjpTopicRow->dialogOpenSignal().connect(this, std::bind(&WJCONFIG::dialogHighlander, this, std::placeholders::_1));
}
unique_ptr<WJPANEL> WJCONFIG::makePanelCategory()
{
	auto category = make_unique<WJPANEL>();
	category->setObjectName("wjpCategory");
	category->setTitle("Topical Category");
	wjpCategory = category.get();
	basicWJP.push_back(wjpCategory);
	wjpCategory->setHidden(1);
	return category;
}
unique_ptr<WJPANEL> WJCONFIG::makePanelTopicCol()
{
	auto topicCol = make_unique<WJPANEL>();
	topicCol->setObjectName("wjpTopicCol");
	topicCol->setTitle("Table Column Headers");
	wjpTopicCol = topicCol.get();
	basicWJP.push_back(wjpTopicCol);
	wjpTopicCol->setHidden(1);
	return topicCol;
}
unique_ptr<WJPANEL> WJCONFIG::makePanelTopicRow()
{
	auto topicRow = make_unique<WJPANEL>();
	topicRow->setObjectName("wjpTopicRow");
	topicRow->setTitle("Table Row Headers");
	wjpTopicRow = topicRow.get();
	basicWJP.push_back(wjpTopicRow);
	wjpTopicRow->setHidden(1);
	return topicRow;
}
unique_ptr<WJPANEL> WJCONFIG::makePanelVariable()
{
	auto variable = make_unique<WJPANEL>();
	int index = varWJP.size();
	string sName = "wjpVariable" + to_string(index);
	variable->setObjectName(sName);
	variable->setTitle("Parameter");
	//variable->initButtonMID(wrIconMID);
	variable->initStackedPB(linkIconChevronDown, linkIconChevronRight);
	return variable;
}
unique_ptr<WJPANEL> WJCONFIG::makePanelYear(vector<string> vsYear)
{
	auto year = make_unique<WJPANEL>();
	year->setObjectName("wjpYear");
	year->setTitle("Census Year");
	wjpYear = year.get();
	wjpYear->setCB(vsYear);
	basicWJP.push_back(wjpYear);
	wjpYear->cbTitle->changed().connect(this, std::bind(&WJCONFIG::yearChanged, this));
	wjpYear->setHidden(0);
	wjpYear->cbTitle->setEnabled(1);
	return year;
}
void WJCONFIG::removeDifferentiators()
{
	for (int ii = 0; ii < mapDiffIndex.size(); ii++)
	{
		vLayout->removeWidget(diffWP[ii]);
	}
	diffWP.clear();
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
void WJCONFIG::resetTopicMID()
{
	wjpTopicCol->mapFilterMID.clear();
	wjpTopicCol->jtMID.clear();
	wjpTopicCol->textMID->setText("");
	wjpTopicCol->viFilter.clear();
	wjpTopicCol->boxMID->setHidden(1);
	
	wjpTopicRow->mapFilterMID.clear();
	wjpTopicRow->jtMID.clear();
	wjpTopicRow->textMID->setText("");
	wjpTopicRow->viFilter.clear();
	wjpTopicRow->boxMID->setHidden(1);
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
void WJCONFIG::setMobile(bool goMobile)
{
	mobile = goMobile;
	widgetMobile(mobile);
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
void WJCONFIG::setPrompt(string& sP, vector<vector<string>>& vvsC, vector<vector<string>>& vvsP)
{
	lock_guard<mutex> lg(m_config);
	sPrompt = sP;
	vvsCata = vvsC;
	vvsPrompt = vvsP;
}
void WJCONFIG::toggleMobilePanel(Wt::WPanel*& wPanel, bool mobile)
{
	// This mobile toggling function is made for standard WPanel widgets (containing
	// WComboBoxes), rather than WJPANEL widgets.
	Wt::WContainerWidget* cwTemp = (Wt::WContainerWidget*)wPanel->centralWidget();
	Wt::WVBoxLayout* layoutTemp = (Wt::WVBoxLayout*)cwTemp->layout();
	int numCB = layoutTemp->count();
	Wt::WLayoutItem* layoutItem = nullptr;
	Wt::WComboBox* cbTemp = nullptr;
	if (mobile)
	{
		wPanel->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
		for (int ii = 0; ii < numCB; ii++)
		{
			layoutItem = layoutTemp->itemAt(ii);
			cbTemp = (Wt::WComboBox*)layoutItem->widget();
			cbTemp->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
			cbTemp->setMinimumSize(wlAuto, 75.0);
		}
	}
	else
	{
		wPanel->decorationStyle().font().setSize(Wt::FontSize::Medium);
		for (int ii = 0; ii < numCB; ii++)
		{
			layoutItem = layoutTemp->itemAt(ii);
			cbTemp = (Wt::WComboBox*)layoutItem->widget();
			cbTemp->decorationStyle().font().setSize(Wt::FontSize::Medium);
			cbTemp->setMinimumSize(wlAuto, wlAuto);
		}
	}
}
void WJCONFIG::topicSelChanged()
{
	// Use the current col/row MID selections to update the table's selected cell.
	if (wjpTopicRow->boxMID->isHidden() || wjpTopicCol->boxMID->isHidden()) { return; }
	int iRow = wjpTopicRow->getIndexMID(1);
	int iCol = wjpTopicCol->getIndexMID(1);
	headerSignal_.emit(iRow, iCol);
}
void WJCONFIG::topicSelClicked()
{
	if (wjpTopicRow != nullptr) { wjpTopicRow->jf.timerStart(); }
	if (wjpTopicCol != nullptr) { wjpTopicCol->jf.timerStart(); }
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
		wjpTopicCol->boxMID->setHidden(1);
	}
	if (prompt[4] == "*")
	{
		wjpTopicRow->boxMID->setHidden(1);
	}
	if (prompt[3] == "*" && prompt[4] == "*")
	{
		resetSignal_.emit(3);  // Reset all tabs.
		wjpTopicCol->viFilter.clear();
		wjpTopicRow->viFilter.clear();
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
void WJCONFIG::unhighlightPanel(Wt::WPanel*& wPanel, int widgetIndex)
{
	// This unhighlighting function is made for standard WPanel widgets (containing
	// WComboBoxes), rather than WJPANEL widgets. 
	int numItem;
	Wt::WContainerWidget* cwTitle = wPanel->titleBarWidget();
	Wt::WCssDecorationStyle& stylePanel = cwTitle->decorationStyle();
	stylePanel.setBackgroundColor(wcOffWhite);

	Wt::WContainerWidget* wBox = (Wt::WContainerWidget*)wPanel->centralWidget();
	int numCB = wBox->count();
	if (widgetIndex >= numCB) { return; }
	Wt::WComboBox* wCB = (Wt::WComboBox*)wBox->widget(widgetIndex);
	Wt::WCssDecorationStyle& styleCB = wCB->decorationStyle();
	styleCB.setBorder(wbDefaultCB);
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
		varWJP[index]->jtMID.clear();
		varWJP[index]->textMID->setText("");
		varWJP[index]->boxMID->setHidden(1);
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
	varWJP[index]->boxMID->setHidden(0);
	varWJP[index]->setTree(vsMID);
	varWJP[index]->setTitle("Parameter");
	varMIDChanged();
}
void WJCONFIG::widgetMobile(bool mobile)
{
	if (mobile)
	{
		setMaximumSize(wlAuto, wlAuto);
		if (pbMobile != nullptr)
		{
			pbMobile->setMinimumSize(wlAuto, 50.0);
			pbMobile->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
		}
		if (textCata != nullptr)
		{
			textCata->setMinimumSize(wlAuto, 100.0);
			textCata->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
		}
	}
	else
	{
		setMaximumSize(300.0, wlAuto);
		if (pbMobile != nullptr)
		{
			pbMobile->setMinimumSize(wlAuto, wlAuto);
			pbMobile->decorationStyle().font().setSize(Wt::FontSize::Medium);
		}
		if (textCata != nullptr)
		{
			textCata->setMinimumSize(wlAuto, wlAuto);
			textCata->decorationStyle().font().setSize(Wt::FontSize::Medium);
		}
	}

	for (int ii = 0; ii < basicWJP.size(); ii++)
	{
		if (basicWJP[ii] == nullptr) { continue; }
		basicWJP[ii]->toggleMobile(mobile);
	}
	for (int ii = 0; ii < diffWP.size(); ii++)
	{
		if (diffWP[ii] == nullptr) { continue; }
		toggleMobilePanel(diffWP[ii], mobile);
	}
	for (int ii = 0; ii < varWJP.size(); ii++)
	{
		if (varWJP[ii] == nullptr) { continue; }
		if (varWJP[ii] == nullptr) { continue; }
		varWJP[ii]->toggleMobile(mobile);
	}
	if (wjpDemo != nullptr) { wjpDemo->toggleMobile(mobile); }
}
