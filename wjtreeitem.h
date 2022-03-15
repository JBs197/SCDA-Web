#pragma once
#include <any>
#include <Wt/WStandardItem.h>
#include "jtree.h"

class WJTREEITEM
{
	std::vector<std::unique_ptr<WJTREEITEM>> vChildren;
	std::vector<std::any> vData, vDataUserRole;
	WJTREEITEM* wjtiParent;

public:
	WJTREEITEM(const JNODE& jn, WJTREEITEM* parent = nullptr);
	WJTREEITEM(const std::vector<std::string>& vsData, WJTREEITEM* parent = nullptr);
	WJTREEITEM(const std::vector<std::any>& vVarData, WJTREEITEM* parent = nullptr);
	~WJTREEITEM();

	int ID;

	void addChild(std::unique_ptr<WJTREEITEM> wjti);
	WJTREEITEM* getChild(int iRow);
	std::any data(int iCol) const;
	void dataAll(std::vector<std::any>& vVarData) const;
	std::any dataUserRole(int role) const;
	std::string getName();
	int getNumChildren();
	int getNumCol();
	WJTREEITEM* getParent();
	int getRow() const;
	void removeChildren();
	void setData(int role, std::string sData);
	void setData(int role, std::any aData);
};
