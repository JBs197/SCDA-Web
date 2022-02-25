#pragma once
#include <Wt/WComboBox.h>
#include <Wt/WPanel.h>

class WJFILTER : public Wt::WPanel
{
	std::atomic_int goDeaf;
	std::set<std::string> setDesolate, setNormal;
	Wt::Signal<> updateFilter_;
	std::vector<std::string> vsFilter;

	void selectionChanged(const int& selIndex);

public:
	WJFILTER(std::string title);
	~WJFILTER() = default;

	void getSelected(int& selIndex, std::string& selValue);
	void initValue(std::vector<std::string>& vsValue);
	Wt::Signal<>& updateFilter() { return updateFilter_; }
	void setEnabled(bool trueFalse);
};
