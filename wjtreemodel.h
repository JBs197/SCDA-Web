#pragma once
#include <Wt/WAbstractItemModel.h>
#include "wjtreeitem.h"

class WJTREEMODEL : public Wt::WAbstractItemModel
{
	JSTRING jstr;
	WJTREEITEM* wjtiRoot;
	
	void addChildrenAll(int parentID, WJTREEITEM*& wjtiParent);
	void err(std::string message);

public:
	WJTREEMODEL(std::vector<std::string> vsHeader);
	~WJTREEMODEL();

	bool headerTitles;
	shared_ptr<JTREE> jt;

	int columnCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const override;
	Wt::cpp17::any data(const Wt::WModelIndex& index, Wt::ItemDataRole role) const override;
	WJTREEITEM* getNode(const Wt::WModelIndex& wmiNode);
	WJTREEITEM* getRoot();
	Wt::cpp17::any headerData(int section, Wt::Orientation orientation, Wt::ItemDataRole role = Wt::ItemDataRole::Display) const override;
	Wt::WModelIndex index(int row, int column, const Wt::WModelIndex& index = Wt::WModelIndex()) const override;
	Wt::WModelIndex parent(const Wt::WModelIndex& parent = Wt::WModelIndex()) const override;
	void populate();
	int rowCount(const Wt::WModelIndex& parent = Wt::WModelIndex()) const override;
	void setHeaderData(std::vector<std::string> vsHeader, int role = Wt::ItemDataRole::Display);
};
