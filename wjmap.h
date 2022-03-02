#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include "jstring.h"
#include "wjgridlayout.h"
#include "wtpaint.h"

class WJMAP : public Wt::WContainerWidget
{
	void err(std::string message);
	void initGUI();

public:
	WJMAP();
	~WJMAP() = default;

	enum indexMain { UnitPin, Map, Tip};
	enum indexUnitPin { UnitLabel, UnitButton, Stretch, PinData, Reset };
};

