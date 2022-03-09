#pragma once
#include "wjpanel.h"

class WJFILTER : public WJPANEL
{
	std::atomic_int goDeaf;
	std::set<std::string> setDesolate, setNormal;
	Wt::Signal<> updateFilter_;

	void clearList(Wt::WPopupMenu* popup = nullptr);
	void err(std::string message);
	void highlightItem(const int& liveDead, const int& index);
	void selectionChanged(const std::string& text);
	void unhighlightItem(const int& liveDead, const int& index);
	void updateLabel(std::string text);

public:
	WJFILTER(std::string title);
	~WJFILTER() = default;

	std::string getSelected();
	void initList(std::vector<std::string>& vsLive, std::vector<std::string>& vsDead);
	void setEnabled(bool trueFalse);
	Wt::Signal<>& updateFilter() { return updateFilter_; }
};
