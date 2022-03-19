#pragma once
#include <Wt/WHBoxLayout.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>
#include "jparse.h"
#include "jtree.h"
#include "wjgridlayout.h"
#include "wjtableview.h"

class WJTABLEBOX : public Wt::WContainerWidget
{
	std::shared_ptr<JTREE> colHeader, rowHeader;
	std::string colTopic, rowTopic;
	JPARSE jparse;
	std::unordered_map<int, int> mapGeoCode;  // GeoCode -> table index.
	std::shared_ptr<Wt::WStandardItemModel> modelTable;
	std::shared_ptr<std::vector<std::vector<std::vector<std::string>>>> vvvsTable;  // Form [table index][row index][column index]

	void err(std::string message);
	void initGUI();
	void populateHeader();
	void populateTable(int index);

public:
	WJTABLEBOX();
	~WJTABLEBOX() = default;

	enum indexMain { UnitPin, Table, Tip };
	enum indexUnitPin { UnitLabel, UnitButton, Stretch, PinRow, PinColumn, Reset };

	void configure(std::string& configXML);
	void initTableAll(const std::vector<std::string>& vsGeoCode, const std::vector<std::vector<std::vector<std::string>>>& vvvsTable);
	void selectRegion(std::pair<int, std::string> selRegion);
	void setHeader(const std::vector<std::vector<std::vector<std::string>>>& vvvsMID);
	void setTopic(const std::vector<std::vector<std::string>>& vvsDIM);
};
