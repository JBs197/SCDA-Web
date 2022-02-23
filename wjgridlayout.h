#pragma once
#include <Wt/WGridLayout.h>

struct WJGRIDLAYOUT : public Wt::WGridLayout
{
	WJGRIDLAYOUT() : WGridLayout() {}
	~WJGRIDLAYOUT() = default;

	Wt::WLayoutItem* itemAtPosition(int iRow, int iCol);
	Wt::WLayoutItem* itemAtPosition(std::pair<int, int>& coord);
};

