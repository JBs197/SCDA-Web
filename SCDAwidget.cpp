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
	auto mainLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto mainLayout = this->setLayout(std::move(mainLayoutUnique));

	auto wjParamBoxUnique = make_unique<WJPARAMBOX>();
	auto wjParamBox = mainLayout->insertWidget(layoutMain::Parameter, std::move(wjParamBoxUnique));

	auto tabBoxUnique = make_unique<Wt::WContainerWidget>();
	auto tabBox = mainLayout->insertWidget(layoutMain::Tab, std::move(tabBoxUnique));
	auto tabLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto tabLayout = tabBox->setLayout(std::move(tabLayoutUnique));
	auto tabUnique = make_unique<Wt::WTabWidget>();
	auto tab = tabLayout->addWidget(std::move(tabUnique));

	auto cataListUnique = make_unique<WJCATATAB>();
	auto wmItem = tab->insertTab(tabIndex::Catalogue, std::move(cataListUnique), "Catalogues");
	auto cataTab = (WJCATATAB*)wmItem->contents();
	cataTab->selectCata().connect(this, bind(&SCDAwidget::pullCatalogueFirst, this, placeholders::_1, placeholders::_2));

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
void SCDAwidget::processDataEvent(const DataEvent& event)
{
	switch (event.type()) {
	case DataEvent::CatalogueList:
	{
		auto mainLayout = (Wt::WVBoxLayout*)this->layout();
		auto wlItem = mainLayout->itemAt(layoutMain::Tab);
		auto tabBox = (Wt::WContainerWidget*)wlItem->widget();
		auto vLayout = (Wt::WVBoxLayout*)tabBox->layout();
		wlItem = vLayout->itemAt(0);
		auto tab = (Wt::WTabWidget*)wlItem->widget();
		auto cataTab = (WJCATATAB*)tab->widget(tabIndex::Catalogue);
		auto gLayout = (WJGRIDLAYOUT*)cataTab->layout();
		wlItem = gLayout->itemAtPosition(cataTab->mapGrid.at("filterbox"));
		auto filterBox = (WJFILTERBOX*)wlItem->widget();
		filterBox->initCataAll(event.getCataList());
		filterBox->initFilter();

		cataTab->populateCataList();
		break;
	}
	case DataEvent::Data:
	{

		break;
	}
	}

}
void SCDAwidget::pullCatalogueFirst(const string& sYear, const string& sCata)
{
	// Send a request to the server for a new catalogue's data, using default
	// DIMs and the root geographic region as the parent region.
	string sessionID = Wt::WApplication::instance()->sessionId();
	CataRequest cataReq;
	cataReq.sYear = sYear;
	cataReq.sCata = sCata;
	sRef.pullCatalogue(sessionID, cataReq);
}
