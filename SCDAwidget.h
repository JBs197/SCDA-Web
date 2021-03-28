#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include "SCDAserver.h"

using namespace std;

class SCDAwidget : public Wt::WContainerWidget
{
	Wt::WContainerWidget *boxControl, *boxTreelist, *boxTable;
	SCDAserver& serverRef;

	void init();
	virtual void makeUI(unique_ptr<Wt::WContainerWidget> boxc, unique_ptr<Wt::WContainerWidget> boxtl, unique_ptr<Wt::WContainerWidget> boxt);

public:
	SCDAwidget(SCDAserver& myserver) : WContainerWidget(), serverRef(myserver) { init(); }

};

