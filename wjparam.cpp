#include "wjparam.h"

using namespace std;

WJPARAM::WJPARAM()
{
	Wt::WPanel::setStyleClass("wjparam");
	Wt::WPanel::setCollapsible(0);

	WJPANEL::setLabel("");
	WJPANEL::setStyleLabel("wjparamlabel");
	WJPANEL::setStyleButton("wjparambutton");
	WJPANEL::setStylePopup("wjparampopup");

	jtMID = make_shared<JTREE>();
}

void WJPARAM::clearList(Wt::WPopupMenu* popup)
{
	if (popup == nullptr) {
		auto box = (Wt::WContainerWidget*)this->centralWidget();
		auto hLayout = (Wt::WHBoxLayout*)box->layout();
		auto wlItem = hLayout->itemAt(index::Button);
		auto pb = (Wt::WPushButton*)wlItem->widget();
		popup = pb->menu();
	}
	Wt::WMenuItem* wmItem = nullptr;
	int numItem = popup->count();
	for (int ii = numItem - 1; ii >= 0; ii--) {
		wmItem = popup->itemAt(ii);
		popup->removeItem(wmItem);
	}
}
void WJPARAM::err(string message)
{
	string errorMessage = "WJPARAM error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJPARAM::populate()
{
	// Use the local JTREE to populate the panel button's popup menu. 
	
	auto box = (Wt::WContainerWidget*)this->centralWidget();
	auto hLayout = (Wt::WHBoxLayout*)box->layout();
	auto wlItem = hLayout->itemAt(index::Button);
	auto pb = (Wt::WPushButton*)wlItem->widget();
	auto popup = pb->menu();
	clearList(popup);

	JNODE& jnRoot = jtMID->getRoot();
	populateTree(jtMID.get(), jnRoot.ID, popup);
}
void WJPARAM::populateTree(JTREE* jt, int parentID, Wt::WPopupMenu* popup)
{
	// Recursive function, used to populate a WPopupMenu with the contents of JTREE.
	Wt::WString wsTemp;
	Wt::WMenuItem* child = nullptr;
	vector<int> childrenID = jt->getChildrenID(parentID);
	int numChildren = (int)childrenID.size();
	for (int ii = 0; ii < numChildren; ii++) {
		JNODE& jn = jt->getNode(childrenID[ii]);
		wsTemp = Wt::WString::fromUTF8(jn.vsData[0]);
		auto childUnique = make_unique<Wt::WMenuItem>(wsTemp, nullptr, Wt::ContentLoading::Eager);
		child = popup->addItem(std::move(childUnique));

		populateTree(jt, jn.ID, popup);
	}
}
void WJPARAM::setTitle(string& title)
{
	Wt::WPanel::setTitle(title);
}
