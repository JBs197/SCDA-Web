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
	vector<vector<int>> treeSTanc;  // Form [index][anc1, anc2, ...]
	vector<vector<int>> treeSTdes;  // Form [index][des1, des2, ...]
	vector<string> treePL;  // Names by index.

public:
	JTREE() {}
	~JTREE() {}

	void init(string root);

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
		if (treeSTdes[parentIndex][0] < 0) { treeSTdes[parentIndex].clear(); }
		treeSTdes[parentIndex].push_back(count);
		mapS.emplace(sname, count);
		mapI.emplace(iname, count);
		treePL.push_back(sname);
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

        ikids = treeSTdes[pIndex];
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
