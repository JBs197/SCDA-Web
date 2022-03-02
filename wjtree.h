#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WVBoxLayout.h>
#include "jtree.h"

class WJTREE : public Wt::WContainerWidget
{
	void err(std::string message);
	void initGUI();

public:
	WJTREE();
	~WJTREE() = default;

	std::shared_ptr<JTREE> jt;

};