#include "wjcatalist.h"

using namespace std;

WJCATALIST::WJCATALIST() : WContainerWidget(), numCata(-1)
{
	setStyleClass("wjcatalist");

	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	auto labelUnique = make_unique<Wt::WText>("X catalogues satisfy the chosen filters.");
	auto label = vLayout->insertWidget(index::Label, std::move(labelUnique));
	label->setStyleClass("textlabel");
	label->setWordWrap(1);

	auto tableBoxUnique = make_unique<Wt::WContainerWidget>();
	auto tableBox = vLayout->insertWidget(index::List, std::move(tableBoxUnique));
	tableBox->setOverflow(Wt::Overflow::Auto, Wt::Orientation::Vertical);
	auto boxLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto boxLayout = tableBox->setLayout(std::move(boxLayoutUnique));
	auto tableUnique = make_unique<Wt::WTable>();
	auto table = boxLayout->addWidget(std::move(tableUnique));

}

void WJCATALIST::addItem(string& sCata)
{
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::List);
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto boxLayout = (Wt::WVBoxLayout*)box->layout();
	wlItem = boxLayout->itemAt(0);
	auto table = (Wt::WTable*)wlItem->widget();
	table->elementAt(numCata, 0)->addNew<Wt::WText>(sCata);
	table->elementAt(numCata, 0)->setStyleClass("tablecell highlighted normal");
	table->elementAt(numCata, 0)->toggleStyleClass("highlighted", 0);
	table->elementAt(numCata, 0)->mouseWentOver().connect(this, bind(&WJCATALIST::highlightRow, this, numCata));
	table->elementAt(numCata, 0)->mouseWentOut().connect(this, bind(&WJCATALIST::unhighlightRow, this, numCata));
	numCata++;
	updateLabel();
}
void WJCATALIST::highlightRow(const int& iRow)
{
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::List);
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto boxLayout = (Wt::WVBoxLayout*)box->layout();
	wlItem = boxLayout->itemAt(0);
	auto table = (Wt::WTable*)wlItem->widget();
	table->elementAt(iRow, 0)->toggleStyleClass("highlighted", 1, 1);
	table->elementAt(iRow, 0)->toggleStyleClass("normal", 0, 1);
	displayCata_.emit(iRow);
}
void WJCATALIST::itemClicked(const std::string& sCata)
{
	selectCata_.emit(sCata);
}
void WJCATALIST::resetList()
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
void WJCATALIST::setList(vector<string>& vsCata)
{
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::List);
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto boxLayout = (Wt::WVBoxLayout*)box->layout();
	wlItem = boxLayout->itemAt(0);
	auto table = (Wt::WTable*)wlItem->widget();
	table->clear();

	numCata = (int)vsCata.size();
	for (int ii = 0; ii < numCata; ii++) {
		table->elementAt(ii, 0)->addNew<Wt::WText>(vsCata[ii]);
		table->elementAt(ii, 0)->setStyleClass("tablecell highlighted normal");
		table->elementAt(ii, 0)->toggleStyleClass("highlighted", 0);
		table->elementAt(ii, 0)->mouseWentOver().connect(this, bind(&WJCATALIST::highlightRow, this, ii));
		table->elementAt(ii, 0)->mouseWentOut().connect(this, bind(&WJCATALIST::unhighlightRow, this, ii));
		table->elementAt(ii, 0)->clicked().connect(this, bind(&WJCATALIST::itemClicked, this, vsCata[ii]));
	}
	updateLabel();
}
void WJCATALIST::unhighlightRow(const int& iRow)
{
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::List);
	auto box = (Wt::WContainerWidget*)wlItem->widget();
	auto boxLayout = (Wt::WVBoxLayout*)box->layout();
	wlItem = boxLayout->itemAt(0);
	auto table = (Wt::WTable*)wlItem->widget();
	table->elementAt(iRow, 0)->toggleStyleClass("highlighted", 0, 1);
	table->elementAt(iRow, 0)->toggleStyleClass("normal", 1, 1);
}
void WJCATALIST::updateLabel()
{
	string message = to_string(numCata);
	if (numCata == 1) {
		message += " catalogue satisfies the chosen filters.";
	}
	else { message += " catalogues satisfy the chosen filters."; }

	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::Label);
	auto label = (Wt::WText*)wlItem->widget();
	label->setText(message.c_str());
}
