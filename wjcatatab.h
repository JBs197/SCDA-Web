#pragma once
#include <Wt/WTable.h>
#include "wjcatainfo.h"

class WJCATATAB : public Wt::WContainerWidget
{
	Wt::Signal<int> displayCata_;
	std::unordered_map<std::string, std::pair<int, int>> mapGrid;
	int numCata;
	Wt::Signal<std::string> selectCata_;

	void highlightRow(const int& iRow);
	void initGrid();
	void initGUI();
	void itemClicked(const std::string& sCata);
	void unhighlightRow(const int& iRow);

public:
	WJCATATAB();
	~WJCATATAB() = default;

	enum index{ Label, List };

	void addItem(std::string& sCata);
	void clearInfo();
	Wt::Signal<int>& displayCata() { return displayCata_; }
	void resetList();
	Wt::Signal<std::string>& selectCata() { return selectCata_; }
	void setList(std::vector<std::string>& vsCata);
	void populateInfo(WJCATA& wjCata);
	void updateFilteredLabel();
};
