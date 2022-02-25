#pragma once
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include "SCDAserver.h"
#include "wjcatainfo.h"
#include "wjcatalist.h"
#include "wjfilterbox.h"

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	SCDAserver& sRef;

	void initGUI();
	void populateCataInfo(const int& cataIndex);
	void populateCataList();
	void processDataEvent(const DataEvent& event);

public:
	SCDAwidget(SCDAserver& myserver);
	~SCDAwidget() = default;

	enum layoutFilter { FilterBox, CataList, CataInfo };
	enum layoutMain { Filter, Display };
	std::unordered_map<std::string, int> mapResource;
	std::vector<std::shared_ptr<Wt::WMemoryResource>> vResource;

};
