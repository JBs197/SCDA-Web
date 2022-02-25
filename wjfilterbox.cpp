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
	mapGrid.emplace("year", make_pair(index::Year, 0));
	mapGrid.emplace("category", make_pair(index::Category, 0));
	mapGrid.emplace("rowTopic", make_pair(index::RowTopic, 0));
	mapGrid.emplace("colTopic", make_pair(index::ColTopic, 0));
	mapGrid.emplace("parameter1", make_pair(0, 1));
	mapGrid.emplace("parameter2", make_pair(1, 1));
	mapGrid.emplace("parameter3", make_pair(2, 1));
	mapGrid.emplace("parameter4", make_pair(3, 1));
}
void WJFILTERBOX::initGUI()
{
	auto gLayoutUnique = make_unique<WJGRIDLAYOUT>();
	auto gLayout = this->setLayout(std::move(gLayoutUnique));

	string name = "year";
	auto filterYearUnique = make_unique<WJFILTER>("Select Census Year");
	auto filterYear = gLayout->addWidget(std::move(filterYearUnique), get<0>(mapGrid.at(name)), get<1>(mapGrid.at(name)));
	filterYear->setObjectName(name);

	name = "category";
	auto filterCategoryUnique = make_unique<WJFILTER>("Select Census Category");
	auto filterCategory = gLayout->addWidget(std::move(filterCategoryUnique), get<0>(mapGrid.at(name)), get<1>(mapGrid.at(name)));
	filterCategory->setObjectName(name);

	name = "rowTopic";
	auto filterRowUnique = make_unique<WJFILTER>("Select Row Topic");
	auto filterRow = gLayout->addWidget(std::move(filterRowUnique), get<0>(mapGrid.at(name)), get<1>(mapGrid.at(name)));
	filterRow->setObjectName(name);

	name = "colTopic";
	auto filterColUnique = make_unique<WJFILTER>("Select Column Topic");
	auto filterCol = gLayout->addWidget(std::move(filterColUnique), get<0>(mapGrid.at(name)), get<1>(mapGrid.at(name)));
	filterCol->setObjectName(name);

	name = "parameter1";
	auto filterParam1Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam1 = (WJFILTER*)gLayout->addWidget(std::move(filterParam1Unique), get<0>(mapGrid.at(name)), get<1>(mapGrid.at(name)));
	filterParam1->setObjectName(name);
	filterParam1->setEnabled(1);

	name = "parameter2";
	auto filterParam2Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam2 = (WJFILTER*)gLayout->addWidget(std::move(filterParam2Unique), get<0>(mapGrid.at(name)), get<1>(mapGrid.at(name)));
	filterParam2->setObjectName(name);
	filterParam2->setEnabled(0);

	name = "parameter3";
	auto filterParam3Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam3 = (WJFILTER*)gLayout->addWidget(std::move(filterParam3Unique), get<0>(mapGrid.at(name)), get<1>(mapGrid.at(name)));
	filterParam3->setObjectName(name);
	filterParam3->setEnabled(0);

	name = "parameter4";
	auto filterParam4Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam4 = (WJFILTER*)gLayout->addWidget(std::move(filterParam4Unique), get<0>(mapGrid.at(name)), get<1>(mapGrid.at(name)));
	filterParam4->setObjectName(name);
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

	resetFiltered();
}
void WJFILTERBOX::resetFiltered()
{
	if (setFiltered != nullptr) { setFiltered.reset(); }
	setFiltered = make_shared<set<int>>();
	int numCata = (int)vCata->size();
	for (int ii = 0; ii < numCata; ii++) {
		setFiltered->emplace(ii);
	}
}
void WJFILTERBOX::updateFiltered()
{
	resetFiltered();

	int numParam, selIndex;
	string filterName, filterValue;
	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	Wt::WLayoutItem* wlItem = nullptr;
	WJFILTER* wjFilter = nullptr;
	for (int ii = 0; ii < 4; ii++) {
		// Non-parameter filters.
		wlItem = gLayout->itemAtPosition(ii, 0);
		wjFilter = (WJFILTER*)wlItem->widget();
		wjFilter->getSelected(selIndex, filterValue);
		if (selIndex == 0) { continue; }
		
		filterName = wjFilter->objectName();
		switch (get<0>(mapGrid.at(filterName))) {
		case index::Year:
		{
			for (auto it = setFiltered->begin(); it != setFiltered->end(); ++it) {
				if (vCata->at(*it).year != filterValue) {
					setFiltered->erase(*it);
				}
			}
			break;
		}
		case index::Category:
		{
			for (auto it = setFiltered->begin(); it != setFiltered->end(); ++it) {
				if (vCata->at(*it).category != filterValue) {
					setFiltered->erase(*it);
				}
			}
			break;
		}
		case index::RowTopic:
		{
			for (auto it = setFiltered->begin(); it != setFiltered->end(); ++it) {
				if (vCata->at(*it).rowTopic != filterValue) {
					setFiltered->erase(*it);
				}
			}
			break;
		}
		case index::ColTopic:
		{
			for (auto it = setFiltered->begin(); it != setFiltered->end(); ++it) {
				if (vCata->at(*it).colTopic != filterValue) {
					setFiltered->erase(*it);
				}
			}
			break;
		}
		}
		if (setFiltered->size() == 0) { return; }
	}
	
	for (int ii = 0; ii < 4; ii++) {
		// Parameter filters.
		wlItem = gLayout->itemAtPosition(ii, 1);
		wjFilter = (WJFILTER*)wlItem->widget();
		if (!wjFilter->isEnabled()) { continue; }
		wjFilter->getSelected(selIndex, filterValue);
		if (selIndex == 0) { continue; }

		for (auto it = setFiltered->begin(); it != setFiltered->end(); ++it) {
			numParam = (int)vCata->at(*it).vParameter.size();
			for (int jj = 0; jj < numParam; jj++) {
				if (vCata->at(*it).vParameter[jj] == filterValue) { break; }
				else if (jj == numParam - 1) {
					setFiltered->erase(*it);
				}
			}
		}
		if (setFiltered->size() == 0) { return; }
	}
}
