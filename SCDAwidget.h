#pragma once
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include "SCDAserver.h"
#include "wjcatalist.h"
#include "wjfilterbox.h"

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	SCDAserver& sRef;

	void initGUI();

public:
	SCDAwidget(SCDAserver& myserver);
	~SCDAwidget() = default;

	std::unordered_map<std::string, int> mapResource;
	std::vector<std::shared_ptr<Wt::WMemoryResource>> vResource;

};
