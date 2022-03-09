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
void WJPARAMBOX::resetAll()
{
	// Note: the panel at index 0 is always kept (invisible) to act as a template.

	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	Wt::WLayoutItem* wlItem = nullptr;
	int numItem = vLayout->count();
	for (int ii = numItem - 2; ii > 0; ii--) {
		wlItem = vLayout->itemAt(ii);
		vLayout->removeItem(wlItem);
	}
}
