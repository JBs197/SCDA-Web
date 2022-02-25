#include "wjfilter.h"

using namespace std;

WJFILTER::WJFILTER(string title) : WPanel()
{
	setStyleClass("wjfilter");
	setTitle(title.c_str());
	setCollapsible(0);

	auto cb = setCentralWidget(make_unique<Wt::WComboBox>());
	cb->activated().connect(this, bind(&WJFILTER::selectionChanged, this, placeholders::_1));
	
	goDeaf = 0;
}

void WJFILTER::getSelected(int& selIndex, string& selValue)
{
	auto cb = (Wt::WComboBox*)centralWidget();
	selIndex = cb->currentIndex();
	const Wt::WString wsTemp = cb->currentText();
	selValue = wsTemp.toUTF8();
}
void WJFILTER::initValue(vector<string>& vsValue)
{
	// vsValue should already be sorted.
	int numValue = (int)vsValue.size();
	vsFilter.resize(numValue + 1);
	vsFilter[0] = "All";
	for (int ii = 0; ii < numValue; ii++) {
		vsFilter[1 + ii] = std::move(vsValue[ii]);
	}

	auto cb = (Wt::WComboBox*)this->centralWidget();
	goDeaf = 1;
	cb->clear();
	for (int ii = 0; ii <= numValue; ii++) {
		cb->addItem(vsFilter[ii]);
	}
	cb->setCurrentIndex(0);
	goDeaf = 0;
}
void WJFILTER::selectionChanged(const int& selIndex)
{
	if (!goDeaf) { updateFilter_.emit(); }
}
void WJFILTER::setEnabled(bool trueFalse)
{
	Wt::WComboBox* cb = (Wt::WComboBox*)this->centralWidget();
	if (cb == nullptr) { return; }
	if (trueFalse) { cb->enable(); }
	else { cb->disable(); }
}

