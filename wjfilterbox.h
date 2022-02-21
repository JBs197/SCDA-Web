#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WComboBox.h>
#include "jsort.h"
#include "SCDAserver.h"
#include "wjfilter.h"
#include "wjgridlayout.h"

class WJFILTERBOX : public Wt::WContainerWidget
{
	JSORT jsort;
	SCDAserver& sRef;

	void err(std::string message);
	void initGUI();

public:
	WJFILTERBOX(SCDAserver& serverRef);
	~WJFILTERBOX() = default;

	void setFilter(std::vector<std::vector<std::string>>& vvsFilter);
};
