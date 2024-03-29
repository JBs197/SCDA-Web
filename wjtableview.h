#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WStandardItem.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/WTableView.h>
#include "jstring.h"
#include "wjdelegate.h"

class WJTABLEVIEW : public Wt::WTableView
{
	std::shared_ptr<WJCELL> wjCell;
	std::shared_ptr<WJHEADER> wjHeader;

	void err(std::string message);

public:
	WJTABLEVIEW();
	~WJTABLEVIEW() = default;

	double colWidth, headerHeight, headerWidth, preloadMargin, rowHeight;

	void initCellDelegate(double height, std::string& cssCell, std::string& cssRowHeader, std::string& cssColHeader, std::string& cssTopLeft);
	void setCellSize();
};
