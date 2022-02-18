#include "wjtree.h"

using namespace std;

void WJTREE::err(string message)
{
	string errorMessage = "WJTREE error:\n" + message;
	JLOG::getInstance()->err(errorMessage);
}
Wt::WTreeNode* WJTREE::findNode(int jtreeID)
{
	if (tree == nullptr) { return nullptr; }
	vector<int> viGenealogy = jt.getGenealogyID(jtreeID);
	viGenealogy.push_back(jtreeID);
	string sIDj, sIDw;
	vector<Wt::WTreeNode*> vNode;
	Wt::WTreeNode* node = tree->treeRoot();
	int index, numNode;
	for (int ii = 0; ii < viGenealogy.size(); ii++) {
		sIDj = to_string(viGenealogy[ii]);
		vNode = node->childNodes();
		numNode = (int)vNode.size();
		for (int jj = 0; jj < numNode; jj++) {
			if (vNode[jj]->objectName() == sIDj) {
				node = vNode[jj];
				break;
			}
		}
	}
	return node;
}
void WJTREE::init(int width, int height)
{
	wlAuto = Wt::WLength::Auto;
	maxWidth = Wt::WLength(width - 10);
	maxHeight = Wt::WLength(height);
	this->setMaximumSize(maxWidth, maxHeight);
	this->setOverflow(Wt::Overflow::Hidden, Wt::Orientation::Horizontal);

	auto vLayout = make_unique<Wt::WVBoxLayout>();
	auto treeUnique = make_unique<Wt::WTree>();
	tree = treeUnique.get();
	vLayout->addWidget(move(treeUnique));
	vLayout->addStretch(1);
	this->setLayout(move(vLayout));

	tree->setMaximumSize(maxWidth, maxHeight);
	tree->setObjectName("Tree");
	tree->setSelectionMode(Wt::SelectionMode::Single);
}
