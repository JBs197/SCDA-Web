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

	void initParamAll(const std::vector<std::vector<std::string>>& vvsDIM);
	void resetAll();
};
