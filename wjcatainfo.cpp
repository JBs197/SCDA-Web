#include "wjcatainfo.h"

using namespace std;

WJCATAINFO::WJCATAINFO() : WContainerWidget()
{
	initGUI();
}

void WJCATAINFO::depopulate()
{
	int index{ 0 };
	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	auto wlItem = gLayout->itemAtPosition(index, 1);
	Wt::WText* text = nullptr;
	while (wlItem != nullptr) {		
		if (index <= index::Parameter) {
			text = (Wt::WText*)wlItem->widget();
			text->setText(" ");
		}
		else { gLayout->removeItem(wlItem); }
		index++;
		wlItem = gLayout->itemAtPosition(index, 1);
	}
}
void WJCATAINFO::initGUI()
{
	setStyleClass("wjcatainfo");
	setOverflow(Wt::Overflow::Auto, Wt::Orientation::Vertical);
	
	auto gLayoutUnique = make_unique<WJGRIDLAYOUT>();
	auto gLayout = setLayout(std::move(gLayoutUnique));
	gLayout->setColumnStretch(0, 1);
	gLayout->setColumnStretch(1, 4);

	auto label = make_unique<Wt::WText>("Name:");
	gLayout->addWidget(std::move(label), index::Name, 0);
	label = make_unique<Wt::WText>("Description:");
	gLayout->addWidget(std::move(label), index::Description, 0);
	label = make_unique<Wt::WText>("Year:");
	gLayout->addWidget(std::move(label), index::Year, 0);
	label = make_unique<Wt::WText>("Category:");
	gLayout->addWidget(std::move(label), index::Category, 0);
	label = make_unique<Wt::WText>("Row Topic:");
	gLayout->addWidget(std::move(label), index::RowTopic, 0);
	label = make_unique<Wt::WText>("Column Topic:");
	gLayout->addWidget(std::move(label), index::ColTopic, 0);
	label = make_unique<Wt::WText>("Parameters:");
	gLayout->addWidget(std::move(label), index::Parameter, 0);
	
	//unique_ptr<Wt::WContainerWidget> boxUnique;
	//Wt::WContainerWidget* box = nullptr;
	//unique_ptr<Wt::WVBoxLayout> vLayoutUnique;
	//Wt::WVBoxLayout* vLayout = nullptr;
	for (int ii = 0; ii <= index::Parameter; ii++) {
		//boxUnique = make_unique<Wt::WContainerWidget>();
		//box = gLayout->addWidget(std::move(boxUnique), ii, 1);
		//box->setOverflow(Wt::Overflow::Auto, Wt::Orientation::Vertical);
		//vLayoutUnique = make_unique<Wt::WVBoxLayout>();
		//vLayout = box->setLayout(std::move(vLayoutUnique));
		label = make_unique<Wt::WText>(" ");
		label->setWordWrap(1);
		
		//vLayout->addWidget(std::move(label));
		gLayout->addWidget(std::move(label), ii, 1);
	}
}
void WJCATAINFO::populate(WJCATA& wjCata)
{
	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	auto wlItem = gLayout->itemAtPosition(index::Name, 1);
	auto text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.name);

	wlItem = gLayout->itemAtPosition(index::Description, 1);
	text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.desc);

	wlItem = gLayout->itemAtPosition(index::Year, 1);
	text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.year);

	wlItem = gLayout->itemAtPosition(index::Category, 1);
	text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.category);

	wlItem = gLayout->itemAtPosition(index::RowTopic, 1);
	text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.rowTopic);

	wlItem = gLayout->itemAtPosition(index::ColTopic, 1);
	text = (Wt::WText*)wlItem->widget();
	text->setText(wjCata.colTopic);

	int numParam = (int)wjCata.vParameter.size();
	int index = index::Parameter;
	for (int ii = 0; ii < numParam; ii++) {
		wlItem = gLayout->itemAtPosition(index, 1);
		if (wlItem == nullptr) {
			auto textUnique = make_unique<Wt::WText>(wjCata.vParameter[ii]);
			gLayout->addWidget(std::move(textUnique), index::Parameter + ii, 1);
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
