#include "wjfilterbox.h"

using namespace std;

WJFILTERBOX::WJFILTERBOX(shared_ptr<vector<WJCATA>>& vCataRef, std::shared_ptr<std::set<int>>& setPassedRef) 
	: vCata(vCataRef), setPassed(setPassedRef)
{
	setStyleClass("wjfilterbox");
	initGUI();
}

void WJFILTERBOX::err(string message)
{
	string errorMessage = "WJFILTERBOX error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJFILTERBOX::getFilterAll(vector<string>& vsFilter)
{
	// Return a list of each active filter.
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	Wt::WLayoutItem* wlItem = nullptr;
	WJFILTER* wjFilter = nullptr;
	Wt::WContainerWidget* box = nullptr;
	Wt::WHBoxLayout* hLayout = nullptr;
	Wt::WText* label = nullptr;

	int numFilter = vLayout->count();
	vsFilter.resize(numFilter);
	for (int ii = 0; ii < numFilter; ii++) {
		wlItem = vLayout->itemAt(ii);
		wjFilter = (WJFILTER*)wlItem->widget();
		box = (Wt::WContainerWidget*)wjFilter->centralWidget();
		if (box->isEnabled()) {			
			hLayout = (Wt::WHBoxLayout*)box->layout();
			wlItem = hLayout->itemAt(WJFILTER::Label);
			label = (Wt::WText*)wlItem->widget();
			const Wt::WString wsTemp = label->text();
			vsFilter[ii] = wsTemp.toUTF8();
		}
		else {
			vsFilter.resize(ii);
			break;
		}
	}
}
void WJFILTERBOX::initCataAll(vector<WJCATA> vCataAll)
{
	int numCata = (int)vCataAll.size();
	setCataAll.reset();
	setCataAll = make_shared<set<int>>();
	for (int ii = 0; ii < numCata; ii++) {
		setCataAll->emplace(ii);
	}
	vCata.reset();
	vCata = make_shared<vector<WJCATA>>(std::move(vCataAll));
}
void WJFILTERBOX::initGUI()
{
	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	string name = "year";
	auto filterYearUnique = make_unique<WJFILTER>("Census Year");
	auto filterYear = vLayout->insertWidget(index::Year, std::move(filterYearUnique));
	filterYear->setObjectName(name);
	filterYear->updateFilter().connect(this, bind(&WJFILTERBOX::updateFilterAll, this));

	name = "category";
	auto filterCategoryUnique = make_unique<WJFILTER>("Census Category");
	auto filterCategory = vLayout->insertWidget(index::Category, std::move(filterCategoryUnique));
	filterCategory->setObjectName(name);
	filterCategory->updateFilter().connect(this, bind(&WJFILTERBOX::updateFilterAll, this));

	name = "rowTopic";
	auto filterRowUnique = make_unique<WJFILTER>("Row Topic");
	auto filterRow = vLayout->insertWidget(index::RowTopic, std::move(filterRowUnique));
	filterRow->setObjectName(name);
	filterRow->updateFilter().connect(this, bind(&WJFILTERBOX::updateFilterAll, this));

	name = "colTopic";
	auto filterColUnique = make_unique<WJFILTER>("Column Topic");
	auto filterCol = vLayout->insertWidget(index::ColumnTopic, std::move(filterColUnique));
	filterCol->setObjectName(name);
	filterCol->updateFilter().connect(this, bind(&WJFILTERBOX::updateFilterAll, this));

	name = "parameter0";
	auto filterParam1Unique = make_unique<WJFILTER>("Parameter");
	auto filterParam1 = (WJFILTER*)vLayout->insertWidget(index::Parameter, std::move(filterParam1Unique));
	filterParam1->setObjectName(name);
	filterParam1->setEnabled(1);
	filterParam1->updateFilter().connect(this, bind(&WJFILTERBOX::updateFilterAll, this));

	/*
	name = "parameter2";
	auto filterParam2Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam2 = (WJFILTER*)vLayout->insertWidget(index::Parameter + 1, std::move(filterParam2Unique));
	filterParam2->setObjectName(name);
	filterParam2->setEnabled(0);
	filterParam2->updateFilter().connect(this, bind(&WJFILTERBOX::updateFilterAll, this));

	name = "parameter3";
	auto filterParam3Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam3 = (WJFILTER*)vLayout->insertWidget(index::Parameter + 2, std::move(filterParam3Unique));
	filterParam3->setObjectName(name);
	filterParam3->setEnabled(0);
	filterParam3->updateFilter().connect(this, bind(&WJFILTERBOX::updateFilterAll, this));

	name = "parameter4";
	auto filterParam4Unique = make_unique<WJFILTER>("Select a Parameter");
	auto filterParam4 = (WJFILTER*)vLayout->insertWidget(index::Parameter + 3, std::move(filterParam4Unique));
	filterParam4->setObjectName(name);
	filterParam4->setEnabled(0);
	filterParam4->updateFilter().connect(this, bind(&WJFILTERBOX::updateFilterAll, this));
	*/

}
void WJFILTERBOX::initFilter()
{
	// Populate the filter comboboxes using the list of catalogues (and their values).
	int numCata = (int)vCata->size();
	if (numCata == 0) { err("No loaded catalogues-initFilter"); }
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	Wt::WLayoutItem* wlItem = nullptr;
	WJFILTER* wjFilter = nullptr;
	vector<string> vsBlank{}, vsFilter;

	vFilterCandidate.reset();
	vFilterCandidate = make_shared<vector<set<string>>>();
	vFilterCandidate->resize(index::Count, set<string>());
	for (int ii = 0; ii < index::Count; ii++) {
		vFilterCandidate->at(ii).emplace("All");
	}

	for (int ii = 0; ii < numCata; ii++) {
		vFilterCandidate->at(index::Year).emplace(vCata->at(ii).year);
		vFilterCandidate->at(index::Category).emplace(vCata->at(ii).category);
		if (vCata->at(ii).rowTopic.size() > 0) {
			vFilterCandidate->at(index::RowTopic).emplace(vCata->at(ii).rowTopic);
		}
		vFilterCandidate->at(index::ColumnTopic).emplace(vCata->at(ii).colTopic);
		for (string param : vCata->at(ii).vParameter) {
			vFilterCandidate->at(index::Parameter).emplace(param);
		}
	}

	int index;
	for (int ii = 0; ii < index::Count; ii++) {
		wlItem = vLayout->itemAt(ii);
		wjFilter = (WJFILTER*)wlItem->widget();
		vsFilter.resize(vFilterCandidate->at(ii).size());
		index = 0;
		for (auto it = vFilterCandidate->at(ii).begin(); it != vFilterCandidate->at(ii).end(); ++it) {
			vsFilter[index] = *it;
			index++;
		}
		jsort.sortAlphabetically(vsFilter);
		wjFilter->initList(vsFilter, vsBlank);
	}

	/*
	for (int ii = 1; ii < numParameter; ii++) {
		wlItem = vLayout->itemAt(index::Parameter + ii);
		wjFilter = (WJFILTER*)wlItem->widget();
		vsFilter.resize(vFilterCandidate->at(index::Parameter).size());
		index = 0;
		for (auto it = vFilterCandidate->at(index::Parameter).begin(); it != vFilterCandidate->at(index::Parameter).end(); ++it) {
			vsFilter[index] = *it;
			index++;
		}
		jsort.sortAlphabetically(vsFilter);
		wjFilter->initList(vsFilter, vsBlank);
	}
	*/

	/*
	auto wlItem = gLayout->itemAt(index::Year);
	auto wjFilter = (WJFILTER*)wlItem->widget();
	vector<string> vsFilter(setYear.size());
	int index{ 0 };
	for (auto it = setYear.begin(); it != setYear.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initList(vsFilter, vsBlank);

	wlItem = gLayout->itemAt(index::Category);
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilter.resize(setCategory.size());
	index = 0;
	for (auto it = setCategory.begin(); it != setCategory.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initList(vsFilter, vsBlank);

	wlItem = gLayout->itemAt(index::RowTopic);
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilter.resize(setRowTopic.size());
	index = 0;
	for (auto it = setRowTopic.begin(); it != setRowTopic.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initList(vsFilter, vsBlank);

	wlItem = gLayout->itemAt(index::ColumnTopic);
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilter.resize(setColTopic.size());
	index = 0;
	for (auto it = setColTopic.begin(); it != setColTopic.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	wjFilter->initList(vsFilter, vsBlank);

	wlItem = gLayout->itemAt(index::Parameter);
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilter.resize(setParameter.size());
	index = 0;
	for (auto it = setParameter.begin(); it != setParameter.end(); ++it) {
		vsFilter[index] = *it;
		index++;
	}
	jsort.sortAlphabetically(vsFilter);
	vector<string> vsFilterCopy = vsFilter;
	wjFilter->initList(vsFilterCopy, vsBlank);

	wlItem = gLayout->itemAt(index::Parameter + 1);
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilterCopy = vsFilter;
	wjFilter->initList(vsFilterCopy, vsBlank);

	wlItem = gLayout->itemAt(index::Parameter + 2);
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilterCopy = vsFilter;
	wjFilter->initList(vsFilterCopy, vsBlank);

	wlItem = gLayout->itemAt(index::Parameter + 3);
	wjFilter = (WJFILTER*)wlItem->widget();
	vsFilterCopy = vsFilter;
	wjFilter->initList(vsFilterCopy, vsBlank);
	*/

	resetPassed();
}
void WJFILTERBOX::resetPassed()
{
	if (setPassed != nullptr) { setPassed.reset(); }
	setPassed = make_shared<set<int>>();
	int numCata = (int)vCata->size();
	for (int ii = 0; ii < numCata; ii++) {
		setPassed->emplace(ii);
	}
}
void WJFILTERBOX::setFilterAll(vector<vector<vector<string>>>& vvvsLiveDead)
{
	// vvvsLiveDead has form [filter index][live, dead][filter values].
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	Wt::WLayoutItem* wlItem = nullptr;
	WJFILTER* wjFilter = nullptr;
		
	int numFilter = (int)vvvsLiveDead.size();
	for (int ii = 0; ii < numFilter; ii++) {
		wlItem = vLayout->itemAt(ii);
		wjFilter = (WJFILTER*)wlItem->widget();
		wjFilter->initList(vvvsLiveDead[ii][0], vvvsLiveDead[ii][1]);
	}
}
void WJFILTERBOX::updateFilterAll()
{
	// Obtain the list of active filters, and update the list of successful catalogues
	// which pass them all, as well as each filter's live/dead lists.
	vector<string> vsFilter;
	getFilterAll(vsFilter);

	atomic_int statusLiveDead{ 0 };
	vector<vector<vector<string>>> vvvsLiveDead;
	auto pvCata = vCata.get();
	auto pvFilterCandidate = vFilterCandidate.get();
	std::jthread thr(&WJFILTERBOX::updateLiveDead, this, ref(vvvsLiveDead), ref(statusLiveDead), vsFilter, ref(pvCata), ref(pvFilterCandidate));

	updatePassed(vsFilter);
	while (statusLiveDead == 0) {
		this_thread::sleep_for(20ms);
	}
	setFilterAll(vvvsLiveDead);
}
void WJFILTERBOX::updateLiveDead(vector<vector<vector<string>>>& vvvsLiveDead, atomic_int& statusLiveDead, vector<string> vsFilter, vector<WJCATA>* vCata, vector<set<string>>* vFilterCandidate)
{
	// For all active filters, determine which of their alternatives 
	// would yield one or more successful candidate catalogues (live), or
	// if all catalogues would fail to pass the combined set of filters (dead).
	// Return form [filter index][live, dead][filter values].

	int numFilter = (int)vsFilter.size();
	vvvsLiveDead.resize(numFilter, vector<vector<string>>(2, vector<string>()));
	int numCata = (int)vCata->size();

	bool letMeOut;
	int numParam, numTest;
	set<int> setStarter;
	for (int ii = 0; ii < numFilter; ii++) {
		// Firstly, determine which catalogues will satisfy the collective group of
		// non-ii filters.
		setStarter = *setCataAll;
		for (int jj = 0; jj < numFilter; jj++) {
			if (vsFilter[jj] == "All" || jj == ii) { continue; }

			for (auto it = setStarter.begin(); it != setStarter.end();) {
				switch (jj) {
				case index::Year:
				{
					if (vCata->at(*it).year != vsFilter[jj]) {
						it = setStarter.erase(it);
					}
					else { ++it; }
					break;
				}
				case index::Category:
				{
					if (vCata->at(*it).category != vsFilter[jj]) {
						it = setStarter.erase(it);
					}
					else { ++it; }
					break;
				}
				case index::RowTopic:
				{
					if (vCata->at(*it).rowTopic != vsFilter[jj]) {
						it = setStarter.erase(it);
					}
					else { ++it; }
					break;
				}
				case index::ColumnTopic:
				{
					if (vCata->at(*it).colTopic != vsFilter[jj]) {
						it = setStarter.erase(it);
					}
					else { ++it; }
					break;
				}

				numParam = (int)vCata->at(*it).vParameter.size();
				for (int kk = 0; kk < numParam; kk++) {
					if (vCata->at(*it).vParameter[kk] == vsFilter[jj]) {
						++it;
						break;
					}
					else if (kk == numParam - 1) {
						it = setStarter.erase(it);
					}
				}
				}
			}
		}

		// If no catalogues survived the other filters, then all of this filter's 
		// candidates are dead. 
		if (setStarter.size() == 0) {
			for (auto it = vFilterCandidate->at(ii).begin(); it != vFilterCandidate->at(ii).end(); ++it) {
				vvvsLiveDead[ii][1].emplace_back(*it);
			}
			continue;
		}

		// Make a single modification, and see if any catalogues survive.
		for (auto itCandidate = vFilterCandidate->at(ii).begin(); itCandidate != vFilterCandidate->at(ii).end(); ++itCandidate) {
			if (*itCandidate == "All") {
				vvvsLiveDead[ii][0].emplace_back(*itCandidate);
			}
			else {
				letMeOut = 0;
				for (auto it = setStarter.begin(); it != setStarter.end(); ++it) {							
					switch (ii) {
					case index::Year:
					{
						if (vCata->at(*it).year == *itCandidate) {
							vvvsLiveDead[ii][0].emplace_back(*itCandidate);
							letMeOut = 1;
						}
						break;
					}
					case index::Category:
					{
						if (vCata->at(*it).category == *itCandidate) {
							vvvsLiveDead[ii][0].emplace_back(*itCandidate);
							letMeOut = 1;
						}
						break;
					}
					case index::RowTopic:
					{
						if (vCata->at(*it).rowTopic == *itCandidate) {
							vvvsLiveDead[ii][0].emplace_back(*itCandidate);
							letMeOut = 1;
						}
						break;
					}
					case index::ColumnTopic:
					{
						if (vCata->at(*it).colTopic == *itCandidate) {
							vvvsLiveDead[ii][0].emplace_back(*itCandidate);
							letMeOut = 1;
						}
						break;
					}

					numParam = (int)vCata->at(*it).vParameter.size();
					for (int jj = 0; jj < numParam; jj++) {
						if (vCata->at(*it).vParameter[jj] == *itCandidate) {
							vvvsLiveDead[ii][0].emplace_back(*itCandidate);
							letMeOut = 1;
							break;
						}
					}
					}

					if (letMeOut) { break; }
				}

				if (!letMeOut) { vvvsLiveDead[ii][1].emplace_back(*itCandidate); }
			}
		}
	}
	statusLiveDead = 1;
}
void WJFILTERBOX::updatePassed(vector<string>& vsFilter)
{
	// Reads the active filter in each filterbox, then re-creates the set of 
	// catalogue names which satisfy all active filters.
	
	resetPassed();

	int filteredIndex, numParam;
	string filterName, filterValue;
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	Wt::WLayoutItem* wlItem = nullptr;
	WJFILTER* wjFilter = nullptr;

	int numFilter = (int)vsFilter.size();
	for (int ii = 0; ii < numFilter; ii++) {
		if (vsFilter[ii] == "All") { continue; }

		switch (ii) {
		case index::Year:
		{
			for (auto it = setPassed->begin(); it != setPassed->end();) {
				if (vCata->at(*it).year != vsFilter[ii]) {
					it = setPassed->erase(it);
				}
				else { ++it; }
			}
			break;
		}
		case index::Category:
		{
			for (auto it = setPassed->begin(); it != setPassed->end();) {
				if (vCata->at(*it).category != vsFilter[ii]) {
					it = setPassed->erase(it);
				}
				else { ++it; }
			}
			break;
		}
		case index::RowTopic:
		{
			for (auto it = setPassed->begin(); it != setPassed->end();) {
				if (vCata->at(*it).rowTopic != vsFilter[ii]) {
					it = setPassed->erase(it);
				}
				else { ++it; }
			}
			break;
		}
		case index::ColumnTopic:
		{
			for (auto it = setPassed->begin(); it != setPassed->end();) {
				if (vCata->at(*it).colTopic != vsFilter[ii]) {
					it = setPassed->erase(it);
				}
				else { ++it; }
			}
			break;
		}

		// Subsequent indices are all parameter indices.
		for (auto it = setPassed->begin(); it != setPassed->end();) {
			numParam = (int)vCata->at(*it).vParameter.size();
			for (int jj = 0; jj < numParam; jj++) {
				if (vCata->at(*it).vParameter[jj] == vsFilter[ii]) {
					++it;
					break;
				}
				else if (jj == numParam - 1) {
					it = setPassed->erase(it);
				}
			}
		}
		}

		if (setPassed->size() == 0) { break; }
	}
	populateCataList_.emit();

	/*
	for (int ii = 0; ii < 4; ii++) {
		// Non-parameter filters.
		wlItem = vLayout->itemAt(ii);
		wjFilter = (WJFILTER*)wlItem->widget();
		filterValue = wjFilter->getSelected();
		if (filterValue == "All") { continue; }
		
		switch (ii) {
		case index::Year:
		{
			for (auto it = setPassed->begin(); it != setPassed->end();) {
				if (vCata->at(*it).year != filterValue) {
					it = setPassed->erase(it);
				}
				else { ++it; }
			}
			break;
		}
		case index::Category:
		{
			for (auto it = setPassed->begin(); it != setPassed->end();) {
				if (vCata->at(*it).category != filterValue) {
					it = setPassed->erase(it);
				}
				else { ++it; }
			}
			break;
		}
		case index::RowTopic:
		{
			for (auto it = setPassed->begin(); it != setPassed->end();) {
				if (vCata->at(*it).rowTopic != filterValue) {
					it = setPassed->erase(it);
				}
				else { ++it; }
			}
			break;
		}
		case index::ColumnTopic:
		{
			for (auto it = setPassed->begin(); it != setPassed->end();) {
				if (vCata->at(*it).colTopic != filterValue) {
					it = setPassed->erase(it);
				}
				else { ++it; }
			}
			break;
		}
		}
		if (setPassed->size() == 0) { 
			populateCataList_.emit();
			return; 
		}
	}
	for (int ii = 0; ii < 4; ii++) {
		// Parameter filters.
		wlItem = vLayout->itemAt(index::Parameter + ii);
		wjFilter = (WJFILTER*)wlItem->widget();
		if (!wjFilter->isEnabled()) { continue; }
		filterValue = wjFilter->getSelected();
		if (filterValue == "All") { continue; }

		for (auto it = setPassed->begin(); it != setPassed->end();) {
			numParam = (int)vCata->at(*it).vParameter.size();
			for (int jj = 0; jj < numParam; jj++) {
				if (vCata->at(*it).vParameter[jj] == filterValue) { 
					++it;
					break; 
				}
				else if (jj == numParam - 1) {
					it = setPassed->erase(it);
				}
			}
		}
		if (setPassed->size() == 0) { 
			populateCataList_.emit();
			return; 
		}
	}
	populateCataList_.emit();
	*/

}
