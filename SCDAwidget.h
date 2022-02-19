#pragma once
#include <Wt/WBootstrapTheme.h>
#include <Wt/WContainerWidget.h>
#include "SCDAserver.h"
#include "wjfilterbox.h"

class SCDAwidget : public Wt::WContainerWidget, public SCDAserver::User
{
	SCDAserver& sRef;

	void initGUI();

public:
	SCDAwidget(SCDAserver& myserver);
	~SCDAwidget() = default;

};
