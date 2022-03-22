#include "wjparambox.h"

using namespace std;

WJPARAMBOX::WJPARAMBOX()
{
	setStyleClass("wjparambox");
	initGUI();
}

void WJPARAMBOX::err(string message)
{
	string errorMessage = "WJPARAMBOX error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJPARAMBOX::initGUI()
{
	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	string title{ "Parameter: None" };
	auto paramUnique = make_unique<WJPARAM>();
	auto param = vLayout->addWidget(std::move(paramUnique));
	param->setTitle(title);
	param->setDisabled(1);

	vLayout->addStretch(1);
}
void WJPARAMBOX::initParamAll(const vector<vector<string>>& vvsDIM)
{
	// Make a WJPARAM widget for each parameter (not including row/column topics)
	// within the database DIM table.

	resetAll();
	int numParam = (int)vvsDIM.size() - 2;
	if (numParam > 0) {
		unique_ptr<WJPARAM> paramUnique;
		for (int ii = 0; ii < numParam; ii++) {
			paramUnique = make_unique<WJPARAM>();
		}
	}
}
void WJPARAMBOX::resetAll()
{
	// Note: the panel at index 0 is always kept (disabled).

	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	Wt::WLayoutItem* wlItem = nullptr;
	int numItem = vLayout->count();
	for (int ii = numItem - 2; ii > 0; ii--) {
		wlItem = vLayout->itemAt(ii);
		vLayout->removeItem(wlItem);
	}

	wlItem = vLayout->itemAt(0);
	auto param = (WJPARAM*)wlItem->widget();
	param->setDisabled(1);
}
