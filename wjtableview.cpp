#include "wjtableview.h"

using namespace std;

WJTABLEVIEW::WJTABLEVIEW() : colWidth(-1.0), headerHeight(-1.0), headerWidth(-1.0),
preloadMargin(-1.0), rowHeight(-1.0) 
{
	setSortingEnabled(0);
	setColumnResizeEnabled(1);
	setOverflow(Wt::Overflow::Auto);
}

void WJTABLEVIEW::err(string message)
{
	string errorMessage = "WJTABLEVIEW error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJTABLEVIEW::initCellDelegate(double height, string& cssCell, string& cssRowHeader)
{
	wjCell = make_shared<WJCELL>(height);
	wjCell->initCSS(cssCell, cssRowHeader);
	setItemDelegate(wjCell);
}
void WJTABLEVIEW::setCellSize()
{
	auto wsiModel = model();
	if (wsiModel == nullptr) { return; }
	int numCol = wsiModel->columnCount();

	if (colWidth > 0.0) {
		for (int ii = 1; ii < numCol; ii++) {
			setHeaderWordWrap(ii, 1);
			setHeaderAlignment(ii, Wt::AlignmentFlag::Top);
			setColumnAlignment(ii, Wt::AlignmentFlag::Top);
			setColumnWidth(ii, colWidth);
		}
	} 
	if (headerHeight > 0.0) { setHeaderHeight(headerHeight); }
	if (headerWidth > 0.0) { setColumnWidth(0, headerWidth); }
	if (preloadMargin > 0.0) { setPreloadMargin(preloadMargin); }
	if (rowHeight > 0.0) { setRowHeight(rowHeight); }
}
