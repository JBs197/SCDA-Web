#include "SCDAwidget.h"

using namespace std;

SCDAwidget::SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver)
{
	initGUI();
	initSignalSlot();

	string sessionID = Wt::WApplication::instance()->sessionId();
	sRef.pullCataAll(sessionID);
}

void SCDAwidget::initGUI()
{
	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	auto hLayoutUnique = make_unique<Wt::WHBoxLayout>();
	auto hLayout = vLayout->addLayout(std::move(hLayoutUnique));

	auto wjFilterBoxUnique = make_unique<WJFILTERBOX>(sRef);
	auto wjFilterBox = hLayout->addWidget(std::move(wjFilterBoxUnique));

	auto wjCataListUnique = make_unique<WJCATALIST>();
	auto wjCataList = hLayout->addWidget(std::move(wjCataListUnique));
}
void SCDAwidget::initSignalSlot()
{
	if (sRef.connect(this, bind(&SCDAwidget::processDataEvent, this, placeholders::_1))) {
		//
	}
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
		break;
	}
	}

}
