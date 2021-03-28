#include "SCDAwidget.h"

void SCDAwidget::makeUI(unique_ptr<Wt::WContainerWidget> boxc, unique_ptr<Wt::WContainerWidget> boxtl, unique_ptr<Wt::WContainerWidget> boxt)
{
	// Add Bootstrap theme?
	auto vLayout = make_unique<Wt::WVBoxLayout>();  // Add graphics underneath later.
	auto hLayout = make_unique<Wt::WHBoxLayout>();
	hLayout->addWidget(move(boxc));
	hLayout->addWidget(move(boxtl));
	hLayout->addWidget(move(boxt));
	vLayout->addLayout(move(hLayout));
	this->setLayout(move(vLayout));
}
void SCDAwidget::init()
{
	clear();
	auto uniqueBoxControl = make_unique<Wt::WContainerWidget>();
	boxControl = uniqueBoxControl.get();
	auto uniqueBoxTreelist = make_unique<Wt::WContainerWidget>();
	boxTreelist = uniqueBoxTreelist.get();
	auto uniqueBoxTable = make_unique<Wt::WContainerWidget>();
	boxTable = uniqueBoxTable.get();

	makeUI(move(uniqueBoxControl), move(uniqueBoxTreelist), move(uniqueBoxTable));
}
