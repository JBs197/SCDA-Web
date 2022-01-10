#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WVBoxLayout.h>
#include "jfunc.h"
#include "jtree.h"

using namespace std;

class WJTREE : public Wt::WContainerWidget
{
	JFUNC jf;
	Wt::WLength maxWidth, maxHeight, wlAuto;

	void err(string message);
	void init(int width, int height);

public:
	WJTREE(int width, int height) { init(width, height); }
	~WJTREE() override {}

	JTREE jt;
	Wt::WTree* tree = nullptr;

	Wt::WTreeNode* findNode(int jtreeID);
};