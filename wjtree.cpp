#include "wjtree.h"

using namespace std;

WJTREE::WJTREE() : WContainerWidget()
{
	initGUI();

	jt = make_shared<JTREE>();
}

void WJTREE::err(string message)
{
	string errorMessage = "WJTREE error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJTREE::initGUI()
{
	setOverflow(Wt::Overflow::Auto, Wt::Orientation::Horizontal);

	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	auto treeUnique = make_unique<Wt::WTree>();
	auto tree = vLayout->addWidget(std::move(treeUnique));
	tree->setObjectName("Tree");
	tree->setSelectionMode(Wt::SelectionMode::Single);

	vLayout->addStretch(1);
}
