#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WPanel.h>
#include <Wt/WPopupMenu.h>
#include <Wt/WPushButton.h>
#include "jlog.h"

class WJPANEL : public Wt::WPanel
{
	void err(std::string message);

public:
	WJPANEL();
	~WJPANEL() = default;

	enum index { Label, Stretch, Button };

	void setLabel(std::string label);
	void setStyleButton(std::string cssClass);
	void setStyleLabel(std::string cssClass);
	void setStylePopup(std::string cssClass);
	void setStyleTitle(std::string cssClass);
};
