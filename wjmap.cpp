#include "wjmap.h"

void WJLEGEND::display(bool oddItalics)
{
	Wt::WContainerWidget::clear();
	auto layout = make_unique<Wt::WVBoxLayout>();
	Wt::WColor wColour;
	Wt::WString wsTemp;
	string temp;
	for (int ii = 0; ii < vvsParameter.size(); ii++)
	{
		if (oddItalics)
		{
			temp = italicize(vvsParameter[ii], 2);
			wsTemp = Wt::WString::fromUTF8(temp);
		}
		else 
		{ 
			temp = italicize(vvsParameter[ii], 0);
			wsTemp = Wt::WString::fromUTF8(temp); 
		}
		auto wText = make_unique<Wt::WText>(wsTemp);
		wText->setTextAlignment(Wt::AlignmentFlag::Left);
		wText->decorationStyle().font().setSize(Wt::FontSize::Large);
		if (vColour.size() > ii)
		{
			wColour = Wt::WColor(colourList[vColour[ii]][0], colourList[vColour[ii]][1], colourList[vColour[ii]][2]);
			wText->decorationStyle().setBackgroundColor(wColour);
		}
		layout->addWidget(move(wText));
	}
	this->setLayout(move(layout));
}
void WJLEGEND::initColour()
{
	colourList.resize(5);
	colourList[0] = { 0, 0, 0, 255 };  // Black
	colourList[1] = { 255, 255, 255, 255 };  // White
	colourList[2] = { 210, 210, 210, 255 };  // Light Grey
	colourList[3] = { 200, 200, 255, 255 };  // SelectedWeak
	colourList[4] = { 150, 150, 192, 255 };  // SelectedStrong
}
string WJLEGEND::italicize(vector<string>& vsParameter, int italicFreq)
{
	// Adds HTML italic tags to every Nth segment within vsParameter, and 
	// combines all parameters into a single string segmented by markers.
	string italic;
	for (int ii = 0; ii < vsParameter.size(); ii++)
	{
		if (ii > 0) { italic += " | "; }
		if (italicFreq > 0)
		{
			if (ii % italicFreq == italicFreq - 1) { italic += "<i>"; }
			italic += vsParameter[ii];
			if (ii % italicFreq == italicFreq - 1) { italic += "</i>"; }
		}
		else { italic += vsParameter[ii]; }
	}
	return italic;
}
string WJLEGEND::italicizeOdd(string& sParameter)
{
	// Adds HTML italic tags to odd-numbered segments within sParameter.
	string italic;
	size_t pos1, pos2, posStart, posStop;
	posStart = sParameter.find('|');
	if (posStart > sParameter.size())
	{
		italic = sParameter;
		return italic;
	}
	pos1 = sParameter.find_first_not_of(' ', posStart + 1);
	italic = sParameter.substr(0, pos1);
	posStop = sParameter.find('|', pos1);
	while (posStop < sParameter.size())
	{
		pos2 = sParameter.find_last_not_of(' ', posStop - 1) + 1;
		italic += "<i>" + sParameter.substr(pos1, pos2 - pos1) + "</i>";
		posStart = sParameter.find('|', posStop + 1);
		if (posStart > sParameter.size()) { break; }
		pos1 = sParameter.find_first_not_of(' ', posStart + 1);
		italic += sParameter.substr(pos2, pos1 - pos2);  // Non-italicized even-numbered segment.
		posStop = sParameter.find('|', pos1);
	}
	if (posStart < posStop)  // pos1 is valid, and we have reached the end of the parameter.
	{
		italic += "<i>" + sParameter.substr(pos1) + "</i>";
	}
	else  // pos2 is valid, and we have reached the end of the parameter.
	{
		italic += sParameter.substr(pos2);
	}
	return italic;
}
void WJLEGEND::setColour(vector<int>& viChanged)
{
	if (viChanged.size() != vvsParameter.size()) { jf.err("Size mismatch-wjlegend.setColour"); }
	vColour.resize(viChanged.size());
	for (int ii = 0; ii < vColour.size(); ii++)
	{
		if (ii % 2 == 1)
		{
			if (viChanged[ii] > 0) { vColour[ii] = 4; }
			else { vColour[ii] = 2; }
		}
		else
		{
			if (viChanged[ii] > 0) { vColour[ii] = 3; }
			else { vColour[ii] = 1; }
		}
	}
}

void WJMAP::addTipPin(int layoutIndex)
{
	Wt::WFlags<Wt::Orientation> flags;
	Wt::WColor wcTip(255, 255, 180);
	auto box = make_unique<Wt::WContainerWidget>();
	box->decorationStyle().setBackgroundColor(wcTip);
	box->setPadding(0.0);
	auto tipLayout = make_unique<Wt::WGridLayout>();

	auto boxText = make_unique<Wt::WContainerWidget>();
	boxText->setPadding(0.0);
	auto text = make_unique<Wt::WText>();
	text->decorationStyle().font().setSize(Wt::FontSize::Small);
	text->decorationStyle().font().setWeight(Wt::FontWeight::Bold);
	Wt::WString wsTip = "Tip: The easiest way to compare datasets from different maps is to pin each dataset to a bar graph, using the buttons located at the top-right corner of the data map.";
	text->setText(wsTip);
	boxText->addWidget(move(text));
	tipLayout->addWidget(move(boxText), 0, 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Middle);

	auto button = make_unique<Wt::WPushButton>();
	button->setMinimumSize(25.0, 25.0);
	Wt::WCssDecorationStyle& buttonStyle = button->decorationStyle();
	buttonStyle.setBackgroundColor(Wt::StandardColor::White);
	buttonStyle.setBackgroundImage(linkIconClose, flags, Wt::Side::CenterX | Wt::Side::CenterY);
	button->clicked().connect(this, std::bind(&WJMAP::removeTipPin, this, layoutIndex));
	tipLayout->addWidget(move(button), 0, 1, Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle);

	auto boxDummy = make_unique<Wt::WContainerWidget>();
	tipLayout->addWidget(move(boxDummy), 2, 0, Wt::AlignmentFlag::Left | Wt::AlignmentFlag::Top);
	tipLayout->setColumnStretch(0, 1);
	tipLayout->setRowStretch(2, 1);

	box->setLayout(move(tipLayout));
	Wt::WVBoxLayout* bigLayout = (Wt::WVBoxLayout*)this->layout();
	bigLayout->insertWidget(layoutIndex, move(box));
}
void WJMAP::build()
{
	auto boxUnitPin = makeUnitPinBox();
	auto boxMapUnique = make_unique<Wt::WContainerWidget>();
	boxMapUnique->setContentAlignment(Wt::AlignmentFlag::Center);
	auto wjLegend = make_unique<WJLEGEND>();

	auto vLayout = make_unique<Wt::WVBoxLayout>();
	boxOption = vLayout->addWidget(move(boxUnitPin));
	boxMap = vLayout->addWidget(move(boxMapUnique));
	wjlMap = vLayout->addWidget(move(wjLegend));
	vLayout->addStretch(1);
	this->setLayout(move(vLayout));
}
int WJMAP::getLegendBarDouble(vector<string>& vsRegion, string sUnit, int displayDataSize)
{
	if (vsRegion.size() > 2 && sUnit == "# of persons" && displayDataSize == 1) { return 1; }
	else if (vsRegion[0] == "Canada") { return 2; }
	else { return 0; }
}
int WJMAP::getLegendTickLines(string sUnit)
{
	if (sUnit == "$" || sUnit == "%") { return 1; }
	else { return 2; }
}
void WJMAP::init()
{
	build();
	initColour();
	initMaps();

	wlAuto = Wt::WLength::Auto;

	textUnit->setText("");
	textUnit->decorationStyle().font().setSize(Wt::FontSize::Large);
	textUnit->setTextAlignment(Wt::AlignmentFlag::Middle);
}
void WJMAP::initColour()
{
	wcSelectedWeak = Wt::WColor(200, 200, 255);
	wcWhite = Wt::WColor(255, 255, 255);
}
void WJMAP::initMaps()
{
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
}
unique_ptr<Wt::WContainerWidget> WJMAP::makeUnitPinBox()
{
	auto boxOptionUnique = make_unique<Wt::WContainerWidget>();
	auto textUnitUnique = make_unique<Wt::WText>();
	textUnitUnique->setTextFormat(Wt::TextFormat::Plain);
	auto pbUnitUnique = make_unique<Wt::WPushButton>();
	auto popupUnitUnique = make_unique<Wt::WPopupMenu>();
	popupUnit = popupUnitUnique.get();
	auto pbPinUnique = make_unique<Wt::WPushButton>("Pin Data To Bar Graph");
	auto pbPinResetUnique = make_unique<Wt::WPushButton>("Reset Bar Graph");

	auto layoutOption = make_unique<Wt::WHBoxLayout>();
	textUnit = layoutOption->addWidget(move(textUnitUnique));
	pbUnitUnique->setMenu(move(popupUnitUnique));
	pbUnit = layoutOption->addWidget(move(pbUnitUnique));
	layoutOption->addStretch(1);
	pbPin = layoutOption->addWidget(move(pbPinUnique));
	pbPinReset = layoutOption->addWidget(move(pbPinResetUnique));
	boxOptionUnique->setLayout(move(layoutOption));

	return boxOptionUnique;
}
void WJMAP::removeTipPin(int layoutIndex)
{
	Wt::WVBoxLayout* bigLayout = (Wt::WVBoxLayout*)this->layout();
	Wt::WLayoutItem* layoutItem = bigLayout->itemAt(layoutIndex);
	bigLayout->removeItem(layoutItem);
	tipSignal_.emit("mapPin");
}
void WJMAP::resetMenu()
{
	auto popupItems = popupUnit->items();
	for (int ii = 0; ii < popupItems.size(); ii++)
	{
		popupUnit->removeItem(popupItems[ii]);
	}
}
void WJMAP::updatePinButtons(vector<string> vsTooltip)
{
	if (vsTooltip.size() != 2) { jf.err("Invalid input-wjmap.updatePinButtons"); }
	Wt::WPushButton* wpb = nullptr;
	Wt::WString wsTemp;
	for (int ii = 0; ii < vsTooltip.size(); ii++)
	{
		switch (ii) {
		case 0:
			wpb = pbPin;
			break;
		case 1:
			wpb = pbPinReset;
			break;
		}
		if (vsTooltip[ii].size() < 1) {
			wsTemp = "";
			wpb->setToolTip(wsTemp);
			wpb->decorationStyle().setBackgroundColor(wcSelectedWeak);
			wpb->setEnabled(1);
		}
		else if (mapTooltip.count(vsTooltip[ii])) {
			wsTemp = mapTooltip.at(vsTooltip[ii]);
			wpb->setToolTip(wsTemp);
			wpb->decorationStyle().setBackgroundColor(wcWhite);
			wpb->setEnabled(0);
		}
		else { jf.err("Tooltip not found-wjmap.updatePinButtons"); }
	}
}
void WJMAP::widgetMobile(bool Mobile)
{
	if (Mobile == mobile) { return; }
	mobile = Mobile;
	Wt::WVBoxLayout* vLayout = (Wt::WVBoxLayout*)this->layout();
	Wt::WLayoutItem* wlItem = nullptr;
	Wt::WBoxLayout* boxLayoutOld = (Wt::WBoxLayout*)boxOption->layout();
	unique_ptr<Wt::WBoxLayout> boxLayout = nullptr;

	if (mobile) {
		boxLayout = make_unique<Wt::WBoxLayout>(Wt::LayoutDirection::TopToBottom);

		auto pbPinResetUnique = boxLayoutOld->removeWidget(pbPinReset);
		pbPinResetUnique->setMinimumSize(wlAuto, 50.0);
		pbPinResetUnique->decorationStyle().font().setSize(Wt::FontSize::XXLarge);

		auto pbPinUnique = boxLayoutOld->removeWidget(pbPin);
		pbPinUnique->setMinimumSize(wlAuto, 50.0);
		pbPinUnique->decorationStyle().font().setSize(Wt::FontSize::XXLarge);

		auto pbUnitUnique = boxLayoutOld->removeWidget(pbUnit);
		pbUnitUnique->setMinimumSize(wlAuto, 50.0);
		pbUnitUnique->decorationStyle().font().setSize(Wt::FontSize::XXLarge);

		auto textUnique = boxLayoutOld->removeWidget(textUnit);
		textUnique->setMinimumSize(wlAuto, 50.0);
		textUnique->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
		textUnit = (Wt::WText*)textUnique.get();

		auto unitLayout = make_unique<Wt::WHBoxLayout>();
		unitLayout->addWidget(move(textUnique), 0, Wt::AlignmentFlag::Middle);
		pbUnit = (Wt::WPushButton*)unitLayout->addWidget(move(pbUnitUnique), 1);
		boxLayout->addLayout(move(unitLayout));

		pbPin = (Wt::WPushButton*)boxLayout->addWidget(move(pbPinUnique));
		pbPinReset = (Wt::WPushButton*)boxLayout->addWidget(move(pbPinResetUnique));
	}
	else
	{
		boxLayout = make_unique<Wt::WBoxLayout>(Wt::LayoutDirection::LeftToRight);

		auto pbPinResetUnique = boxLayoutOld->removeWidget(pbPinReset);
		pbPinResetUnique->setMinimumSize(wlAuto, wlAuto);
		pbPinResetUnique->decorationStyle().font().setSize(Wt::FontSize::Medium);

		auto pbPinUnique = boxLayoutOld->removeWidget(pbPin);
		pbPinUnique->setMinimumSize(wlAuto, wlAuto);
		pbPinUnique->decorationStyle().font().setSize(Wt::FontSize::Medium);

		wlItem = boxLayoutOld->itemAt(0);
		auto unitLayout = (Wt::WBoxLayout*)wlItem->layout();

		auto pbUnitUnique = unitLayout->removeWidget(pbUnit);
		pbUnitUnique->setMinimumSize(wlAuto, wlAuto);
		pbUnitUnique->decorationStyle().font().setSize(Wt::FontSize::Medium);

		auto textUnique = unitLayout->removeWidget(textUnit);
		textUnique->setMinimumSize(wlAuto, wlAuto);
		textUnique->decorationStyle().font().setSize(Wt::FontSize::Large);

		textUnit = (Wt::WText*)boxLayout->addWidget(move(textUnique), 0);
		pbUnit = (Wt::WPushButton*)boxLayout->addWidget(move(pbUnitUnique), 0);
		boxLayout->addStretch(1);
		pbPin = (Wt::WPushButton*)boxLayout->addWidget(move(pbPinUnique), 0);
		pbPinReset = (Wt::WPushButton*)boxLayout->addWidget(move(pbPinResetUnique), 0);
	}

	boxOption->setLayout(move(boxLayout));
}
