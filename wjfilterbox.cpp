#include "wjfilterbox.h"

using namespace std;

WJFILTERBOX::WJFILTERBOX(SCDAserver& serverRef) : WContainerWidget(), sRef(serverRef)
{
	initGrid();
	initGUI();
}

void WJFILTERBOX::err(string message)
{
	sRef.errClient(message, "WJFILTERBOX");
}
void WJFILTERBOX::initGrid()
{
	mapGrid.emplace("year", make_pair(0, 0));
	mapGrid.emplace("category", make_pair(1, 0));
	mapGrid.emplace("rowTopic", make_pair(2, 0));
	mapGrid.emplace("colTopic", make_pair(3, 0));
	mapGrid.emplace("parameter1", make_pair(0, 1));
	mapGrid.emplace("parameter2", make_pair(1, 1));
	mapGrid.emplace("parameter3", make_pair(2, 1));
	mapGrid.emplace("parameter4", make_pair(3, 1));
}
void WJFILTERBOX::initGUI()
{
	auto gLayoutUnique = make_unique<WJGRIDLAYOUT>();
	auto gLayout = this->setLayout(std::move(gLayoutUnique));

	auto filterYearUnique = make_unique<WJFILTER>("Select Census Year");
	auto filterYear = gLayout->addWidget(std::move(filterYearUnique), mapGrid.at("year"));

	auto filterCategoryUnique = make_unique<WJFILTER>("Select Census Category");
	auto filterCategory = gLayout->addWidget(std::move(filterCategoryUnique), mapGrid.at("category"));

	auto filterRowUnique = make_unique<WJFILTER>("Select Row Topic");
	auto filterRow = gLayout->addWidget(std::move(filterRowUnique), mapGrid.at("rowTopic"));

	auto filterColUnique = make_unique<WJFILTER>("Select Column Topic");
	auto filterCol = gLayout->addWidget(std::move(filterColUnique), mapGrid.at("colTopic"));

	auto filterParam1Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam1 = (WJFILTER*)gLayout->addWidget(std::move(filterParam1Unique), mapGrid.at("parameter1"));
	filterParam1->setEnabled(1);

	auto filterParam2Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam2 = (WJFILTER*)gLayout->addWidget(std::move(filterParam2Unique), mapGrid.at("parameter2"));
	filterParam2->setEnabled(0);

	auto filterParam3Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam3 = (WJFILTER*)gLayout->addWidget(std::move(filterParam3Unique), mapGrid.at("parameter3"));
	filterParam3->setEnabled(0);

	auto filterParam4Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam4 = (WJFILTER*)gLayout->addWidget(std::move(filterParam4Unique), mapGrid.at("parameter4"));
	filterParam4->setEnabled(0);
}
void WJFILTERBOX::initFilter()
{
	// Populate the filter comboboxes using the list of catalogues (and their values).
	int numCata = (int)vCata.size();
	if (numCata == 0) { err("No loaded catalogues-initFilter"); }
	auto gLayout = (WJGRIDLAYOUT*)this->layout();

	setCategory.clear();
	setColTopic.clear();
	setParameter.clear();
	setRowTopic.clear();
	setYear.clear();
	for (WJCATA wjCata : vCata) {
		setCategory.emplace(wjCata.category);
		setColTopic.emplace(wjCata.colTopic);
		setYear.emplace(wjCata.year);
		if (wjCata.rowTopic.size() > 0) {
			setRowTopic.emplace(wjCata.rowTopic);
		}
		for (string param : wjCata.vParameter) {
			setParameter.emplace(param);
		}
	}

	auto wlItem = gLayout->itemAtPosition(mapGrid.at("year"));
	auto wjFilter = (WJFILTER*)wlItem->widget();
	vector<string> vsFilter(setYear.size());
	int index{ 0 };
	for (auto it = setYear.begin(); it != setYear.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initValue(vsFilter);

	wlItem = gLayout->itemAtPosition(mapGrid.at("category"));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilter.resize(setCategory.size());
	index = 0;
	for (auto it = setCategory.begin(); it != setCategory.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initValue(vsFilter);

	wlItem = gLayout->itemAtPosition(mapGrid.at("rowTopic"));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilter.resize(setRowTopic.size());
	index = 0;
	for (auto it = setRowTopic.begin(); it != setRowTopic.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initValue(vsFilter);

	wlItem = gLayout->itemAtPosition(mapGrid.at("colTopic"));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilter.resize(setColTopic.size());
	index = 0;
	for (auto it = setColTopic.begin(); it != setColTopic.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initValue(vsFilter);

	wlItem = gLayout->itemAtPosition(mapGrid.at("parameter1"));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilter.resize(setParameter.size());
	index = 0;
	for (auto it = setParameter.begin(); it != setParameter.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	vector<string> vsFilterCopy = vsFilter;
	wjFilter->initValue(vsFilterCopy);

	wlItem = gLayout->itemAtPosition(mapGrid.at("parameter2"));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilterCopy = vsFilter;
	wjFilter->initValue(vsFilterCopy);

	wlItem = gLayout->itemAtPosition(mapGrid.at("parameter3"));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilterCopy = vsFilter;
	wjFilter->initValue(vsFilterCopy);

	wlItem = gLayout->itemAtPosition(mapGrid.at("parameter4"));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilterCopy = vsFilter;
	wjFilter->initValue(vsFilterCopy);
}
