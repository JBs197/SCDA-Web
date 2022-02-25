#include "wjcatainfo.h"

using namespace std;

WJCATAINFO::WJCATAINFO() : WContainerWidget()
{
	setStyleClass("wjcatainfo");
	initGUI();
}

void WJCATAINFO::initGUI()
{
	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	auto displayBoxUnique = make_unique<Wt::WContainerWidget>();
	auto displayBox = vLayout->addWidget(std::move(displayBoxUnique), 1);
	displayBox->setOverflow(Wt::Overflow::Auto, Wt::Orientation::Vertical);
	auto gLayoutUnique = make_unique<WJGRIDLAYOUT>();
	auto gLayout = displayBox->setLayout(std::move(gLayoutUnique));
	gLayout->setColumnStretch(0, 1);
	gLayout->setColumnStretch(1, 3);

	auto label = make_unique<Wt::WText>("Name:");
	gLayout->addWidget(std::move(label), displayIndex::Name, 0);
	label = make_unique<Wt::WText>("Description:");
	gLayout->addWidget(std::move(label), displayIndex::Description, 0);
	label = make_unique<Wt::WText>("Year:");
	gLayout->addWidget(std::move(label), displayIndex::Year, 0);
	label = make_unique<Wt::WText>("Category:");
	gLayout->addWidget(std::move(label), displayIndex::Category, 0);
	label = make_unique<Wt::WText>("Row Topic:");
	gLayout->addWidget(std::move(label), displayIndex::RowTopic, 0);
	label = make_unique<Wt::WText>("Column Topic:");
	gLayout->addWidget(std::move(label), displayIndex::ColTopic, 0);
	label = make_unique<Wt::WText>("Parameters:");
	gLayout->addWidget(std::move(label), displayIndex::Parameter, 0);
	for (int ii = 0; ii <= displayIndex::Parameter; ii++) {
		label = make_unique<Wt::WText>(" ");
		gLayout->addWidget(std::move(label), ii, 1);
	}

	auto selBoxUnique = make_unique<Wt::WContainerWidget>();
	auto selBox = vLayout->addWidget(std::move(selBoxUnique), 0);
	auto hLayoutUnique = make_unique<Wt::WHBoxLayout>();
	auto hLayout = selBox->setLayout(std::move(hLayoutUnique));
	label = make_unique<Wt::WText>("Selected Catalogue:");
	hLayout->addWidget(std::move(label));
	label = make_unique<Wt::WText>(" ");
	hLayout->addWidget(std::move(label));
	hLayout->addStretch(1);
}
void WJCATAINFO::populate(WJCATA& wjCata)
{
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::Display);
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto gLayout = (WJGRIDLAYOUT*)box->layout();

	wlItem = gLayout->itemAtPosition(displayIndex::Name, 1);
	auto text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.name);

	wlItem = gLayout->itemAtPosition(displayIndex::Description, 1);
	text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.desc);

	wlItem = gLayout->itemAtPosition(displayIndex::Year, 1);
	text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.year);

	wlItem = gLayout->itemAtPosition(displayIndex::Category, 1);
	text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.category);

	wlItem = gLayout->itemAtPosition(displayIndex::RowTopic, 1);
	text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.rowTopic);

	wlItem = gLayout->itemAtPosition(displayIndex::ColTopic, 1);
	text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.colTopic);

	int numParam = (int)wjCata.vParameter.size();
	int index = displayIndex::Parameter;
	for (int ii = 0; ii < numParam; ii++) {
		wlItem = gLayout->itemAtPosition(index, 1);
		if (wlItem == nullptr) {
			auto textUnique = make_unique<Wt::WText>(wjCata.vParameter[ii]);
			gLayout->addWidget(std::move(textUnique), displayIndex::Parameter + ii, 1);
		}
		else {
			text = (Wt::WText*)wlItem->widget();
			text->setText(wjCata.vParameter[ii]);
		}
		index++;
	}

	wlItem = gLayout->itemAtPosition(index, 1);
	while (wlItem != nullptr) {
		gLayout->removeItem(wlItem);
		index++;
		wlItem = gLayout->itemAtPosition(index, 1);
	}
}
void WJCATAINFO::selectCata(const string& sCata)
{
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::Selection);
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto hLayout = (Wt::WHBoxLayout*)box->layout();
	wlItem = hLayout->itemAt(1);
	auto text = (Wt::WText*)wlItem->widget();
	text->setText(sCata);
}
