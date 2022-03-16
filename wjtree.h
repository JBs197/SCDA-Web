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
	std::pair<int, std::string> selectedRegion;

	void addChildren(int parentID, Wt::WTreeNode*& parentNode, int genRemaining);
	void addPlaceholderNode(Wt::WTreeNode*& node);
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

	std::pair<int, std::string> getSelectedRegion();
	void setLabel(const std::string& sCata);
	void setSelectedRegion(const std::pair<int, std::string>& selRegion);
	void setTree(const std::vector<std::vector<std::string>>& vvsGeo);
	void updateSelectedRegion();
};