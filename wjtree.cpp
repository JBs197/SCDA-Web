#include "wjtree.h"

void WJTREE::err(string message)
{
	string errorMessage = "WJTREE error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJTREE::init(int width, int height)
{
	wlAuto = Wt::WLength::Auto;
	maxWidth = Wt::WLength(width - 10);
	maxHeight = Wt::WLength(height);
	this->setMaximumSize(maxWidth, maxHeight);
	this->setOverflow(Wt::Overflow::Hidden, Wt::Orientation::Horizontal);

	auto vLayout = make_unique<Wt::WVBoxLayout>();
	auto treeRegionUnique = make_unique<Wt::WTreeTable>();
	treeRegion = treeRegionUnique.get();
	vLayout->addWidget(move(treeRegionUnique));
	vLayout->addStretch(1);
	this->setLayout(move(vLayout));

	treeRegion->setMaximumSize(maxWidth, maxHeight);
	treeRegion->setObjectName("TreeRegion");
	treeRegion->tree()->setSelectionMode(Wt::SelectionMode::Single);
	treeRegion->addColumn(" ", 10);  // GEO_CODE
	treeRegion->addColumn(" ", 10);  // GEO_LEVEL
}
