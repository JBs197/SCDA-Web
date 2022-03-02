#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>
#include "wjgridlayout.h"

class WJTABLE : public Wt::WContainerWidget
{
	void err(std::string message);
	void initGUI();

public:
	WJTABLE();
	~WJTABLE() = default;

	enum indexMain { UnitPin, Table, Tip };
	enum indexUnitPin { UnitLabel, UnitButton, Stretch, PinRow, PinColumn, Reset };
};
