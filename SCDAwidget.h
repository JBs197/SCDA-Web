#pragma once
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTabWidget.h>
#include "SCDAserver.h"
#include "wjcatatab.h"
#include "wjfilterbox.h"

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	SCDAserver& sRef;

	void err(std::string message);
	void initGUI();
	void populateCataInfo(const int& cataIndex);
	void populateCataList();
	void processDataEvent(const DataEvent& event);
	void pullCatalogue(const std::string& sCata);

public:
	SCDAwidget(SCDAserver& myserver);
	~SCDAwidget() = default;

	enum layoutMain { Filter, Tab };
	enum tabIndex { Catalogue, Tree, Table, Map, Download };

	std::unordered_map<std::string, int> mapResource;
	std::vector<std::shared_ptr<Wt::WMemoryResource>> vResource;

};
