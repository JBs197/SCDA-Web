#include "wjmap.h"

void WJLEGEND::display(bool italicize)
{
	Wt::WContainerWidget::clear();
	auto layout = make_unique<Wt::WVBoxLayout>();
	Wt::WColor wColour;
	Wt::WString wsTemp;
	string temp;
	for (int ii = 0; ii < vsParameter.size(); ii++)
	{
		if (italicize)
		{
			temp = italicizeOdd(vsParameter[ii]);
			wsTemp = Wt::WString::fromUTF8(temp);
		}
		else { wsTemp = Wt::WString::fromUTF8(vsParameter[ii]); }
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
	if (viChanged.size() != vsParameter.size()) { jf.err("Size mismatch-wjlegend.setColour"); }
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

void WJMAP::build()
{
	auto boxOptionUnique = make_unique<Wt::WContainerWidget>();
	auto textUnitUnique = make_unique<Wt::WText>();
	auto pbUnitUnique = make_unique<Wt::WPushButton>();
	auto popupUnitUnique = make_unique<Wt::WPopupMenu>();
	popupUnit = popupUnitUnique.get();
	auto pbPinUnique = make_unique<Wt::WPushButton>("Pin Data To Bar Graph");
	auto pbPinResetUnique = make_unique<Wt::WPushButton>("Reset Bar Graph");
	auto boxMapUnique = make_unique<Wt::WContainerWidget>();
	boxMapUnique->setContentAlignment(Wt::AlignmentFlag::Center);
	auto wjLegend = make_unique<WJLEGEND>();

	auto layoutOption = make_unique<Wt::WHBoxLayout>();
	textUnit = layoutOption->addWidget(move(textUnitUnique));
	pbUnitUnique->setMenu(move(popupUnitUnique));
	pbUnit = layoutOption->addWidget(move(pbUnitUnique));
	layoutOption->addStretch(1);
	pbPin = layoutOption->addWidget(move(pbPinUnique));
	pbPinReset = layoutOption->addWidget(move(pbPinResetUnique));
	boxOptionUnique->setLayout(move(layoutOption));

	auto vLayout = make_unique<Wt::WVBoxLayout>();
	boxOption = vLayout->addWidget(move(boxOptionUnique));
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

	wlAuto = Wt::WLength::Auto;

	textUnit->setText("");
	textUnit->decorationStyle().font().setSize(Wt::FontSize::Large);
	textUnit->setTextAlignment(Wt::AlignmentFlag::Middle);

}
void WJMAP::resetMenu()
{
	auto popupItems = popupUnit->items();
	for (int ii = 0; ii < popupItems.size(); ii++)
	{
		popupUnit->removeItem(popupItems[ii]);
	}
}
void WJMAP::updateUnit(string sUnit)
{

}
void WJMAP::widgetMobile(bool Mobile)
{
	if (Mobile == mobile) { return; }
	mobile = Mobile;
	Wt::WLayoutItem* wlItem = nullptr;
	Wt::WBoxLayout* layoutOld = (Wt::WBoxLayout*)boxOption->layout();
	int direction = (int)layoutOld->direction();  // 0 or 1 = horizontal,  2 or 3 = vertical
	int numItem = layoutOld->count();
	vector<unique_ptr<Wt::WLayoutItem>> vItem(numItem);
	for (int ii = numItem - 1; ii >= 0; ii--)
	{
		wlItem = layoutOld->itemAt(ii);
		vItem[ii] = move(layoutOld->removeItem(wlItem));
	}
	if (mobile)
	{
		auto vLayout = make_unique<Wt::WVBoxLayout>();
		for (int ii = 0; ii < numItem; ii++)
		{
			vLayout->addItem(move(vItem[ii]));
		}
		pbUnit->setMinimumSize(wlAuto, 50.0);
		pbUnit->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
		pbPin->setMinimumSize(wlAuto, 50.0);
		pbPin->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
		pbPinReset->setMinimumSize(wlAuto, 50.0);
		pbPinReset->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
		textUnit->setMinimumSize(wlAuto, 100.0);
		textUnit->decorationStyle().font().setSize(Wt::FontSize::XXLarge);
	}
	else
	{
		auto hLayout = make_unique<Wt::WHBoxLayout>();
		for (int ii = 0; ii < numItem; ii++)
		{
			hLayout->addItem(move(vItem[ii]));
		}
		pbUnit->setMinimumSize(wlAuto, wlAuto);
		pbUnit->decorationStyle().font().setSize(Wt::FontSize::Medium);
		pbPin->setMinimumSize(wlAuto, wlAuto);
		pbPin->decorationStyle().font().setSize(Wt::FontSize::Medium);
		pbPinReset->setMinimumSize(wlAuto, wlAuto);
		pbPinReset->decorationStyle().font().setSize(Wt::FontSize::Medium);
		textUnit->setMinimumSize(wlAuto, wlAuto);
		textUnit->decorationStyle().font().setSize(Wt::FontSize::Large);
	}
}
