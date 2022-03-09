#pragma once
#include <Wt/WContainerWidget.h>
#include "jtree.h"
#include "wjpanel.h"

class WJPARAM : public WJPANEL
{
	void clearList(Wt::WPopupMenu* popup = nullptr);
	void err(std::string message);
	void populateTree(JTREE* jt, int parentID, Wt::WPopupMenu* popup);

public:
	WJPARAM();
	~WJPARAM() = default;

	std::shared_ptr<JTREE> jtMID;

	void populate();
	void selectNode(const int& nodeID);
	void setTitle(std::string& title);
};
