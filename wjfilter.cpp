#include "wjfilter.h"

using namespace std;

WJFILTER::WJFILTER(string title) : WPanel()
{
	setTitle(title.c_str());
	setCollapsible(0);

	setCentralWidget(make_unique<Wt::WComboBox>());
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
	cb->clear();
	for (int ii = 0; ii <= numValue; ii++) {
		cb->addItem(vsFilter[ii]);
	}
}
void WJFILTER::setEnabled(bool trueFalse)
{
	Wt::WComboBox* cb = (Wt::WComboBox*)this->centralWidget();
	if (cb == nullptr) { return; }
	if (trueFalse) { cb->enable(); }
	else { cb->disable(); }
}

