#include "wjtreeitem.h"

using namespace std;

WJTREEITEM::WJTREEITEM(const JNODE& jn, WJTREEITEM* parent) : wjtiParent(parent)
{
	ID = jn.ID;

	for (int ii = 0; ii < 8; ii++) {
		vDataUserRole.emplace_back("");
	}
	vDataUserRole[0] = get<0>(jn.colour).c_str();
	vDataUserRole[1] = get<1>(jn.colour).c_str();
	vDataUserRole[5] = "0";
	vDataUserRole[6] = get<0>(jn.colourSelected).c_str();
	vDataUserRole[7] = get<1>(jn.colourSelected).c_str();

	int numCol = (int)jn.vsData.size() + (int)jn.mapAttribute.size();
	vData.resize(numCol);
	for (int ii = 0; ii < numCol; ii++) {
		vData[ii] = jn.vsData[ii].c_str();
	}

}
WJTREEITEM::WJTREEITEM(const vector<string>& vsData, WJTREEITEM* parent)
	: wjtiParent(parent), ID(-1) {

	int numCol = (int)vsData.size();
	vData.resize(numCol);
	for (int ii = 0; ii < numCol; ii++) {
		vData[ii] = vsData[ii].c_str();
	}
	if (parent != nullptr && parent->vDataUserRole.size() > 0) {
		vDataUserRole = parent->vDataUserRole;
	}
}
WJTREEITEM::WJTREEITEM(const vector<any>& vVarData, WJTREEITEM* parent)
	: wjtiParent(parent), ID(-1) {
	vData = vVarData;
	if (parent != nullptr && parent->vDataUserRole.size() > 0) {
		vDataUserRole = parent->vDataUserRole;
	}
}
WJTREEITEM::~WJTREEITEM()
{
	int numChildren = (int)vChildren.size();
	for (int ii = 0; ii < numChildren; ii++) {
		vChildren[ii].reset();
	}
}

void WJTREEITEM::addChild(unique_ptr<WJTREEITEM> wjti)
{
	vChildren.emplace_back(wjti);
}
WJTREEITEM* WJTREEITEM::getChild(int iRow)
{
	if (iRow < 0 || iRow >= vChildren.size()) { return nullptr; }
	return vChildren.at(iRow).get();
}
any WJTREEITEM::data(int iCol) const
{
	// Used for DisplayRole data.
	if (iCol < 0 || iCol >= vData.size()) { return any(); }
	return vData.at(iCol);
}
void WJTREEITEM::dataAll(vector<any>& vVarData) const
{
	vVarData = vData;
}
any WJTREEITEM::dataUserRole(int role) const
{
	// Used for UserRole data.
	int iRole = Wt::ItemDataRole::User;
	if (role - iRole < 0 || role - iRole >= vDataUserRole.size()) {
		return any();
	}
	return vDataUserRole.at(role - iRole);
}
string WJTREEITEM::getName()
{
	// Note: this returns the node's vData[0] string (name visible on tree), not 
	// Wt's ObjectName property. 
	string name;
	if (vData.size() > 0) {
		name = any_cast<string>(vData[0]);
	}
	return name;
}
int WJTREEITEM::getNumChildren()
{
	return (int)vChildren.size();
}
int WJTREEITEM::getNumCol()
{
	return (int)vData.size();
}
WJTREEITEM* WJTREEITEM::getParent()
{
	return wjtiParent;
}
int WJTREEITEM::getRow() const
{
	if (wjtiParent != nullptr) {
		int numSibling = wjtiParent->getNumChildren();
		for (int ii = 0; ii < numSibling; ii++) {
			if (wjtiParent->vChildren.at(ii)->ID == ID) { return ii; }
		}
	}
	return -1;
}
void WJTREEITEM::removeChildren()
{
	int numChildren = (int)vChildren.size();
	for (int ii = 0; ii < numChildren; ii++) {
		vChildren[ii].reset();
	}
	vChildren.clear();
}
void WJTREEITEM::setData(int role, string sData)
{
	// If setting DisplayRole data, then sData is assumed to be a 
	// MIME-friendly list of column values, with the first char 
	// being the marker/splitter char. 
	if (role < 0 || sData.size() == 0) { return; }

	if (role == Wt::ItemDataRole::Display) {
		for (int ii = 0; ii < vData.size(); ii++) {
			vData[ii].reset();
		}
		vData.clear();

		char marker = sData[0];
		int index = 0;
		size_t pos1 = 1;
		size_t pos2 = sData.find(marker, pos1);
		while (pos2 < sData.size()) {
			auto data = make_any<string>(sData.substr(pos1, pos2));
			vData.push_back(std::move(data));
			pos1 = pos2 + 1;
			pos2 = sData.find(marker, pos1);
		}
		if (pos1 < sData.size()) {
			auto data = make_any<string>(sData.substr(pos1, pos2));
			vData.push_back(std::move(data));
		}
	}
	else if (role >= Wt::ItemDataRole::User) {
		while (role - Wt::ItemDataRole::User >= vDataUserRole.size()) {
			vDataUserRole.emplace_back("");
		}
		vDataUserRole[role - Wt::ItemDataRole::User] = sData.c_str();
	}
}
void WJTREEITEM::setData(int role, any aData)
{
	if (role < 0 || !aData.has_value()) { return; }

	if (role == Wt::ItemDataRole::Display) {
		for (int ii = 0; ii < vData.size(); ii++) {
			vData[ii].reset();
		}
		vData.clear();

		string sData = any_cast<string>(aData);
		char marker = sData[0];
		int index = 0;
		size_t pos1 = 1;
		size_t pos2 = sData.find(marker, pos1);
		while (pos2 < sData.size()) {
			auto data = make_any<string>(sData.substr(pos1, pos2));
			vData.push_back(std::move(data));
			pos1 = pos2 + 1;
			pos2 = sData.find(marker, pos1);
		}
		if (pos1 < sData.size()) {
			auto data = make_any<string>(sData.substr(pos1, pos2));
			vData.push_back(std::move(data));
		}
	}
	else if (role >= Wt::ItemDataRole::User) {
		while (role - Wt::ItemDataRole::User >= vDataUserRole.size()) {
			vDataUserRole.emplace_back("");
		}
		vDataUserRole[role - Wt::ItemDataRole::User] = std::move(aData);
	}
}
