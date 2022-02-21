#include "SCDAwidget.h"

using namespace std;

SCDAwidget::SCDAwidget(SCDAserver& myserver) : WContainerWidget(), sRef(myserver)
{
	initGUI();
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
