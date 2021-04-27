#include "jtree.h"

void JTREE::deleteChildrenHelper(int nodeIndex)
{
	vector<int> vKids = treeSTdes[nodeIndex];
	if (vKids.size() < 1)
	{
		string sNode = treePL[nodeIndex];
		int iNode = treePLi[nodeIndex];
		mapS.erase(sNode);
		mapI.erase(iNode);
		treeSTanc[nodeIndex] = { -2 };
		treePL[nodeIndex].clear();
		treePLi[nodeIndex] = -2;
	}
	else
	{
		for (int ii = 0; ii < vKids.size(); ii++)
		{
			deleteChildrenHelper(vKids[ii]);
		}
		deleteChildrenHelper(nodeIndex);
	}
}
void JTREE::init(string root)
{
	treePL.clear();
	treePLi.clear();
	treeSTanc.clear();
	treeSTdes.clear();
	mapI.clear();
	mapS.clear();
	treePL.push_back(root);
	treePLi.push_back(-1);
	treeSTanc.push_back({ -1 });
	treeSTdes.push_back({ -1 });
	mapI.emplace(-1, 0);
	mapS.emplace(root, 0);
	count = 1;
}
