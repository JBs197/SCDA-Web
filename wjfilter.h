#pragma once
#include <Wt/WComboBox.h>
#include <Wt/WPanel.h>

class WJFILTER : public Wt::WPanel
{
	std::set<std::string> setDesolate, setNormal;
	std::vector<std::string> vsFilter;

public:
	WJFILTER(std::string title);
	~WJFILTER() = default;

	void initValue(std::vector<std::string>& vsValue);
	void setEnabled(bool trueFalse);
};
