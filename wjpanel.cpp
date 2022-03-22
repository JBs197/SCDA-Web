#include "wjpanel.h"

using namespace std;

WJPANEL::WJPANEL()
{
	auto boxUnique = make_unique<Wt::WContainerWidget>();
	auto box = setCentralWidget(std::move(boxUnique));
	auto hLayoutUnique = make_unique<Wt::WHBoxLayout>();
	auto hLayout = box->setLayout(std::move(hLayoutUnique));

	auto labelUnique = make_unique<Wt::WText>("");
	auto label = hLayout->insertWidget(index::Label, std::move(labelUnique));

	hLayout->insertStretch(index::Stretch, 1);

	auto pbUnique = make_unique<Wt::WPushButton>();
	auto pb = hLayout->insertWidget(index::Button, std::move(pbUnique));

	auto popup = make_unique<Wt::WPopupMenu>();	
	pb->setMenu(std::move(popup));
}

void WJPANEL::err(string message)
{
	string errorMessage = "WJPANEL error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJPANEL::setLabel(string label)
{
	auto box = (Wt::WContainerWidget*)this->centralWidget();
	auto hLayout = (Wt::WHBoxLayout*)box->layout();
	auto wlItem = hLayout->itemAt(index::Label);
	auto wLabel = (Wt::WText*)wlItem->widget();
	wLabel->setText(label);
}
void WJPANEL::setStyleButton(string cssClass)
{
	auto box = (Wt::WContainerWidget*)this->centralWidget();
	auto hLayout = (Wt::WHBoxLayout*)box->layout();
	auto wlItem = hLayout->itemAt(index::Button);
	auto pb = (Wt::WPushButton*)wlItem->widget();
	pb->setStyleClass(cssClass);
}
void WJPANEL::setStyleLabel(string cssClass)
{
	auto box = (Wt::WContainerWidget*)this->centralWidget();
	auto hLayout = (Wt::WHBoxLayout*)box->layout();
	auto wlItem = hLayout->itemAt(index::Label);
	auto wLabel = (Wt::WText*)wlItem->widget();
	wLabel->setStyleClass(cssClass);
}
void WJPANEL::setStylePopup(string cssClass)
{
	auto box = (Wt::WContainerWidget*)this->centralWidget();
	auto hLayout = (Wt::WHBoxLayout*)box->layout();
	auto wlItem = hLayout->itemAt(index::Button);
	auto pb = (Wt::WPushButton*)wlItem->widget();
	auto popup = pb->menu();
	popup->setStyleClass(cssClass);
}
void WJPANEL::setStyleTitle(string cssClass)
{
	auto title = Wt::WPanel::titleBarWidget();
	title->setStyleClass(cssClass);
}
