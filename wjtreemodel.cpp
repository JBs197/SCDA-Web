#include "wjtreemodel.h"

using namespace std;

WJTREEMODEL::WJTREEMODEL(vector<string> vsHeader)
{
	wjtiRoot = new WJTREEITEM(vsHeader);
	setHeaderData(vsHeader);
}

int WJTREEMODEL::columnCount(const Wt::WModelIndex& parent) const
{
	if (parent.isValid()) {
		return static_cast<WJTREEITEM*>(parent.internalPointer())->getNumCol();
	}
	return wjtiRoot->getNumCol();
}
Wt::cpp17::any WJTREEMODEL::data(const Wt::WModelIndex& index, Wt::ItemDataRole role) const
{
	if (!index.isValid()) { return Wt::cpp17::any(); }
	int iRole = role.value();

	WJTREEITEM* wjti = static_cast<WJTREEITEM*>(index.internalPointer());
	if (iRole < Wt::ItemDataRole::User) { return wjti->data(iRole); }
	else if (iRole >= Wt::ItemDataRole::User) { return wjti->dataUserRole(iRole); }

	return Wt::cpp17::any();
}
Wt::cpp17::any WJTREEMODEL::headerData(int section, Wt::Orientation orientation, Wt::ItemDataRole role) const
{
	if (orientation == Wt::Orientation::Horizontal && role == Wt::ItemDataRole::Display) {
		return wjtiRoot->data(section);
	}
	return Wt::cpp17::any();
}
WJTREEITEM* WJTREEMODEL::getNode(const Wt::WModelIndex& wmiNode)
{
	if (!wmiNode.isValid()) { return nullptr; }
	return static_cast<WJTREEITEM*>(wmiNode.internalPointer());
}
WJTREEITEM* WJTREEMODEL::getRoot()
{
	return wjtiRoot;
}
Wt::WModelIndex WJTREEMODEL::index(int row, int column, const Wt::WModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) { return Wt::WModelIndex(); }

	WJTREEITEM* wjtiParent;
	if (parent.isValid()) {
		wjtiParent = static_cast<WJTREEITEM*>(parent.internalPointer());
	}
	else { wjtiParent = wjtiRoot; }

	WJTREEITEM* wjtiChild = wjtiParent->getChild(row);
	if (wjtiChild == nullptr) { return Wt::WModelIndex(); }
	return createIndex(row, column, wjtiChild);
}
Wt::WModelIndex WJTREEMODEL::parent(const Wt::WModelIndex& index) const
{
	if (!index.isValid()) { return Wt::WModelIndex(); }
	WJTREEITEM* wjtiChild = static_cast<WJTREEITEM*>(index.internalPointer());

	WJTREEITEM* wjtiParent = wjtiChild->getParent();
	if (wjtiParent == wjtiRoot) { return Wt::WModelIndex(); }

	return createIndex(wjtiParent->getRow(), 0, wjtiParent);
}
void WJTREEMODEL::populate()
{
	// Loads the local JTREE into this model.
	vector<any> vaData;
	wjtiRoot->dataAll(vaData);
	Wt::WAbstractItemModel::reset();
	delete wjtiRoot;
	wjtiRoot = new WJTREEITEM(vaData);

	JNODE jnRoot = jt.getRoot();
	int rootID = jnRoot.ID;
	addChildrenAll(rootID, wjtiRoot);
}
int WJTREEMODEL::rowCount(const Wt::WModelIndex& parent) const
{
	if (parent.column() > 0) { return 0; }

	WJTREEITEM* wjtiParent;
	if (parent.isValid()) {
		wjtiParent = static_cast<WJTREEITEM*>(parent.internalPointer());
	}
	else { wjtiParent = wjtiRoot; }
	return wjtiParent->getNumChildren();
}
void WJTREEMODEL::setHeaderData(vector<string> vsHeader, int role)
{
	headerTitles = 0;
	int numCol = (int)vsHeader.size();
	for (int ii = 0; ii < numCol; ii++) {
		if (vsHeader[ii].size() > 0) {
			Wt::WAbstractItemModel::setHeaderData(ii, Wt::Orientation::Horizontal, vsHeader[ii].c_str(), role);
			headerTitles = 1;
		}
	}
}
