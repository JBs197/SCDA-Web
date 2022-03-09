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
	//const int numParameter{ 4 };
	Wt::Signal<> populateCataList_;
	std::shared_ptr<std::set<int>> setCataAll;
	std::shared_ptr<std::set<int>>& setPassed;
	std::shared_ptr<std::vector<WJCATA>>& vCata;
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
	WJFILTERBOX(std::shared_ptr<std::vector<WJCATA>>& vCataRef, std::shared_ptr<std::set<int>>& setPassedRef);
	~WJFILTERBOX() = default;

	enum index{ Year, Category, RowTopic, ColumnTopic, Parameter, Count };

	void initCataAll(std::vector<WJCATA> vCataAll);
	void initFilter();
	Wt::Signal<>& populateCataList() { return populateCataList_; }
};
