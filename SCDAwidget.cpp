#include "SCDAwidget.h"

using namespace std;

SCDAwidget::SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver)
{
	sRef.connect(this, bind(&SCDAwidget::processDataEvent, this, placeholders::_1));
	
	initGUI();

	string sessionID = Wt::WApplication::instance()->sessionId();
	sRef.pullCataAll(sessionID);
}

void SCDAwidget::initGUI()
{
	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	auto hLayoutUnique = make_unique<Wt::WHBoxLayout>();
	auto hLayout = vLayout->insertLayout(layoutMain::Filter, std::move(hLayoutUnique));

	auto wjFilterBoxUnique = make_unique<WJFILTERBOX>(sRef);
	auto wjFilterBox = hLayout->insertWidget(layoutFilter::FilterBox, std::move(wjFilterBoxUnique));

	auto wjCataListUnique = make_unique<WJCATALIST>();
	auto wjCataList = hLayout->insertWidget(layoutFilter::CataList, std::move(wjCataListUnique));
	wjCataList->displayCata().connect(this, bind(&SCDAwidget::populateCataInfo, this, placeholders::_1));

	auto wjCataInfoUnique = make_unique<WJCATAINFO>();
	auto wjCataInfo = hLayout->insertWidget(layoutFilter::CataInfo, std::move(wjCataInfoUnique));
	wjCataList->selectCata().connect(wjCataInfo, bind(&WJCATAINFO::selectCata, wjCataInfo, placeholders::_1));

}
void SCDAwidget::populateCataInfo(const int& cataIndex)
{
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(layoutMain::Filter);
	auto hLayout = (Wt::WHBoxLayout*)wlItem->layout();
	wlItem = hLayout->itemAt(layoutFilter::FilterBox);
	auto wjFilterBox = (WJFILTERBOX*)wlItem->widget();
	wlItem = hLayout->itemAt(layoutFilter::CataInfo);
	auto wjCataInfo = (WJCATAINFO*)wlItem->widget();

	auto it = wjFilterBox->setFiltered->begin();
	int overallIndex = *next(it, cataIndex);
	wjCataInfo->populate(wjFilterBox->vCata->at(overallIndex));
}
void SCDAwidget::populateCataList()
{
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(layoutMain::Filter);
	auto hLayout = (Wt::WHBoxLayout*)wlItem->layout();
	wlItem = hLayout->itemAt(layoutFilter::FilterBox);
	auto wjFilterBox = (WJFILTERBOX*)wlItem->widget();
	wlItem = hLayout->itemAt(layoutFilter::CataList);
	auto wjCataList = (WJCATALIST*)wlItem->widget();

	int numFiltered = (int)wjFilterBox->setFiltered->size();
	vector<string> vsCata(numFiltered);
	int index{ 0 };
	for (auto it = wjFilterBox->setFiltered->begin(); it != wjFilterBox->setFiltered->end(); ++it) {
		vsCata[index] = wjFilterBox->vCata->at(*it).name;
		index++;
	}
	wjCataList->setList(vsCata);
}
void SCDAwidget::processDataEvent(const DataEvent& event)
{
	switch (event.type()) {
	case DataEvent::CatalogueList:
	{
		auto vLayout = (Wt::WVBoxLayout*)this->layout();
		auto wlItem = vLayout->itemAt(layoutMain::Filter);
		auto hLayout = (Wt::WHBoxLayout*)wlItem->layout();
		wlItem = hLayout->itemAt(layoutFilter::FilterBox);
		auto wjFilterBox = (WJFILTERBOX*)wlItem->widget();
		if (wjFilterBox->vCata != nullptr) {
			wjFilterBox->vCata.reset();
		}
		wjFilterBox->vCata = make_shared<vector<WJCATA>>(event.getCataList());
		wjFilterBox->initFilter();
		populateCataList();
		break;
	}
	}

}
