#include "wjfilter.h"

using namespace std;

WJFILTER::WJFILTER(string title) : WPanel()
{
	initGUI(title);
	
	goDeaf = 0;
}

void WJFILTER::clearList(Wt::WPopupMenu* popup)
{
	if (popup == nullptr) {
		auto box = (Wt::WContainerWidget*)this->centralWidget();
		auto hLayout = (Wt::WHBoxLayout*)box->layout();
		auto wlItem = hLayout->itemAt(index::Button);
		auto pb = (Wt::WPushButton*)wlItem->widget();
		popup = pb->menu();
	}
	Wt::WMenuItem* wmItem = nullptr;
	int numFilter = popup->count();
	for (int ii = numFilter - 1; ii >= 0; ii--) {
		wmItem = popup->itemAt(ii);
		popup->removeItem(wmItem);
	}
}
void WJFILTER::err(string message)
{
	string errorMessage = "WJFILTER error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
string WJFILTER::getSelected()
{
	auto box = (Wt::WContainerWidget*)this->centralWidget();
	auto hLayout = (Wt::WHBoxLayout*)box->layout();
	auto wlItem = hLayout->itemAt(index::Label);
	auto label = (Wt::WText*)wlItem->widget();
	const Wt::WString& wsTemp = label->text();
	return wsTemp.toUTF8();
}
void WJFILTER::highlightItem(const int& liveDead, const int& index)
{
	auto box = (Wt::WContainerWidget*)this->centralWidget();
	auto hLayout = (Wt::WHBoxLayout*)box->layout();
	auto wlItem = hLayout->itemAt(index::Button);
	auto pb = (Wt::WPushButton*)wlItem->widget();
	auto popup = pb->menu();
	Wt::WMenuItem* popupItem = nullptr;

	popupItem = popup->itemAt(index);
	if (liveDead == 0) {
		popupItem->toggleStyleClass("itemlive", 0, 1);
		popupItem->toggleStyleClass("itemlivehighlighted", 1, 1);
	}
	else if (liveDead == 1) {
		popupItem->toggleStyleClass("itemdead", 0, 1);
		popupItem->toggleStyleClass("itemdeadhighlighted", 1, 1);
	}
}
void WJFILTER::initGUI(std::string& title)
{
	setStyleClass("wjfilter");
	setTitle(title);
	setCollapsible(0);

	auto boxUnique = make_unique<Wt::WContainerWidget>();
	auto box = setCentralWidget(std::move(boxUnique));
	auto hLayoutUnique = make_unique<Wt::WHBoxLayout>();
	auto hLayout = box->setLayout(std::move(hLayoutUnique));

	auto labelUnique = make_unique<Wt::WText>("All");
	auto label = hLayout->insertWidget(index::Label, std::move(labelUnique));
	label->setStyleClass("wjfilterlabel");

	hLayout->insertStretch(index::Stretch, 1);

	auto pbUnique = make_unique<Wt::WPushButton>();
	auto pb = hLayout->insertWidget(index::Button, std::move(pbUnique));
	pb->setStyleClass("pbfilter");

	auto popup = make_unique<Wt::WPopupMenu>();
	popup->setStyleClass("wjfilterlist");
	pb->setMenu(std::move(popup));
}
void WJFILTER::initList(vector<string>& vsLive, vector<string>& vsDead)
{
	// DIM lists should already be alphabetically sorted.
	auto box = (Wt::WContainerWidget*)this->centralWidget();
	auto hLayout = (Wt::WHBoxLayout*)box->layout();
	auto wlItem = hLayout->itemAt(index::Button);
	auto pb = (Wt::WPushButton*)wlItem->widget();
	auto popup = pb->menu();
	Wt::WMenuItem* wmItem = nullptr;

	// Ensure that the special filter "All" is at the top of its appropriate list, 
	// whether that be live or dead.
	int numLive = (int)vsLive.size();
	int numDead = (int)vsDead.size();
	int index{ -1 };
	for (int ii = 0; ii < numLive; ii++) {
		if (vsLive[ii] == "All") {
			index = ii;
			for (int jj = ii - 1; jj >= 0; jj--) {
				vsLive[jj].swap(vsLive[jj + 1]);
			}
			break;
		}
	}
	if (index < 0) {
		for (int ii = 0; ii < numDead; ii++) {
			if (vsDead[ii] == "All") {
				index = ii;
				for (int jj = ii - 1; jj >= 0; jj--) {
					vsDead[jj].swap(vsDead[jj + 1]);
				}
				break;
			}
		}
	}
	if (index < 0) { err("Failed to locate wildcard filter-initList"); }

	goDeaf = 1;
	clearList(popup);
	index = 0;
	for (int ii = 0; ii < numLive; ii++) {
		wmItem = popup->addItem(vsLive[ii], nullptr, Wt::ContentLoading::Eager);
		wmItem->setStyleClass("wjfilterlistitem itemlive itemlivehighlighted");
		wmItem->toggleStyleClass("itemlivehighlighted", 0);
		wmItem->clicked().connect(this, bind(&WJFILTER::selectionChanged, this, vsLive[ii]));
		wmItem->mouseWentOut().connect(this, bind(&WJFILTER::unhighlightItem, this, 0, index));
		wmItem->mouseWentOver().connect(this, bind(&WJFILTER::highlightItem, this, 0, index));
		index++;
	}
	if (numDead > 0) {
		wmItem = popup->addSeparator();
		index++;

		for (int ii = 0; ii < numDead; ii++) {
			wmItem = popup->addItem(vsDead[ii], nullptr, Wt::ContentLoading::Eager);
			wmItem->setStyleClass("wjfilterlistitem itemdead itemdeadhighlighted");
			wmItem->toggleStyleClass("itemdeadhighlighted", 0);
			wmItem->clicked().connect(this, bind(&WJFILTER::selectionChanged, this, vsDead[ii]));
			wmItem->mouseWentOut().connect(this, bind(&WJFILTER::unhighlightItem, this, 1, index));
			wmItem->mouseWentOver().connect(this, bind(&WJFILTER::highlightItem, this, 1, index));
			index++;
		}
	}
	goDeaf = 0;
}
void WJFILTER::selectionChanged(const std::string& text)
{
	if (!goDeaf) { 
		updateLabel(text);
		updateFilter_.emit();
	}
}
void WJFILTER::setEnabled(bool trueFalse)
{
	auto box = (Wt::WContainerWidget*)this->centralWidget();
	if (trueFalse) { box->enable(); }
	else { box->disable(); }
}
void WJFILTER::unhighlightItem(const int& liveDead, const int& index)
{
	auto box = (Wt::WContainerWidget*)this->centralWidget();
	auto hLayout = (Wt::WHBoxLayout*)box->layout();
	auto wlItem = hLayout->itemAt(index::Button);
	auto pb = (Wt::WPushButton*)wlItem->widget();
	auto popup = pb->menu();
	auto popupItem = popup->itemAt(index);
	if (liveDead == 0) {
		popupItem->toggleStyleClass("itemlivehighlighted", 0, 1);
		popupItem->toggleStyleClass("itemlive", 1, 1);
	}
	else if (liveDead == 1) {
		popupItem->toggleStyleClass("itemdeadhighlighted", 0, 1);
		popupItem->toggleStyleClass("itemdead", 1, 1);
	}
}
void WJFILTER::updateLabel(string text)
{
	auto box = (Wt::WContainerWidget*)this->centralWidget();
	auto hLayout = (Wt::WHBoxLayout*)box->layout();
	auto wlItem = hLayout->itemAt(index::Label);
	auto label = (Wt::WText*)wlItem->widget();
	label->setText(text);
}
