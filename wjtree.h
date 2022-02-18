#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WVBoxLayout.h>
#include "jtree.h"

class WJTREE : public Wt::WContainerWidget
{
	Wt::WLength maxWidth, maxHeight, wlAuto;

	void err(std::string message);
	void init(int width, int height);

public:
	WJTREE(int width, int height) { init(width, height); }
	~WJTREE() override {}

	JTREE jt;
	Wt::WTree* tree = nullptr;

	Wt::WTreeNode* findNode(int jtreeID);
};