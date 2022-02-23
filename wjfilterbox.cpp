#include "wjfilterbox.h"

using namespace std;

WJFILTERBOX::WJFILTERBOX(SCDAserver& serverRef) : WContainerWidget(), sRef(serverRef)
{
	setStyleClass("wjfilterbox");
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
	auto filterYear = gLayout->addWidget(std::move(filterYearUnique), get<0>(mapGrid.at("year")), get<1>(mapGrid.at("year")));

	auto filterCategoryUnique = make_unique<WJFILTER>("Select Census Category");
	auto filterCategory = gLayout->addWidget(std::move(filterCategoryUnique), get<0>(mapGrid.at("category")), get<1>(mapGrid.at("category")));

	auto filterRowUnique = make_unique<WJFILTER>("Select Row Topic");
	auto filterRow = gLayout->addWidget(std::move(filterRowUnique), get<0>(mapGrid.at("rowTopic")), get<1>(mapGrid.at("rowTopic")));

	auto filterColUnique = make_unique<WJFILTER>("Select Column Topic");
	auto filterCol = gLayout->addWidget(std::move(filterColUnique), get<0>(mapGrid.at("colTopic")), get<1>(mapGrid.at("colTopic")));

	auto filterParam1Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam1 = (WJFILTER*)gLayout->addWidget(std::move(filterParam1Unique), get<0>(mapGrid.at("parameter1")), get<1>(mapGrid.at("parameter1")));
	filterParam1->setEnabled(1);

	auto filterParam2Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam2 = (WJFILTER*)gLayout->addWidget(std::move(filterParam2Unique), get<0>(mapGrid.at("parameter2")), get<1>(mapGrid.at("parameter2")));
	filterParam2->setEnabled(0);

	auto filterParam3Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam3 = (WJFILTER*)gLayout->addWidget(std::move(filterParam3Unique), get<0>(mapGrid.at("parameter3")), get<1>(mapGrid.at("parameter3")));
	filterParam3->setEnabled(0);

	auto filterParam4Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam4 = (WJFILTER*)gLayout->addWidget(std::move(filterParam4Unique), get<0>(mapGrid.at("parameter4")), get<1>(mapGrid.at("parameter4")));
	filterParam4->setEnabled(0);
}
void WJFILTERBOX::initFilter()
{
	// Populate the filter comboboxes using the list of catalogues (and their values).
	int numCata = (int)vCata->size();
	if (numCata == 0) { err("No loaded catalogues-initFilter"); }
	auto gLayout = (WJGRIDLAYOUT*)this->layout();

	setCategory.clear();
	setColTopic.clear();
	setParameter.clear();
	setRowTopic.clear();
	setYear.clear();
	for (int ii = 0; ii < numCata; ii++) {
		setCategory.emplace(vCata->at(ii).category);
		setColTopic.emplace(vCata->at(ii).colTopic);
		setYear.emplace(vCata->at(ii).year);
		if (vCata->at(ii).rowTopic.size() > 0) {
			setRowTopic.emplace(vCata->at(ii).rowTopic);
		}
		for (string param : vCata->at(ii).vParameter) {
			setParameter.emplace(param);
		}
	}

	auto wlItem = gLayout->itemAtPosition(get<0>(mapGrid.at("year")), get<1>(mapGrid.at("year")));
	auto wjFilter = (WJFILTER*)wlItem->widget();
	vector<string> vsFilter(setYear.size());
	int index{ 0 };
	for (auto it = setYear.begin(); it != setYear.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initValue(vsFilter);

	wlItem = gLayout->itemAtPosition(get<0>(mapGrid.at("category")), get<1>(mapGrid.at("category")));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilter.resize(setCategory.size());
	index = 0;
	for (auto it = setCategory.begin(); it != setCategory.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initValue(vsFilter);

	wlItem = gLayout->itemAtPosition(get<0>(mapGrid.at("rowTopic")), get<1>(mapGrid.at("rowTopic")));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilter.resize(setRowTopic.size());
	index = 0;
	for (auto it = setRowTopic.begin(); it != setRowTopic.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initValue(vsFilter);

	wlItem = gLayout->itemAtPosition(get<0>(mapGrid.at("colTopic")), get<1>(mapGrid.at("colTopic")));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilter.resize(setColTopic.size());
	index = 0;
	for (auto it = setColTopic.begin(); it != setColTopic.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initValue(vsFilter);

	wlItem = gLayout->itemAtPosition(get<0>(mapGrid.at("parameter1")), get<1>(mapGrid.at("parameter1")));
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

	wlItem = gLayout->itemAtPosition(get<0>(mapGrid.at("parameter2")), get<1>(mapGrid.at("parameter2")));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilterCopy = vsFilter;
	wjFilter->initValue(vsFilterCopy);

	wlItem = gLayout->itemAtPosition(get<0>(mapGrid.at("parameter3")), get<1>(mapGrid.at("parameter3")));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilterCopy = vsFilter;
	wjFilter->initValue(vsFilterCopy);

	wlItem = gLayout->itemAtPosition(get<0>(mapGrid.at("parameter4")), get<1>(mapGrid.at("parameter4")));
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilterCopy = vsFilter;
	wjFilter->initValue(vsFilterCopy);
}
