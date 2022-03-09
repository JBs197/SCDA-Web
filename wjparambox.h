#pragma once
#include <Wt/WVBoxLayout.h>
#include "wjparam.h"

class WJPARAMBOX : public Wt::WContainerWidget
{
	void err(std::string message);
	void initGUI();

public:
	WJPARAMBOX();
	~WJPARAMBOX() = default;

	void resetAll();
};
