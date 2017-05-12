#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <utility>
#include <iomanip>
using namespace std;
class PredictTable
{
private:
	map<string,vector<string> > productions;  //产生式
	map<string, set<string> > first_set;    //first集
	map<string, set<string> > follow_set;   //follow集
	set<string> termi_symbol_set;     //终结符集
	set<string> nontermi_symbol_set;      //非终结符集
	map<string, bool> fi_visited;		
	map<string, bool> fo_visited;
	string begin_symbol;				//开始符
	const string empty_symbol = "#";	//空符号
public:
	PredictTable(string filename);
	~PredictTable(){};
	bool is_termi_symbol(string s);
	void get_first_set();
	void get_first(string target);
	set<string> First(string expression);
	void get_follow_set();
	void get_follow(string target);
	void generateTable();
	string get_next_symbol(string &expression);
};