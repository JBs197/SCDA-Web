#include "wjcatalist.h"

using namespace std;

WJCATALIST::WJCATALIST() : WContainerWidget(), numCata(-1)
{
	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	auto labelUnique = make_unique<Wt::WLabel>("X catalogues satisfy the chosen filters.");
	auto label = vLayout->addWidget(std::move(labelUnique));
	label->setWordWrap(1);

	auto tableUnique = make_unique<Wt::WTable>();
	auto table = vLayout->addWidget(std::move(tableUnique));


}

void WJCATALIST::updateLabel()
{
	string message = to_string(numCata);
	if (numCata == 1) {
		message += " catalogue satisfies the chosen filters.";
	}
	else { message += " catalogues satisfy the chosen filters."; }

	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(0);
	auto label = (Wt::WLabel*)wlItem->widget();
	label->setText(message.c_str());
}
