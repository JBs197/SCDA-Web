#include "wjmap.h"

using namespace std;

WJMAP::WJMAP() : WContainerWidget()
{
	initGUI();
}

void WJMAP::err(string message)
{
	string errorMessage = "WJMAP error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJMAP::initGUI()
{
	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	auto boxUnitPinUnique = make_unique<Wt::WContainerWidget>();
	auto boxUnitPin = vLayout->insertWidget(indexMain::UnitPin, std::move(boxUnitPinUnique));
	auto hLayoutUnique = make_unique<Wt::WHBoxLayout>();
	auto hLayout = boxUnitPin->setLayout(std::move(hLayoutUnique));

	auto unitLabelUnique = make_unique<Wt::WText>();
	auto unitLabel = hLayout->insertWidget(indexUnitPin::UnitLabel, std::move(unitLabelUnique));

	auto pbUnitUnique = make_unique<Wt::WPushButton>();
	auto pbUnit = hLayout->insertWidget(indexUnitPin::UnitButton, std::move(pbUnitUnique));

	hLayout->insertStretch(indexUnitPin::Stretch, 1);

	auto pbPinDataUnique = make_unique<Wt::WPushButton>("Pin Data To Bar Graph");
	auto pbPinData = hLayout->insertWidget(indexUnitPin::PinData, std::move(pbPinDataUnique));

	auto pbResetUnique = make_unique<Wt::WPushButton>("Reset Bar Graph");
	auto pbReset = hLayout->insertWidget(indexUnitPin::Reset, std::move(pbResetUnique));

	auto boxMapUnique = make_unique<Wt::WContainerWidget>();
	auto boxMap = vLayout->insertWidget(indexMain::Map, std::move(boxMapUnique));
	auto mapLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto mapLayout = boxMap->setLayout(std::move(mapLayoutUnique));
	auto mapUnique = make_unique<WTPAINT>();
	auto map = mapLayout->addWidget(std::move(mapUnique));

	auto boxTipUnique = make_unique<Wt::WContainerWidget>();
	auto boxTip = vLayout->insertWidget(indexMain::Tip, std::move(boxTipUnique));
	auto gLayoutUnique = make_unique<WJGRIDLAYOUT>();
	auto gLayout = boxTip->setLayout(std::move(gLayoutUnique));

	auto tipUnique = make_unique<Wt::WText>("Tip: The easiest way to compare datasets from different maps is to pin each dataset to a bar graph, using the buttons located at the top-right corner of the data map.");
	auto tip = gLayout->addWidget(std::move(tipUnique), 0, 0, 2, 1);

	auto pbTipUnique = make_unique<Wt::WPushButton>();
	auto pbTip = gLayout->addWidget(std::move(pbTipUnique), 0, 1);

	gLayout->setColumnStretch(0, 1);
	gLayout->setRowStretch(1, 1);

}
