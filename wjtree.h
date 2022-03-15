#pragma once
#include <Wt/WContainerWidget.h>
#include <Wt/WStandardItem.h>
#include <Wt/WStandardItemModel.h>
#include <Wt/WText.h>
#include <Wt/WTreeView.h>
#include <Wt/WVBoxLayout.h>
#include "jtime.h"
#include "wjtreemodel.h"

class WJTREE : public Wt::WContainerWidget
{
	std::string sCata;
	std::shared_ptr<WJTREEMODEL> modelGeo;

	void err(std::string message);
	void initGUI();
	void makeTreeGeo(const std::vector<std::vector<std::string>>& vvsGeo);
	void populateTree(JTREE*& jt, int parentID, Wt::WTreeNode*& parentNode);

public:
	WJTREE();
	~WJTREE() = default;

	int defaultRowExpansion;
	enum index { Label, Tree, Stretch };

	void setLabel(const std::string& sCata);
	void setTree(const std::vector<std::vector<std::string>>& vvsGeo);
};