#pragma once
#include <Wt/WGridLayout.h>

struct WJGRIDLAYOUT : public Wt::WGridLayout
{
	WJGRIDLAYOUT() : WGridLayout() {}
	~WJGRIDLAYOUT() = default;

	Wt::WWidget* addWidget(std::unique_ptr<Wt::WWidget> widget, std::pair<int, int>& coord);
	Wt::WLayoutItem* itemAtPosition(int iRow, int iCol);
	Wt::WLayoutItem* itemAtPosition(std::pair<int, int>& coord);
};

/*
Wt::WWidget* WJGRIDLAYOUT::addWidget(std::unique_ptr<Wt::WWidget> widget, std::pair<int, int>& coord)
{
	int iRow = get<0>(coord);
	int iCol = get<1>(coord);
	return Wt::WGridLayout::addWidget(std::move(widget), iRow, iCol);
}
*/
Wt::WLayoutItem* WJGRIDLAYOUT::itemAtPosition(int iRow, int iCol)
{
	int numRow = WGridLayout::rowCount();
	int numCol = WGridLayout::columnCount();
	if (iRow >= numRow || iCol >= numCol) { return nullptr; }
	int index = (iRow * numCol) + iCol;
	return Wt::WGridLayout::itemAt(index);
}

/*
Wt::WLayoutItem* WJGRIDLAYOUT::itemAtPosition(std::pair<int, int>& coord)
{
	int numRow = WGridLayout::rowCount();
	int numCol = WGridLayout::columnCount();
	if (get<0>(coord) >= numRow || get<1>(coord) >= numCol) { return nullptr; }
	int index = (get<0>(coord) * numCol) + get<1>(coord);
	return Wt::WGridLayout::itemAt(index);
}
*/
