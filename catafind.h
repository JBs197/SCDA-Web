#pragma once
#include "catafilter.h"
#include "sqlfunc.h"

// This stateless class handles the server task of determining which catalogue(s) 
// satisfy the user's chosen criteria. 
class CataFind
{
	std::list<std::string> allCata;
	const std::string dbPath;
	SQLFUNC sf;

	void err(std::string message);
	void filterCategory(std::list<std::string>& listCata, CataFilter& cf);
	void filterDifferentiator(std::list<std::string>& listCata, CataFilter& cf, std::vector<std::string>& vsYear);
	void filterParameter(std::list<std::string>& listCata, CataFilter& cf, std::vector<std::string>& vsYear);
	void filterTopic(std::list<std::string>& listCata, CataFilter& cf, std::vector<std::string>& vsYear);
	void filterYear(std::list<std::string>& listCata, CataFilter& cf);
	void initList();

public:
	CataFind(std::string& dbpath);
	~CataFind() = default;

	void applyFilter(std::vector<std::string>& vsCata, CataFilter& cf);
};