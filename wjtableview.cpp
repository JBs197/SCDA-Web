#include "wjtableview.h"

using namespace std;

WJTABLEVIEW::WJTABLEVIEW() : colWidth(-1.0), headerHeight(-1.0), headerWidth(-1.0),
preloadMargin(-1.0), rowHeight(-1.0) 
{
	setRowHeaderCount(1);
	setSortingEnabled(0);
	setColumnResizeEnabled(1);
}

void WJTABLEVIEW::err(string message)
{
	string errorMessage = "WJTABLEVIEW error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJTABLEVIEW::initCellDelegate(double height, string& cssCell, string& cssRowHeader, string& cssColHeader, string& cssTopLeft)
{
	wjCell = make_shared<WJCELL>(height);
	wjCell->initCSS(cssCell, cssRowHeader);
	setItemDelegate(wjCell);

	wjHeader = make_shared<WJHEADER>(height);
	wjHeader->initCSS(cssColHeader, cssTopLeft);
	setHeaderItemDelegate(wjHeader);
}
void WJTABLEVIEW::setCellSize()
{
	auto wsiModel = model();
	if (wsiModel == nullptr) { return; }
	int numCol = wsiModel->columnCount();

	if (headerHeight > 0.0) { setHeaderHeight(headerHeight); }
	if (rowHeight > 0.0) { setRowHeight(rowHeight); }
	if (headerWidth > 0.0) { setColumnWidth(0, headerWidth); }
	//if (preloadMargin > 0.0) { setPreloadMargin(preloadMargin); }
	if (colWidth > 0.0) {
		for (int ii = 1; ii < numCol; ii++) {
			setHeaderWordWrap(ii, 1);
			setHeaderAlignment(ii, Wt::AlignmentFlag::Top);
			setColumnAlignment(ii, Wt::AlignmentFlag::Top);
			setColumnWidth(ii, colWidth);
		}
	}
}
