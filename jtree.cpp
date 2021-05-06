#include "jtree.h"

void JTREE::deleteNodeHelper(int index)
{
	vector<int> vKids = treeSTdes[index];
	if (vKids.size() == 0)
	{
		mapS.erase(treePL[index]);
		mapI.erase(treePLi[index]);
		treePL[index].clear();
		treePLi[index] = -2;
		treeSTanc[index] = { -2 };
	}
	else
	{
		for (int ii = vKids.size() - 1; ii >= 0; ii--)
		{
			deleteNodeHelper(vKids[ii]);
			treeSTdes[index].erase(treeSTdes[index].begin() + ii);
		}
	}
}
void JTREE::init(string rt, string pathR)
{
	root = rt;
	pathRoot = pathR;
	treePL.clear();
	treeSTanc.clear();
	treeSTdes.clear();
	mapI.clear();
	mapS.clear();
	treePL.push_back(rt);
	treePLi.push_back(-1);
	treeSTanc.push_back({ -1 });
	treeSTdes.push_back(vector<int>());
	mapI.emplace(-1, 0);
	mapS.emplace(rt, 0);
	count = 1;

	extHierarchy = { ".pdf", ".png", ".bin" };
}
void JTREE::inputTreeSTPL(vector<vector<int>>& tree_st, vector<string>& tree_pl, vector<int>& tree_ipl)
{
	// NOTE: This function will not insert the STPL tree's first entry, as it is presumed
	// to be the tree root which the JTREE builds itself during initialization.
	if (tree_st[0][0] != 0) { jf.err("Bad root-jt.inputTreeSTPL"); }
	int iRoot = -1;
	int indexGen, indexTree, pivot, indexParent;
	vector<int> kids;
	vector<vector<int>> genLayers;
	genLayers.push_back(tree_st[0]);
	genLayers[0].erase(genLayers[0].begin());
	int numKids = genLayers[0].size();
	for (int ii = 0; ii < numKids; ii++)
	{
		addChild(tree_pl[genLayers[0][ii]], tree_ipl[genLayers[0][ii]], iRoot);  // Root always has index -1.
	}
	while (numKids > 0)
	{
		indexGen = genLayers.size();
		genLayers.push_back(vector<int>());
		numKids = 0;
		for (int ii = 0; ii < genLayers[indexGen - 1].size(); ii++)  // For every parent...
		{
			indexTree = genLayers[indexGen - 1][ii];
			pivot = jf.getPivot(tree_st[indexTree]);
			if (pivot >= tree_st[indexTree].size() - 1) { continue; }
			kids.clear();
			kids.insert(kids.begin(), tree_st[indexTree].begin() + pivot + 1, tree_st[indexTree].end());
			for (int jj = 0; jj < kids.size(); jj++)  // For every child...
			{
				genLayers[indexGen].push_back(kids[jj]);
				numKids++;
				addChild(tree_pl[kids[jj]], tree_ipl[kids[jj]], tree_pl[indexTree]);
			}
		}
	}
}
int JTREE::getHierarchy(string& ext)
{
	for (int ii = 0; ii < extHierarchy.size(); ii++)
	{
		if (ext == extHierarchy[ii])
		{
			return ii;
		}
	}
	return -1;
}
string JTREE::getRootName()
{
	return treePL[0];
}
