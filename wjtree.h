#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WTree.h>
#include <Wt/WTreeNode.h>
#include <Wt/WVBoxLayout.h>
#include "jtree.h"

class WJTREE : public Wt::WContainerWidget
{
	JSTRING jstr;
	std::string sCata;

	void addChildren(int parentID, Wt::WTreeNode*& parentNode, int genRemaining);
	void err(std::string message);
	void expandNode(const std::string& sName);
	void initGUI();
	void makeTreeGeo(const std::vector<std::vector<std::string>>& vvsGeo, JTREE*& jt);

public:
	WJTREE();
	~WJTREE() = default;

	int defaultRowExpansion;
	enum index { Label, Tree, Stretch };
	std::shared_ptr<JTREE> jtGeo;

	void setLabel(const std::string& sCata);
	void setTree(const std::vector<std::vector<std::string>>& vvsGeo);
};