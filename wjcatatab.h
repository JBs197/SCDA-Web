#pragma once
#include <Wt/WTable.h>
#include "wjcatainfo.h"
#include "wjfilterbox.h"

class WJCATATAB : public Wt::WContainerWidget
{
	int numCata;
	Wt::Signal<std::string, std::string> selectCata_;
	std::shared_ptr<std::set<int>> setPassed;
	std::shared_ptr<std::vector<WJCATA>> vCata;

	void highlightRow(const int& iRow);
	void initGrid();
	void initGUI();
	void itemClicked(const int& iRow);
	void unhighlightRow(const int& iRow);

public:
	WJCATATAB();
	~WJCATATAB() = default;

	enum index{ Label, List };
	std::unordered_map<std::string, std::pair<int, int>> mapGrid;

	void addItem(std::string& sCata);
	void clearInfo();
	void resetList();
	Wt::Signal<std::string, std::string>& selectCata() { return selectCata_; }
	void setList(std::vector<std::string>& vsCata);
	void populateCataInfo(WJCATA& wjCata);
	void populateCataList();
	void updateFilteredLabel();
};
