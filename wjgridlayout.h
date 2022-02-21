#pragma once
#include <Wt/WGridLayout.h>

struct WJGRIDLAYOUT : public Wt::WGridLayout
{
	WJGRIDLAYOUT() : WGridLayout() {}
	~WJGRIDLAYOUT() = default;

	Wt::WLayoutItem* itemAtPosition(int iRow, int iCol);
};

Wt::WLayoutItem* WJGRIDLAYOUT::itemAtPosition(int iRow, int iCol)
{
	int numRow = WGridLayout::rowCount();
	int numCol = WGridLayout::columnCount();
	if (iRow >= numRow || iCol >= numCol) { return nullptr; }
	int index = (iRow * numCol) + iCol;
	return WGridLayout::itemAt(index);
}
