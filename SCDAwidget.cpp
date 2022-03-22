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
	auto mainLayoutUnique = make_unique<Wt::WHBoxLayout>();
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
	sRef.getTreeInit(wjTree->defaultRowExpansion);

	auto wjTableUnique = make_unique<WJTABLEBOX>();
	wmItem = tab->insertTab(tabIndex::Table, std::move(wjTableUnique), "Data Table");
	auto wjTable = (WJTABLEBOX*)wmItem->contents();
	wjTable->configure(sRef.configXML);

	auto wjMapUnique = make_unique<WJMAP>();
	wmItem = tab->insertTab(tabIndex::Map, std::move(wjMapUnique), "Map");
	auto wjMap = (WJMAP*)wmItem->contents();

}
void SCDAwidget::processDataEvent(const DataEvent& event)
{
	vector<int> vStatus;
	size_t numThread{ 0 };
	pair<int, string> selRegion;

	auto mainLayout = (Wt::WHBoxLayout*)this->layout();
	auto wlItem = mainLayout->itemAt(layoutMain::Tab);
	auto tabBox = (Wt::WContainerWidget*)wlItem->widget();
	auto vLayout = (Wt::WVBoxLayout*)tabBox->layout();
	wlItem = vLayout->itemAt(0);
	auto tab = (Wt::WTabWidget*)wlItem->widget();
	wlItem = mainLayout->itemAt(layoutMain::Parameter);
	auto paramBox = (WJPARAMBOX*)wlItem->widget();

	switch (event.type()) {
	case DataEvent::CatalogueList:
	{
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
		auto wjTree = (WJTREE*)tab->widget(tabIndex::Tree);
		if (event.cataRet.vvsGeo.size() > 0) {		
			wjTree->setLabel(event.cataRet.sCata);
			wjTree->setTree(event.cataRet.vvsGeo);
		}
		selRegion = wjTree->getSelectedRegion();

		auto wjTable = (WJTABLEBOX*)tab->widget(tabIndex::Table);
		if (event.cataRet.vvsDIM.size() > 0) {
			wjTable->setTopic(event.cataRet.vvsDIM);
			paramBox->initParamAll(event.cataRet.vvsDIM);
		}
		if (event.cataRet.vvvsTable.size() > 0) {			
			wjTable->initTableAll(event.cataRet.vsMapGeo, event.cataRet.vvvsTable);
			wjTable->selectRegion(selRegion);
		}
		if (event.cataRet.vvvsMID.size() > 0) {
			wjTable->setHeader(event.cataRet.vvvsMID);
		}

		break;
	}
	}

	int count;
	numThread = (int)vStatus.size();
	if (numThread > 0) {
		while (1) {
			count = 0;
			for (int ii = 0; ii < numThread; ii++) {
				count += vStatus[ii];
			}
			if (count == numThread) { break; }
			this_thread::sleep_for(20ms);
		}
	}
	int bbq = 1;
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
