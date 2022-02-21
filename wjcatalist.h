#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>
#include <Wt/WTable.h>
#include <Wt/WVBoxLayout.h>

class WJCATALIST : public Wt::WContainerWidget
{
	int numCata;

public:
	WJCATALIST();
	~WJCATALIST() = default;

	void updateLabel();
};
