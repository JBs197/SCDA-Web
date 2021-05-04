#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "jfunc.h"

using namespace std;

class JTREE
{
	int count;
	string errorPath = sroot + "\\SCDA Error Log.txt";
	JFUNC jf;
	unordered_map<string, int> mapS;  // Index from string.
	unordered_map<int, int> mapI;  // Index from int.
	string pathRoot;  // Path to root folder. 
	string root;  // Name given to the root tree item.
	vector<vector<int>> treeSTanc;  // Form [index][anc1, anc2, ...]
	vector<vector<int>> treeSTdes;  // Form [index][des1, des2, ...]
	vector<string> treePL;  // snames by index.
	vector<int> treePLi;  // inames by index.

public:
	JTREE() {}
	~JTREE() {}

	void deleteNodeHelper(int index);
	void init(string root, string pathRoot);
	void inputTreeSTPL(vector<vector<int>>& tree_st, vector<string>& tree_pl, vector<int>& tree_ipl);
	string getRootName();

	// TEMPLATES 

	template<typename ... Args> void addChild(string& sname, int& iname, Args& ... args) {}
	template<> void addChild<string>(string& sname, int& iname, string& sparent)
	{
		int parentIndex;
		try { parentIndex = mapS.at(sparent); }
		catch (out_of_range& oor) { jf.err("mapS-jtree.addChild"); }
		treeSTdes[parentIndex].push_back(count);
		mapS.emplace(sname, count);
		mapI.emplace(iname, count);
		treePL.push_back(sname);
		treePLi.push_back(iname);
		treeSTdes.push_back(vector<int>());
		if (treeSTanc[parentIndex][0] >= 0)  // If parent is not root...
		{
			treeSTanc.push_back(treeSTanc[parentIndex]);
			treeSTanc[count].push_back(parentIndex);
		}
		else
		{
			treeSTanc.push_back({ 0 });
		}
		count++;
	}
	template<> void addChild<int>(string& sname, int& iname, int& iparent)
	{
		int parentIndex;
		try { parentIndex = mapI.at(iparent); }
		catch (out_of_range& oor) { jf.err("mapI-jtree.addChild"); }
		treeSTdes[parentIndex].push_back(count);
		mapS.emplace(sname, count);
		mapI.emplace(iname, count);
		treePL.push_back(sname);
		treePLi.push_back(iname);
		treeSTdes.push_back(vector<int>());
		if (treeSTanc[parentIndex][0] >= 0)  // If parent is not root...
		{
			treeSTanc.push_back(treeSTanc[parentIndex]);
			treeSTanc[count].push_back(parentIndex);
		}
		else
		{
			treeSTanc.push_back({ 0 });
		}
		count++;
	}

	template<typename ... Args> void addChildren(vector<string>& snames, vector<int>& inames, Args& ... args) {}
	template<> void addChildren<string>(vector<string>& snames, vector<int>& inames, string& sparent)
	{
		for (int ii = 0; ii < snames.size(); ii++)
		{
			addChild(snames[ii], inames[ii], sparent);
		}
	}
	template<> void addChildren<int>(vector<string>& snames, vector<int>& inames, int& iparent)
	{
		for (int ii = 0; ii < snames.size(); ii++)
		{
			addChild(snames[ii], inames[ii], iparent);
		}
	}

	template<typename ... Args> void deleteChildren(Args& ... args) {}
	template<> void deleteChildren<string>(string& sParent)
	{
		int indexNode;
		try { indexNode = mapS.at(sParent); }
		catch (out_of_range& oor) { return; }
		vector<int> vKids = treeSTdes[indexNode];
		for (int ii = vKids.size() - 1; ii >= 0; ii--)
		{
			deleteNodeHelper(vKids[ii]);
			treeSTdes[indexNode].erase(treeSTdes[indexNode].begin() + ii);
		}
	}

    template<typename ... Args> void listChildren(Args& ... args) {}
    template<> void listChildren<int, vector<int>>(int& iparent, vector<int>& ikids)
    {
        int pIndex;
        try { pIndex = mapI.at(iparent); }
        catch (out_of_range &oor) { jf.err("mapI-jtree.listChildren"); }

        ikids = treeSTdes[pIndex];
    }
    template<> void listChildren<int, vector<string>>(int& iparent, vector<string>& skids)
    {
        int pIndex;
        try { pIndex = mapI.at(iparent); }
        catch (out_of_range &oor) { jf.err("mapI-jtree.listChildren"); }

        vector<int> ikids = treeSTdes[pIndex];
        skids.resize(ikids.size());
        for (int ii = 0; ii < skids.size(); ii++)
        {
            skids[ii] = treePL[ikids[ii]];
        }
    }
    template<> void listChildren<string, vector<int>>(string& sparent, vector<int>& ikids)
    {
        int pIndex;
        try { pIndex = mapS.at(sparent); }
        catch (out_of_range &oor) { jf.err("mapS-jtree.listChildren"); }

        vector<int> vtemp = treeSTdes[pIndex];
		ikids.resize(vtemp.size());
		for (int ii = 0; ii < ikids.size(); ii++)
		{
			ikids[ii] = treePLi[vtemp[ii]];  // 16?? 17???
		}
    }
    template<> void listChildren<string, vector<string>>(string& sparent, vector<string>& skids)
    {
        int pIndex;
        try { pIndex = mapS.at(sparent); }
        catch (out_of_range &oor) { jf.err("mapS-jtree.listChildren"); }

        vector<int> ikids = treeSTdes[pIndex];
        skids.resize(ikids.size());
        for (int ii = 0; ii < skids.size(); ii++)
        {
            skids[ii] = treePL[ikids[ii]];
        }
    }

};