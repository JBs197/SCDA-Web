#include "wjfilterbox.h"

using namespace std;

WJFILTERBOX::WJFILTERBOX(SCDAserver& serverRef) : WContainerWidget(), sRef(serverRef)
{
	initGUI();
}

void WJFILTERBOX::err(string message)
{
	sRef.errClient(message, "WJFILTERBOX");
}
void WJFILTERBOX::setFilter(vector<vector<string>>& vvsFilter)
{
	// vvsFilter has form [Year, Category, Row Topic, Column Topic, Parameter][Value].
	if (vvsFilter.size() < 5) { err("Insufficient input-initFilter"); }
	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	Wt::WLayoutItem* wlItem = nullptr;
	WJFILTER* wjFilter = nullptr;

	int numItem;
	for (int ii = 0; ii < 4; ii++) {
		wlItem = gLayout->itemAtPosition(ii, 0);
		wjFilter = (WJFILTER*)wlItem->widget();
		numItem = (int)vvsFilter[ii].size();
		jsort.sortAlphabetically(vvsFilter[ii]);
		wjFilter->setValue(vvsFilter[ii]);
	}
	wlItem = gLayout->itemAtPosition(0, 1);
	wjFilter = (WJFILTER*)wlItem->widget();
	numItem = (int)vvsFilter[4].size();
	jsort.sortAlphabetically(vvsFilter[4]);
	wjFilter->setValue(vvsFilter[4]);
}
void WJFILTERBOX::initGUI()
{
	auto gLayoutUnique = make_unique<WJGRIDLAYOUT>();
	auto gLayout = this->setLayout(std::move(gLayoutUnique));

	auto filterYearUnique = make_unique<WJFILTER>("Select Census Year");
	auto filterYear = gLayout->addWidget(std::move(filterYearUnique), 0, 0);

	auto filterCategoryUnique = make_unique<WJFILTER>("Select Census Category");
	auto filterCategory = gLayout->addWidget(std::move(filterCategoryUnique), 1, 0);

	auto filterRowUnique = make_unique<WJFILTER>("Select Row Topic");
	auto filterRow = gLayout->addWidget(std::move(filterRowUnique), 2, 0);

	auto filterColUnique = make_unique<WJFILTER>("Select Column Topic");
	auto filterCol = gLayout->addWidget(std::move(filterColUnique), 3, 0);

	auto filterParam1Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam1 = gLayout->addWidget(std::move(filterParam1Unique), 0, 1);
	filterParam1->setEnabled(1);

	auto filterParam2Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam2 = gLayout->addWidget(std::move(filterParam2Unique), 1, 1);
	filterParam2->setEnabled(0);

	auto filterParam3Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam3 = gLayout->addWidget(std::move(filterParam3Unique), 2, 1);
	filterParam3->setEnabled(0);

	auto filterParam4Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam4 = gLayout->addWidget(std::move(filterParam4Unique), 3, 1);
	filterParam4->setEnabled(0);
}
