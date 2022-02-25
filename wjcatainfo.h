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

	enum displayIndex{ Name, Description, Year, Category, RowTopic, ColTopic, Parameter };
	enum index{ Display, Selection };

	void populate(WJCATA& wjCata);
	void selectCata(const std::string& sCata);
};
