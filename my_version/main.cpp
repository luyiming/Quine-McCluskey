#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#include <set>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

//typedef map<string, bool> Table;
using Table = map<string, bool>; //C++11标准
const int maxn = 11;

int hammingDistance(string v1, string v2);	//Calculate hamming distance of two integer numbers
Table create_next_table(Table &table);	//创建包含主蕴含项的表
void createPrimeImplicantTable(vector<Table> &tables, vector<string> &prim); //从包含主蕴含项的表中提取出主蕴含项
bool match(string s1, string s2);		
void quine_mccluskey(vector<string> &prim, vector<string> task_, vector<vector<string> > &ans);
set<string> simplify(vector<string> &task, vector<string> &prim);	//化简质主蕴含项
void display(set<string> &pre_res, vector<vector<string> > &ans, vector<Table> &tables);	//output function

char itoc[maxn] = { 'A','B','C','D','E','F','G','H','I','J','K' };

int main()
{
	vector<Table> tables;
	vector<string> prim;    //存储主蕴涵项
	//Table first_table = {{ "1001",true }, {"1011", true}, { "1111", true} ,{ "0111", true }, {"0101", true } ,{"0001", true }};
	cout << "此程序中用01字串表示逻辑表达式,如:\n1111代表ABCD，11010代表AB~CD~E，001110代表~A~BCDE~F\n" << endl;
	cout << "输入数据为等长01字串" << endl;
	cout << "样例输入:0000 0001 0011 0010 1101 1011 1111" << endl;
	cout << "样例输出:" << endl;
	cout << "化简结果为:\n~A~B + ABD + ~BCD\n~A~B + ABD + ACD\n\n";
	cout << "样例输入:1100 1101 1111 1110 0101 0111" << endl;
	cout << "样例输出:" << endl;
	cout << "化简结果为:\nBD+AB\n\n";
	cout << "样例输入:11 10 00 01" << endl;
	cout << "样例输出:" << endl;
	cout << "化简结果为:\n1\n\n";

	cout << "请输入子串个数:";
	
	int n;
	cin >> n;
	cout << "请输入" << n << "个01子串:\n";
	
	Table first_table;
	
	while (n--)
	{
		string x;
		cin >> x;
		first_table[x] = true;
	}
	cout << endl;
	
	tables.push_back(first_table);
	int index = 0;
	for (;;)
	{
		Table t = create_next_table(tables.at(index));
		index++;
		if (t.size() == 0)
			break;
		tables.push_back(t);
	}

	createPrimeImplicantTable(tables, prim);

	vector<string> task; //task保存需要化简的式子,即输入的原始式子
	for (auto it = tables[0].begin(); it != tables[0].end(); ++it)
		task.push_back(it->first);

	set<string> pre_res = simplify(task, prim); //去掉所有的质主蕴含项,

	vector<vector<string> > ans;
	quine_mccluskey(prim, task, ans);

	display(pre_res, ans, tables);
	system("pause");
	return 0;
}


void quine_mccluskey(vector<string> &prim, vector<string> task_, vector<vector<string> > &ans)
{
	int min = prim.size();
	int index = 0;
	ans.push_back(vector<string>());
	int maxn = (1 << prim.size()) - 1;
	for (int next_index = 0; next_index <= maxn; next_index++)
	{
		vector<string> task(task_);
		for (int i = 0; i < prim.size(); ++i)
		{
			if ((1 << i) & next_index)
			{
				ans[index].push_back(prim[i]);
				for (auto it = task.begin(); it != task.end(); )
				{
					if (match(*it, prim[i]) == true)
						it = task.erase(it);
					else
						++it;
				}
			}
		}
		if (task.empty())
		{
			if (ans[index].size() < min)
			{
				min = ans[index].size();
				index++;
				ans.push_back(vector<string>());

			}
			else if (ans[index].size() == min)
			{
				index++;
				ans.push_back(vector<string>());
			}
			else
			{
				ans[index].clear();
			}
		}
		else if (!task.empty())
			ans[index].clear();
	}
	for (auto it = ans.begin(); it != ans.end(); )
	{
		if (it->size() > min || it->size() == 0)
			it = ans.erase(it);
		else
			++it;
	}
}
set<string> simplify(vector<string> &task, vector<string> &prim)
{
	set<string> ans;
	for (auto it1 = task.begin(); it1 != task.end(); )
	{
		int cnt = 0;
		auto t = prim.begin();
		for (auto it2 = prim.begin(); it2 != prim.end(); ++it2)
		{
			if (match(*it1, *it2))
			{
				cnt++;
				t = it2;
			}
		}
		if (cnt == 1)
		{
			it1 = task.erase(it1);
			ans.insert(*t);
		}
		else
			++it1;
	}
	for (auto it = ans.begin(); it != ans.end(); ++it)
	{
		for (auto it2 = task.begin(); it2 != task.end(); )
		{
			if (match(*it2, *it) == true)
				it2 = task.erase(it2);
			else
				it2++;
		}
	}
	for (auto it = prim.begin(); it != prim.end(); )
	{
		if (ans.count(*it) == 1)
			it = prim.erase(it);
		else
			++it;
	}
	return ans;
}
bool match(string s1, string s2)
{
	for (int i = 0; i < s1.size(); ++i)
	{
		if (s1[i] != s2[i] && s2[i] != '-')
			return false;
	}
	return true;
}

int hammingDistance(string v1, string v2)
{
	int cnt = 0;
	for (int i = 0; i < v1.length(); ++i)
		if (v1[i] != v2[i])
			cnt++;
	return cnt;
}

void createPrimeImplicantTable(vector<Table> &tables, vector<string> &prim)
{
	for (int i = 0; i < tables.size(); ++i)
	{
		for (auto j = tables[i].begin(); j != tables[i].end(); ++j)
		{
			if (j->second == true)
				prim.push_back(j->first);
		}
	}
}

Table create_next_table(Table &table)
{
	Table next_table;
	for (auto i = table.begin(); i != table.end(); ++i)
	{
		auto j = i;
		j++;
		for (; j != table.end(); ++j)
		{
			if (hammingDistance(i->first, j->first) == 1)
			{
				string tmp;
				for (int k = 0; k < i->first.length(); ++k)
				{
					if (i->first[k] != j->first[k])
						tmp += '-';
					else
						tmp += i->first[k];
				}
				next_table[tmp] = true;
				i->second = false;
				j->second = false;
			}
		}
	}
	return next_table;
}

void display(set<string> &pre_res, vector<vector<string> > &ans, vector<Table> &tables)
{
	///////////////////////////////////////////////////////////
	int len = tables[0].begin()->first.size();
	for (int i = 0; i < len; i++)
		cout << itoc[i];
	cout << " | f()\n";
	for (int i = 0; i < len; i++)
		cout << "-";
	cout << "-|----\n";

	for (int i = 0; i < (1 << len); ++i)
	{
		string tmp;
		int d = 1 << (len - 1);
		for (int j = 0; j < len; ++j)
		{
			if ((d >> j) & i)
				tmp += "1";
			else
				tmp += "0";
		}
		if (tables[0].count(tmp))
			cout << tmp << " |  1" << endl;
		else
			cout << tmp << " |  0" << endl;
	}
	cout << endl;
	////////////////////////////////////////////////////////////

	cout << "化简结果为:\n";
	for (auto it = ans.begin(); it != ans.end(); ++it)
	{
		for (auto it2 = pre_res.begin(); it2 != pre_res.end(); ++it2)
		{
			if (it2 != pre_res.begin())
				cout << "+";
			string s = *it2;
			for (int i = 0; i < s.size(); ++i)
			{
				if (s[i] == '1')
					cout << itoc[i];
				else if (s[i] == '0')
					cout << "~" << itoc[i];
			}
		}
		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			if (pre_res.size() != 0 || it2 != it->begin())
				cout << "+";
			string s = *it2;
			for (int i = 0; i < s.size(); ++i)
			{
				if (s[i] == '1')
					cout << itoc[i];
				else if (s[i] == '0')
					cout << "~" << itoc[i];
			}
		}
		cout << endl << endl;
	}
	if (ans.size() == 0)
	{
		if (pre_res.size() != 0)
		{
			for (auto it2 = pre_res.begin(); it2 != pre_res.end(); ++it2)
			{
				int cnt = 0;
				if (it2 != pre_res.begin())
					cout << "+";
				string s = *it2;
				for (int i = 0; i < s.size(); ++i)
				{
					if (s[i] == '1')
						cout << itoc[i];
					else if (s[i] == '0')
						cout << "~" << itoc[i];
					else if (s[i] == '-')
						cnt++;
				}
				if (cnt == s.size())
					cout << "1";
			}
			cout << endl << endl;
		}
		
		else
			cout << "该式子不可化简\n";
	}
}