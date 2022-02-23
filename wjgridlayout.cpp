#include "wjgridlayout.h"

using namespace std;

Wt::WLayoutItem* WJGRIDLAYOUT::itemAtPosition(int iRow, int iCol)
{
	int numRow = WGridLayout::rowCount();
	int numCol = WGridLayout::columnCount();
	if (iRow >= numRow || iCol >= numCol) { return nullptr; }
	int index = (iRow * numCol) + iCol;
	return Wt::WGridLayout::itemAt(index);
}
Wt::WLayoutItem* WJGRIDLAYOUT::itemAtPosition(std::pair<int, int>& coord)
{
	int numRow = WGridLayout::rowCount();
	int numCol = WGridLayout::columnCount();
	if (get<0>(coord) >= numRow || get<1>(coord) >= numCol) { return nullptr; }
	int index = (get<0>(coord) * numCol) + get<1>(coord);
	return Wt::WGridLayout::itemAt(index);
}
