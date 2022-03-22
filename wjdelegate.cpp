#include "wjdelegate.h"

using namespace std;

void WJCELL::initCSS(string& sCell, string& sRowHeader)
{
	cssCell = sCell;
	cssRowHeader = sRowHeader;
}
unique_ptr<Wt::WWidget> WJCELL::update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags)
{
	int iRow = index.row();
	int iCol = index.column();
	auto widgetUnique = Wt::WItemDelegate::update(widget, index, flags);
	if (!widget) {
		if (iCol > 0) { widgetUnique->setAttributeValue("style", cssCell); }
		else { widgetUnique->setAttributeValue("style", cssRowHeader); }		
	}
	return widgetUnique;
}

void WJHEADER::initCSS(string& sColHeader, string& sTopLeft)
{
	cssColHeader = sColHeader;
	cssTopLeft = sTopLeft;
}
unique_ptr<Wt::WWidget> WJHEADER::update(Wt::WWidget* widget, const Wt::WModelIndex& index, Wt::WFlags<Wt::ViewItemRenderFlag> flags)
{
	int iRow = index.row();
	int iCol = index.column();
	auto widgetUnique = Wt::WItemDelegate::update(widget, index, flags);
	if (!widget) {
		if (iCol > 0) { widgetUnique->setAttributeValue("style", cssColHeader); }
		else { widgetUnique->setAttributeValue("style", cssTopLeft); }
	}
	return widgetUnique;
}
