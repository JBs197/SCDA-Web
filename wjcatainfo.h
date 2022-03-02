#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include "wjcata.h"
#include "wjgridlayout.h"

class WJCATAINFO : public Wt::WContainerWidget
{
	void initGUI();

public:
	WJCATAINFO();
	~WJCATAINFO() = default;

	enum index{ Name, Description, Year, Category, RowTopic, ColTopic, Parameter };

	void depopulate();
	void populate(WJCATA& wjCata);
};
