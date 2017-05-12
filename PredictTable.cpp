#include "PredictTable.h"

PredictTable::PredictTable(string filename){

    ifstream infile(filename);

    if(!infile.is_open()){
        cout << "Error Open File!" << endl;
    }

    int i = 0;
    string line;
    while(getline(infile,line))
    {
        int found = line.find('-');
        string nontermi_symbol = line.substr(0,found);
        if (i == 0) begin_symbol = nontermi_symbol;
        nontermi_symbol_set.insert(nontermi_symbol);
        fi_visited[nontermi_symbol] = false;
        fo_visited[nontermi_symbol] = false;
        int morethan_pos = line.find('>');
        line = line.substr(morethan_pos+1);
        size_t pipe_pos = line.find('|');
        while (pipe_pos != string::npos)
        {
            productions[nontermi_symbol].push_back(line.substr(0, pipe_pos));
            line = line.substr(pipe_pos + 1);
            pipe_pos = line.find('|');
        }
        productions[nontermi_symbol].push_back(line);
        ++i;
    }
    infile.close();

    //消除直接左递归
    for (set<string>::iterator it=nontermi_symbol_set.begin(); it!=nontermi_symbol_set.end(); ++it){
        for (size_t i=0; i<productions[*it].size(); ++i){
            string s = productions[*it][i];
            string s1 = get_next_symbol(s);
            if (s1 == *it){
                productions[*it].erase(productions[*it].begin()+i);
                for (size_t j=0; j<productions[*it].size(); ++j)
                {
                    string s2 = productions[*it][j];
                    string s3 = get_next_symbol(s2);
                    if (s3 != *it){
                        productions[*it].push_back(productions[*it][j]+*it+"'");
                        productions[*it+"'"].push_back(s+*it+"'");
                        productions[*it+"'"].push_back("#");
                        productions[*it].erase(productions[*it].begin()+j);
                    }

                }
                nontermi_symbol_set.insert(*it+"'");
            }
        }

    }

    for (set<string>::iterator it=nontermi_symbol_set.begin(); it!=nontermi_symbol_set.end(); ++it)
    {
        for (size_t i=0; i<productions[*it].size(); ++i)
        {
            string s = productions[*it][i];
            while (s.size()!=0)
            {
                string s1 = get_next_symbol(s);
                if(is_termi_symbol(s1))
                termi_symbol_set.insert(s1);
            }
        }
    }
    termi_symbol_set.insert("$");
}

bool PredictTable::is_termi_symbol(string s)
{
    if (nontermi_symbol_set.find(s)!=nontermi_symbol_set.end()) return false;
        else return true;
}

void PredictTable::get_first_set()
{
    for (set<string>::iterator it = nontermi_symbol_set.begin(); it != nontermi_symbol_set.end(); ++it)
        if (!fi_visited[*it]) get_first(*it);
}

void PredictTable::get_follow_set()
{
    for (set<string>::iterator it = nontermi_symbol_set.begin(); it != nontermi_symbol_set.end(); ++it)
        if (!fo_visited[*it]) get_follow(*it);
}

void PredictTable::get_first(string target)
{
    for (unsigned int i=0; i<productions[target].size(); ++i)
    {
        set<string> string_first_set = First(productions[target][i]);  //求出first(表达式)
        first_set[target].insert(string_first_set.begin(), string_first_set.end());
    }
    fi_visited[target] = true;
}

set<string> PredictTable::First(string expression)
{
    bool flag;

    set<string> string_first_set;
    while (expression.size()!=0)
    {
        string s = get_next_symbol(expression);
        flag = 0;
        if (is_termi_symbol(s))  //对于终结符，直接加入first
        {
            string_first_set.insert(s);
            break;
        }
        else
        {
                if (!fi_visited[s])    get_first(s);
                set<string>::iterator it;
                for (it = first_set[s].begin(); it != first_set[s].end(); it++)
                {
                    if (*it == empty_symbol)
                        flag = 1;
                    else
                        string_first_set.insert(*it);   //FIRST(Y)中的非空符号就加入FIRST(X)
                }
                if (flag == 0)
                    break;
                else continue;
        }  
    }
    if (expression.size() == 0 && flag == 1)
        string_first_set.insert(empty_symbol);  //表达式可推导到空符号
    return string_first_set;
}

string PredictTable::get_next_symbol(string &expression)
{
    size_t max_len = 0;
    if (expression.size()==0) return "";
    string match_str = expression.substr(0,1);
    for (set<string>::iterator it = nontermi_symbol_set.begin(); it!=nontermi_symbol_set.end(); ++it)
        if (expression.find(*it) == 0 && it->size()>max_len){
            max_len = it->size();
            match_str = *it;
        }
    expression = expression.substr(match_str.size());
    return match_str;
}

void PredictTable::get_follow(string target)
{
    if (target == begin_symbol)
        follow_set[begin_symbol].insert("$");
    for (map<string, vector<string> >::iterator it=productions.begin(); it!=productions.end(); ++it)
        for (size_t i=0; i<it->second.size(); ++i){
            size_t found = it->second[i].find(target);
            if (found != string::npos && (found + target.size() != it->second[i].size()))
            {
                set<string> string_first_set = First(it->second[i].substr(found+1));
                for (set<string>::iterator it2=string_first_set.begin(); it2!=string_first_set.end(); ++it2)
                    if (*it2 != empty_symbol){
                        follow_set[target].insert(*it2);
                    }
                if (string_first_set.find(empty_symbol)!=string_first_set.end() && it->first != target)
                {
                    if (!fo_visited[it->first])
                        get_follow(it->first);
                    string s = it->first;
                    for (set<string>::iterator it3 = follow_set[s].begin(); it3 != follow_set[s].end(); it3++)
                        follow_set[target].insert(*it3);
                }
            }
            else if (found != string::npos && (found + target.size() == it->second[i].size()) && it->first != target)
            {
                if (!fo_visited[it->first])
                    get_follow(it->first);
                string s = it->first;
                for (set<string>::iterator it = follow_set[s].begin(); it != follow_set[s].end(); it++)
                    follow_set[target].insert(*it);
            }
        }

    fo_visited[target] = true;
}

void PredictTable::generateTable()
{
    get_first_set();
    //cout << "get_first finished" << endl;
    get_follow_set();
    //cout << "get_follow finished" << endl;
    ofstream first_file("first_set.txt");
    
    for (map<string, set<string> >::iterator it=first_set.begin(); it != first_set.end(); ++it){
        first_file << left << setw(5)<< it->first << ": {";
        for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            first_file << *it2 << " ";
        first_file << "}" << endl;
    }
    first_file.close();

    ofstream follow_file("follow_set.txt");
    for (map<string, set<string> >::iterator it=follow_set.begin(); it != follow_set.end(); ++it){
        follow_file << left << setw(5)<< it->first << ": {";
        for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            follow_file << *it2 << " ";
        follow_file << "}" << endl;
    }
    follow_file.close();

    map<pair<string, string>, string> table;
    for (set<string>::iterator it = nontermi_symbol_set.begin(); it!=nontermi_symbol_set.end(); ++it){
        string target = *it;
        for (size_t i=0; i<productions[target].size(); ++i)
        {
            set<string> string_first_set = First(productions[target][i]);
            for (set<string>::iterator it2 = string_first_set.begin(); it2 != string_first_set.end(); ++it2)
                if (*it2 != empty_symbol) table[make_pair(target,*it2)] = target + "->" + productions[target][i];

            if (string_first_set.find(empty_symbol)!=string_first_set.end()){
                for (set<string>::iterator it3 = follow_set[target].begin(); it3 != follow_set[target].end(); ++it3){
                    table[make_pair(target,*it3)] = target + "->" + productions[target][i];
                }
            }

        } 
    }

    termi_symbol_set.erase("#");

    ofstream outfile("predict_table.txt");
    outfile << left << setw(10) << " ";
    for (set<string>::iterator it1=termi_symbol_set.begin(); it1!=termi_symbol_set.end(); ++it1)
        outfile << left << setw(10) << *it1;
    outfile << endl;
    for (set<string>::iterator it=nontermi_symbol_set.begin(); it!=nontermi_symbol_set.end(); ++it){
        outfile << left << setw(10) << *it;
        for (set<string>::iterator it1=termi_symbol_set.begin(); it1!=termi_symbol_set.end(); ++it1){
            if (table.find(make_pair(*it, *it1)) == table.end()) outfile << left << setw(10) << "Error";
                else outfile << left << setw(10) << table[make_pair(*it, *it1)];
        }
        outfile << endl;
    }    
    outfile.close();

    termi_symbol_set.insert("#");
}
