#pragma once
#include <mutex>
#include <set>
#include <thread>
#include "sqlite3.h"
#include "jfunc.h"

using namespace std;
extern mutex m_err;

class SQLFUNC 
{
    bool analyze = 0;
	sqlite3* db;
    string dbPath;
    JFUNC jf;
    unordered_map<string, int> mapColType;
    set<string> columnType, setTable;
    
    void bind(string& stmt, vector<string>& param);
    void err(string message);
    void log(string message);
	void sqlerr(string func);

public:
    SQLFUNC() {}
	~SQLFUNC() {}

    void addColumn(string tname, string colTitle, string colType);
    void allTables(vector<string>& vsTable);
    void allTables(set<string>& setTableList);
    int count(string tname);
    void createTable(string tname, vector<vector<string>>& vvsColTitle);
    void deleteRow(string tname, vector<string> conditions);
    void clearTable(string tname);
    void dropTable(string tname);
    void executor(string stmt);
    void executor(vector<string> stmts);
    void executor(string stmt, string& result);
    void executor(string stmt, wstring& result);
    void executor(string stmt, vector<string>& results);
    void executor(string stmt, vector<vector<string>>& results);
    void executor(string stmt, vector<vector<wstring>>& results);
    vector<vector<string>> getColTitles(string tname);
    string getColType(string tname, int colIndex);
    string getColType(string tname, string colTitle);
    int getNumCol(string tname);
    int getNumRows(string tname);
    vector<string> getTableListFromRoot(string root);
    vector<string> getTableList(string search);
    void init(string db_path);
    void insert(string tname, vector<string>& rowData);
    void insert(string tname, vector<vector<string>>& vvsColTitle, vector<string>& rowData);
    void insert(string tname, vector<vector<string>>& vvsColTitle, vector<vector<string>>& rowData);
    void insertPrepared(vector<string>& stmts);
    void insertPreparedBind(vector<string>& stmtAndParams);
    void insertPreparedStartStop(vector<string>& stmts, int start, int stop);
    void refreshTableList();
    void removeCol(string tname, string colTitle);
    void removeRow(string tname, vector<string> conditions);
    void safeCol(string tname, vector<vector<string>>& vvsColTitle);
    int sclean(string&, int);
    int select(vector<string> search, string tname, string& result);
    int select(vector<string> search, string tname, vector<string>& results);
    int select(vector<string> search, string tname, vector<vector<string>>& results);
    int select(vector<string> search, string tname, vector<vector<wstring>>& results);
    int select(vector<string> search, string tname, string& result, vector<string> conditions);
    int select(vector<string> search, string tname, wstring& result, vector<string> conditions);
    int select(vector<string> search, string tname, vector<string>& results, vector<string> conditions);
    int select(vector<string> search, string tname, vector<vector<string>>& results, vector<string> conditions);
    int select(vector<string> search, string tname, vector<vector<wstring>>& results, vector<string> conditions);
    int selectOrderBy(vector<string> search, string tname, vector<string>& results, string orderby);
    int selectOrderBy(vector<string> search, string tname, vector<vector<string>>& results, string orderby);
    void selectTree(string tname, vector<vector<int>>& tree_st, vector<string>& tree_pl);
    string sqlErrMsg();
    bool tableExist(string tname);
    void update(string tname, vector<string> revisions, vector<string> conditions);
};

