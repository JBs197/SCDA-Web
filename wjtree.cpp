#include "wjtree.h"

using namespace std;

WJTREE::WJTREE()
{
	defaultRowExpansion = -1;

	initGUI();

	jtGeo = make_shared<JTREE>();
}

void WJTREE::addChildren(int parentID, Wt::WTreeNode*& parentNode, int genRemaining)
{
	// Takes an existing node and makes its children, to the specified number of generations.
	unique_ptr<Wt::WTreeNode> nodeUnique;
	Wt::WTreeNode* node = nullptr;
	string sName;  // Form $JTREE_ID$@child_index0@child_index1@...
	string parentIndexChain = parentNode->objectName();
	size_t pos1 = parentIndexChain.rfind(parentIndexChain[0]);
	parentIndexChain = parentIndexChain.substr(pos1);

	vector<int> childrenID = jtGeo->getChildrenID(parentID);
	int numChildren = (int)childrenID.size();
	if (numChildren == 0) { return; }
	for (int ii = 0; ii < numChildren; ii++) {
		JNODE& jnChild = jtGeo->getNode(childrenID[ii]);
		nodeUnique = make_unique<Wt::WTreeNode>(jnChild.vsData[0]);
		node = parentNode->addChildNode(std::move(nodeUnique));		

		sName = "$" + to_string(jnChild.ID) + parentIndexChain + "@" + to_string(ii);
		node->setObjectName(sName);
		node->expanded().connect(this, bind(&WJTREE::expandNode, this, sName));

		if (genRemaining > 1) { addChildren(childrenID[ii], node, genRemaining - 1); }
	}
}
void WJTREE::err(string message)
{
	string errorMessage = "WJTREE error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
void WJTREE::expandNode(const string& sName)
{
	// sName has form $JTREE_ID$@child_index0@child_index1@...
	size_t pos1 = sName.rfind(sName[0]);
	string indexChain = sName.substr(pos1 + 1);
	int parentID;
	try { parentID = stoi(sName.substr(1, pos1 - 1)); }
	catch (invalid_argument) { err("parentID stoi-expandNode"); }

	vector<string> vsIndex;
	jstr.splitByMarker(vsIndex, indexChain);
	int numIndex = (int)vsIndex.size();
	vector<int> viIndex(numIndex);
	for (int ii = 0; ii < numIndex; ii++) {
		try { viIndex[ii] = stoi(vsIndex[ii]); }
		catch (invalid_argument) { err("Index chain stoi-expandNode"); }
	}
	
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::Tree);
	auto tree = (Wt::WTree*)wlItem->widget();
	auto node = tree->treeRoot();

	vector<Wt::WTreeNode*> vChildren;
	for (int ii = 0; ii < numIndex; ii++) {
		vChildren = node->childNodes();
		node = vChildren[viIndex[ii]];
	}
	addChildren(parentID, node, 1);
}
void WJTREE::initGUI()
{
	setOverflow(Wt::Overflow::Auto, Wt::Orientation::Horizontal);

	auto vLayoutUnique = make_unique<Wt::WVBoxLayout>();
	auto vLayout = this->setLayout(std::move(vLayoutUnique));

	auto labelUnique = make_unique<Wt::WText>(" ");
	auto label = vLayout->insertWidget(index::Label, std::move(labelUnique));

	auto treeUnique = make_unique<Wt::WTree>();
	auto tree = vLayout->insertWidget(index::Tree, std::move(treeUnique));
	tree->setObjectName("Tree");
	tree->setSelectionMode(Wt::SelectionMode::Single);

	vLayout->insertStretch(index::Stretch, 1);
}
void WJTREE::makeTreeGeo(const vector<vector<string>>& vvsGeo, JTREE*& jt)
{
	// Populates the model's JTREE using a database table. vvsGeo should have form
	// [region index][GEO_CODE, sRegion, GEO_LEVEL, Ancestor0, Ancestor1, ...]
	
	jt->reset();
	
	unordered_map<string, int> mapGCID;
	string sParent;
	int parentID;
	JNODE jnRoot = jt->getRoot();
	int numRegion = (int)vvsGeo.size();
	for (int ii = 0; ii < numRegion; ii++) {
		JNODE jn(3);
		mapGCID.emplace(vvsGeo[ii][0], jn.ID);
		jn.vsData[0] = vvsGeo[ii][1];  // Region name.
		jn.vsData[1] = vvsGeo[ii][0];  // GEO_CODE.
		jn.vsData[2] = vvsGeo[ii][2];  // GEO_LEVEL.
		if (vvsGeo[ii].size() < 4) {  // This row is the root.
			jt->addChild(jnRoot.ID, jn);
		}
		else {
			sParent = vvsGeo[ii].back();
			parentID = mapGCID.at(sParent);
			jt->addChild(parentID, jn);
		}
	}

	if (defaultRowExpansion >= 0) { jt->setExpandGeneration(defaultRowExpansion); }
}
/*
void WJTREE::populateTree(JTREE*& jt, int parentID, Wt::WTreeNode*& parentNode)
{
	// Recursive function that takes an existing node and makes its children.

	string sID;
	Wt::WTreeNode* child = nullptr;
	vector<int> childrenID = jt->getChildrenID(parentID);
	int numChildren = (int)childrenID.size();
	if (numChildren == 0) { return; }
	for (int ii = 0; ii < numChildren; ii++) {
		JNODE& jn = jt->getNode(childrenID[ii]);
		sID = to_string(jn.ID);

		auto childUnique = make_unique<Wt::WTreeNode>(jn.vsData[0]);
		child = childUnique.get();
		parentNode->addChildNode(move(childUnique));
		child->setObjectName(sID);
		if (jn.expanded) { child->expand(); }

		populateTree(jt, childrenID[ii], child);
	}
}
*/
void WJTREE::setLabel(const string& cata)
{
	sCata = cata;
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::Label);
	auto label = (Wt::WText*)wlItem->widget();
	label->setText("Displaying geographic region tree for catalogue " + cata);
}
void WJTREE::setTree(const vector<vector<string>>& vvsGeo)
{
	// Converts a raw database geo table into a JTREE, which then populates the
	// tree widget (2 inital generations).
	if (vvsGeo.size() == 0) { return; }	

	// Database table -> JTREE.
	JTREE* jt = jtGeo.get();
	makeTreeGeo(vvsGeo, jt);

	// JTREE -> widget.
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::Tree);
	auto tree = (Wt::WTree*)wlItem->widget();
	JNODE& jnRoot = jt->getRoot();
	string sName = "$" + to_string(jnRoot.ID) + "$";
	auto rootUnique = make_unique<Wt::WTreeNode>(" ");
	auto root = rootUnique.get();
	tree->setTreeRoot(std::move(rootUnique));
	root->setObjectName(sName);
	addChildren(jnRoot.ID, root, 2);

	/*
	JTREE* jt = jtGeo.get();
	JNODE& jnRoot = jt->getRoot();		
	Wt::WTreeNode* treeRoot = nullptr;
	
	auto vLayout = (Wt::WVBoxLayout*)this->layout();
	auto wlItem = vLayout->itemAt(index::Tree);
	auto tree = (Wt::WTree*)wlItem->widget();
	
	if (jnRoot.vsData[0].size() > 0) {
		const Wt::WString wsName(jnRoot.vsData[0]);
		auto treeRootUnique = make_unique<Wt::WTreeNode>(wsName);
		treeRoot = treeRootUnique.get();
		tree->setTreeRoot(std::move(treeRootUnique));
	}
	else {
		const Wt::WString wsName(" ");
		auto treeRootUnique = make_unique<Wt::WTreeNode>(wsName);
		treeRoot = treeRootUnique.get();
		tree->setTreeRoot(std::move(treeRootUnique));
	}

	treeRoot->setNodeVisible(0);
	treeRoot->expand();
	populateTree(jt, jnRoot.ID, treeRoot);
	*/

}
