#pragma once
#include <thread>
#include <unordered_set>
#include <Wt/WContainerWidget.h>
#include <Wt/WVBoxLayout.h>
#include "jsort.h"
#include "wjcata.h"
#include "wjfilter.h"
#include "wjgridlayout.h"

class WJFILTERBOX : public Wt::WContainerWidget
{
	JSORT jsort;
	//std::unordered_set<std::string> setCategory, setColTopic, setParameter, setRowTopic, setYear;
	const int numParameter{ 4 };
	Wt::Signal<> populateCataList_;
	std::shared_ptr<std::vector<std::set<std::string>>> vFilterCandidate;

	void err(std::string message);
	void getFilterAll(std::vector<std::string>& vsFilter);
	void initGUI();
	void resetPassed();
	void setFilterAll(std::vector<std::vector<std::vector<std::string>>>& vvvsLiveDead);
	void updateFilterAll();
	void updateLiveDead(std::vector<std::vector<std::vector<std::string>>>& vvvsLiveDead, std::atomic_int& statusLiveDead, std::vector<std::string> vsFilter, std::vector<WJCATA> *vCata, std::vector<std::set<std::string>> *vFilterCandidate);
	void updatePassed(std::vector<std::string>& vsFilter);

public:
	WJFILTERBOX();
	~WJFILTERBOX() = default;

	enum index{ Year, Category, RowTopic, ColumnTopic, Parameter, Count };
	std::shared_ptr<std::set<int>> setPassed;
	std::shared_ptr<std::vector<WJCATA>> vCata;

	void initFilter();
	Wt::Signal<>& populateCataList() { return populateCataList_; }
	void setIcon(std::vector<unsigned char>& binIcon);
};
