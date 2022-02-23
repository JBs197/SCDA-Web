#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WComboBox.h>
#include "jsort.h"
#include "SCDAserver.h"
#include "wjcata.h"
#include "wjfilter.h"
#include "wjgridlayout.h"

class WJFILTERBOX : public Wt::WContainerWidget
{
	JSORT jsort;
	std::unordered_set<std::string> setCategory, setColTopic, setParameter, setRowTopic, setYear;
	SCDAserver& sRef;
	std::unordered_map<std::string, std::pair<int, int>> mapGrid;

	void err(std::string message);
	void initGrid();
	void initGUI();

public:
	WJFILTERBOX(SCDAserver& serverRef);
	~WJFILTERBOX() = default;

	std::shared_ptr<std::vector<WJCATA>> vCata;

	void initFilter();
	//void setFilter(std::vector<std::vector<std::string>>& vvsFilter);
};
