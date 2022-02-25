#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>

class WJCATALIST : public Wt::WContainerWidget
{
	Wt::Signal<int> displayCata_;
	Wt::Signal<std::string> selectCata_;
	int numCata;

	void highlightRow(const int& iRow);
	void itemClicked(const std::string& sCata);
	void unhighlightRow(const int& iRow);

public:
	WJCATALIST();
	~WJCATALIST() = default;

	enum index{ Label, List };

	void addItem(std::string& sCata);
	Wt::Signal<int>& displayCata() { return displayCata_; }
	void resetList();
	Wt::Signal<std::string>& selectCata() { return selectCata_; }
	void setList(std::vector<std::string>& vsCata);
	void updateLabel();
};
