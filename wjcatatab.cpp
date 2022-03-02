#include "wjcatatab.h"

using namespace std;

WJCATATAB::WJCATATAB() : WContainerWidget(), numCata(-1)
{
	initGrid();
	initGUI();
}

void WJCATATAB::addItem(string& sCata)
{
	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	auto wlItem = gLayout->itemAtPosition(mapGrid.at("filteredList"));
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto boxLayout = (Wt::WVBoxLayout*)box->layout();
	wlItem = boxLayout->itemAt(0);
	auto filteredList = (Wt::WTable*)wlItem->widget();
	filteredList->elementAt(numCata, 0)->addNew<Wt::WText>(sCata);
	filteredList->elementAt(numCata, 0)->setStyleClass("tablecell highlighted normal");
	filteredList->elementAt(numCata, 0)->toggleStyleClass("highlighted", 0);
	filteredList->elementAt(numCata, 0)->mouseWentOver().connect(this, bind(&WJCATATAB::highlightRow, this, numCata));
	filteredList->elementAt(numCata, 0)->mouseWentOut().connect(this, bind(&WJCATATAB::unhighlightRow, this, numCata));
	numCata++;
	updateFilteredLabel();
}
void WJCATATAB::clearInfo()
{
	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	auto wlItem = gLayout->itemAtPosition(mapGrid.at("cataInfo"));
	auto wjCataInfo = (WJCATAINFO*)wlItem->widget();
	wjCataInfo->depopulate();
}
void WJCATATAB::highlightRow(const int& iRow)
{
	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	auto wlItem = gLayout->itemAtPosition(mapGrid.at("filteredList"));
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto boxLayout = (Wt::WVBoxLayout*)box->layout();
	wlItem = boxLayout->itemAt(0);
	auto filteredList = (Wt::WTable*)wlItem->widget();
	filteredList->elementAt(iRow, 0)->toggleStyleClass("highlighted", 1, 1);
	filteredList->elementAt(iRow, 0)->toggleStyleClass("normal", 0, 1);
	displayCata_.emit(iRow);
}
void WJCATATAB::initGrid()
{
	mapGrid.emplace("filteredLabel", make_pair(0, 0));
	mapGrid.emplace("cataLabel", make_pair(0, 1));
	mapGrid.emplace("filteredList", make_pair(1, 0));
	mapGrid.emplace("cataInfo", make_pair(1, 1));
}
void WJCATATAB::initGUI()
{
	setStyleClass("wjcatatab");
	setOverflow(Wt::Overflow::Hidden, Wt::Orientation::Vertical | Wt::Orientation::Horizontal);

	auto gLayoutUnique = make_unique<WJGRIDLAYOUT>();
	auto gLayout = this->setLayout(std::move(gLayoutUnique));
	gLayout->setColumnStretch(0, 1);
	gLayout->setColumnStretch(1, 5);

	auto filteredLabelUnique = make_unique<Wt::WText>("X catalogues satisfy the chosen filters.");
	auto filteredLabel = gLayout->addWidget(std::move(filteredLabelUnique), get<0>(mapGrid.at("filteredLabel")), get<1>(mapGrid.at("filteredLabel")));
	filteredLabel->setStyleClass("textlabel");
	filteredLabel->setWordWrap(1);

	auto cataLabelUnique = make_unique<Wt::WText>("Selected Catalogue:");
	auto cataLabel = gLayout->addWidget(std::move(cataLabelUnique), get<0>(mapGrid.at("cataLabel")), get<1>(mapGrid.at("cataLabel")));
	cataLabel->setStyleClass("textlabel");
	cataLabel->setWordWrap(1);

	auto filteredListBoxUnique = make_unique<Wt::WContainerWidget>();
	auto filteredListBox = gLayout->addWidget(std::move(filteredListBoxUnique), get<0>(mapGrid.at("filteredList")), get<1>(mapGrid.at("filteredList")));
	filteredListBox->setStyleClass("wjcatainfo");
	filteredListBox->setOverflow(Wt::Overflow::Auto, Wt::Orientation::Vertical);
	auto boxLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto boxLayout = filteredListBox->setLayout(std::move(boxLayoutUnique));
	auto filteredListUnique = make_unique<Wt::WTable>();
	auto filteredList = boxLayout->addWidget(std::move(filteredListUnique));

	auto cataInfoUnique = make_unique<WJCATAINFO>();
	auto cataInfo = gLayout->addWidget(std::move(cataInfoUnique), get<0>(mapGrid.at("cataInfo")), get<1>(mapGrid.at("cataInfo")));
	cataInfo->setOverflow(Wt::Overflow::Auto, Wt::Orientation::Vertical | Wt::Orientation::Horizontal);
}
void WJCATATAB::itemClicked(const std::string& sCata)
{
	selectCata_.emit(sCata);

	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	auto wlItem = gLayout->itemAtPosition(mapGrid.at("cataLabel"));
	auto label = (Wt::WText*)wlItem->widget();
	label->setText("Selected Catalogue: " + sCata);
}
void WJCATATAB::resetList()
{
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::List);
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto boxLayout = (Wt::WVBoxLayout*)box->layout();
	wlItem = boxLayout->itemAt(0);
	auto table = (Wt::WTable*)wlItem->widget();
	table->clear();
	numCata = 0;
}
void WJCATATAB::setList(vector<string>& vsCata)
{
	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	auto wlItem = gLayout->itemAtPosition(mapGrid.at("filteredList"));
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto boxLayout = (Wt::WVBoxLayout*)box->layout();
	wlItem = boxLayout->itemAt(0);
	auto filteredList = (Wt::WTable*)wlItem->widget();
	filteredList->clear();

	numCata = (int)vsCata.size();
	for (int ii = 0; ii < numCata; ii++) {
		filteredList->elementAt(ii, 0)->addNew<Wt::WText>(vsCata[ii]);
		filteredList->elementAt(ii, 0)->setStyleClass("tablecell highlighted normal");
		filteredList->elementAt(ii, 0)->toggleStyleClass("highlighted", 0);
		filteredList->elementAt(ii, 0)->mouseWentOver().connect(this, bind(&WJCATATAB::highlightRow, this, ii));
		filteredList->elementAt(ii, 0)->mouseWentOut().connect(this, bind(&WJCATATAB::unhighlightRow, this, ii));
		filteredList->elementAt(ii, 0)->clicked().connect(this, bind(&WJCATATAB::itemClicked, this, vsCata[ii]));
	}
	updateFilteredLabel();
}
void WJCATATAB::populateInfo(WJCATA& wjCata)
{
	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	auto wlItem = gLayout->itemAtPosition(mapGrid.at("cataInfo"));
	auto wjCataInfo = (WJCATAINFO*)wlItem->widget();
	wjCataInfo->populate(wjCata);
}
void WJCATATAB::unhighlightRow(const int& iRow)
{
	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	auto wlItem = gLayout->itemAtPosition(mapGrid.at("filteredList"));
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto boxLayout = (Wt::WVBoxLayout*)box->layout();
	wlItem = boxLayout->itemAt(0);
	auto filteredList = (Wt::WTable*)wlItem->widget();
	filteredList->elementAt(iRow, 0)->toggleStyleClass("highlighted", 0, 1);
	filteredList->elementAt(iRow, 0)->toggleStyleClass("normal", 1, 1);
}
void WJCATATAB::updateFilteredLabel()
{
	string message = to_string(numCata);
	if (numCata == 1) {
		message += " catalogue satisfies the chosen filters.";
	}
	else { message += " catalogues satisfy the chosen filters."; }

	auto gLayout = (WJGRIDLAYOUT*)this->layout();
	auto wlItem = gLayout->itemAtPosition(mapGrid.at("filteredLabel"));
	auto label = (Wt::WText*)wlItem->widget();
	label->setText(message.c_str());
}
