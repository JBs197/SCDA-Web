#include "wjtable.h"

using namespace std;

WJTABLE::WJTABLE() : WContainerWidget()
{
	initGUI();
}

void WJTABLE::initGUI()
{
	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	auto boxUnitPinUnique = make_unique<Wt::WContainerWidget>();
	auto boxUnitPin = vLayout->insertWidget(indexMain::UnitPin, std::move(boxUnitPinUnique));
	auto hLayoutUnique = make_unique<Wt::WHBoxLayout>();
	auto hLayout = boxUnitPin->setLayout(std::move(hLayoutUnique));

	auto unitLabelUnique = make_unique<Wt::WText>("None");
	auto unitLabel = hLayout->insertWidget(indexUnitPin::UnitLabel, std::move(unitLabelUnique));

	auto pbUnitUnique = make_unique<Wt::WPushButton>();
	auto pbUnit = hLayout->insertWidget(indexUnitPin::UnitButton, std::move(pbUnitUnique));
	pbUnit->setStyleClass("pbunit");

	hLayout->insertStretch(indexUnitPin::Stretch, 1);

	auto pbPinRowUnique = make_unique<Wt::WPushButton>("Pin Row Data To Bar Graph");
	auto pbPinRow = hLayout->insertWidget(indexUnitPin::PinRow, std::move(pbPinRowUnique));
	pbPinRow->setStyleClass("pbpin");

	auto pbPinColUnique = make_unique<Wt::WPushButton>("Pin Column Data To Bar Graph");
	auto pbPinCol = hLayout->insertWidget(indexUnitPin::PinColumn, std::move(pbPinColUnique));
	pbPinCol->setStyleClass("pbpin");

	auto pbResetUnique = make_unique<Wt::WPushButton>("Reset Bar Graph");
	auto pbReset = hLayout->insertWidget(indexUnitPin::Reset, std::move(pbResetUnique));
	pbReset->setStyleClass("pbpin");

	auto boxTableUnique = make_unique<Wt::WContainerWidget>();
	auto boxTable = vLayout->insertWidget(indexMain::Table, std::move(boxTableUnique));
	auto tableLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto tableLayout = boxTable->setLayout(std::move(tableLayoutUnique));
	auto tableUnique = make_unique<Wt::WTable>();
	auto table = tableLayout->addWidget(std::move(tableUnique));

	auto boxTipUnique = make_unique<Wt::WContainerWidget>();
	auto boxTip = vLayout->insertWidget(indexMain::Tip, std::move(boxTipUnique));
	auto gLayoutUnique = make_unique<WJGRIDLAYOUT>();
	auto gLayout = boxTip->setLayout(std::move(gLayoutUnique));

	auto tipUnique = make_unique<Wt::WText>("Tip: Adjust any column width by dragging its column header edge left or right.");	
	auto tip = gLayout->addWidget(std::move(tipUnique), 0, 0, 2, 1);

	auto pbTipUnique = make_unique<Wt::WPushButton>();
	auto pbTip = gLayout->addWidget(std::move(pbTipUnique), 0, 1);

	gLayout->setColumnStretch(0, 1);
	gLayout->setRowStretch(1, 1);

}
