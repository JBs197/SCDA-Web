#include "SCDAwidget.h"

using namespace std;

SCDAwidget::SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver)
{
	sRef.connect(this, bind(&SCDAwidget::processDataEvent, this, placeholders::_1));
	
	initGUI();

	string sessionID = Wt::WApplication::instance()->sessionId();
	sRef.pullCataAll(sessionID);
}

void SCDAwidget::err(string message)
{
	string errorMessage = "SCDAwidget error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void SCDAwidget::initGUI()
{
	auto hLayoutUnique = make_unique<Wt::WHBoxLayout>();
	auto hLayout = this->setLayout(std::move(hLayoutUnique));

	auto wjFilterBoxUnique = make_unique<WJFILTERBOX>();
	auto wjFilterBox = hLayout->insertWidget(layoutMain::Filter, std::move(wjFilterBoxUnique));
	wjFilterBox->populateCataList().connect(this, bind(&SCDAwidget::populateCataList, this));

	auto tabBoxUnique = make_unique<Wt::WContainerWidget>();
	auto tabBox = hLayout->insertWidget(layoutMain::Tab, std::move(tabBoxUnique));
	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = tabBox->setLayout(std::move(vLayoutUnique));
	auto tabUnique = make_unique<Wt::WTabWidget>();
	auto tab = vLayout->addWidget(std::move(tabUnique));

	auto cataListUnique = make_unique<WJCATATAB>();
	auto wmItem = tab->insertTab(tabIndex::Catalogue, std::move(cataListUnique), "Catalogues");
	auto cataTab = (WJCATATAB*)wmItem->contents();
	cataTab->displayCata().connect(this, bind(&SCDAwidget::populateCataInfo, this, placeholders::_1));

	auto wjTreeUnique = make_unique<WJTREE>();
	wmItem = tab->insertTab(tabIndex::Tree, std::move(wjTreeUnique), "Geographic Regions");
	auto wjTree = (WJTREE*)wmItem->contents();

	auto wjTableUnique = make_unique<WJTABLE>();
	wmItem = tab->insertTab(tabIndex::Table, std::move(wjTableUnique), "Data Table");
	auto wjTable = (WJTABLE*)wmItem->contents();

	auto wjMapUnique = make_unique<WJMAP>();
	wmItem = tab->insertTab(tabIndex::Map, std::move(wjMapUnique), "Map");
	auto wjMap = (WJMAP*)wmItem->contents();

}
void SCDAwidget::populateCataInfo(const int& cataIndex)
{
	auto hLayout = (Wt::WHBoxLayout*)this->layout();
	auto wlItem = hLayout->itemAt(layoutMain::Filter);
	auto wjFilterBox = (WJFILTERBOX*)wlItem->widget();
	wlItem = hLayout->itemAt(layoutMain::Tab);
	auto tabBox = (Wt::WContainerWidget*)wlItem->widget();
	auto vLayout = (Wt::WVBoxLayout*)tabBox->layout();
	wlItem = vLayout->itemAt(0);
	auto tab = (Wt::WTabWidget*)wlItem->widget();
	auto wjCataTab = (WJCATATAB*)tab->widget(tabIndex::Catalogue);

	auto it = wjFilterBox->setPassed->begin();
	if (it != wjFilterBox->setPassed->end()) {
		int overallIndex = *next(it, cataIndex);
		wjCataTab->populateInfo(wjFilterBox->vCata->at(overallIndex));
	}
	else { wjCataTab->clearInfo(); }
}
void SCDAwidget::populateCataList()
{
	auto hLayout = (Wt::WHBoxLayout*)this->layout();
	auto wlItem = hLayout->itemAt(layoutMain::Filter);
	auto wjFilterBox = (WJFILTERBOX*)wlItem->widget();
	wlItem = hLayout->itemAt(layoutMain::Tab);
	auto tabBox = (Wt::WContainerWidget*)wlItem->widget();
	auto vLayout = (Wt::WVBoxLayout*)tabBox->layout();
	wlItem = vLayout->itemAt(0);
	auto tab = (Wt::WTabWidget*)wlItem->widget();
	auto wjCataList = (WJCATATAB*)tab->widget(tabIndex::Catalogue);

	int numFiltered = (int)wjFilterBox->setPassed->size();
	vector<string> vsCata(numFiltered);
	int index{ 0 };
	for (auto it = wjFilterBox->setPassed->begin(); it != wjFilterBox->setPassed->end(); ++it) {
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
		auto hLayout = (Wt::WHBoxLayout*)this->layout();
		auto wlItem = hLayout->itemAt(layoutMain::Filter);
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
void SCDAwidget::pullCatalogue(const string& sCata)
{
	//sRef.pullCatalogue(sCata);
}
