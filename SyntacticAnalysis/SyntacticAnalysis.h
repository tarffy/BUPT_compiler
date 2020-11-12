#pragma once
#include <fstream>
#include <ios>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <set>
using std::ifstream;
using std::string;
using std::unordered_map;
using std::pair;
using std::vector;
using std::cout;
using std::hash;
using std::set;
struct hashfunc
{
	template<typename T, typename U>
	size_t operator() (const pair<T, U> &i) const
	{
		return hash<T>()(i.first) ^ hash<U>()(i.second);
	}
};
class SyntacticAna {
private:
	ifstream in;
	int generate_num;	//生成式的数量
	vector<string> generate_raw;	//记录原始生成式
	int symbol_num = 0;		//符号的数量
	unordered_map<string, int> symbols;		//记录所有的符号
	set<int> nonter;		//非终结符
	set<int> termi;
	int __e;
	vector<string> symbols_hash;
	vector<pair<int, vector<int> > >  generate;		//格式化所有生成式--以数字代替符号
	unordered_map<pair<int, int>, int,hashfunc> table;		//分析表
	vector<string> errors;
	vector< set<int>> first, follow;
	vector<int> stack_;
	int stack_cur;
	string input;

	inline void error(string message) {
		errors.push_back(message);
	}
	void check_token(string &token);	//检查token是否存在于symbols中 不存在则加入
	void get_generate();//从文件读入生成式
	void create_table();//根据生成式、FIRST、FOLLOW集构造分析表
	void handle_generate_raw();//处理读入的原始生成式
	void get_next_token(string &s, int &cur);//语法分析中 读取输入字符串的下一个字符
	void first_and_follow_set();//计算所有非终结符的FIRST、FOLLOW集
	void reconstruct_generate_raw();//消左递归后重新构造生成式
	void handle_recursion();//消除左递归
	void show_table_and_generate();//符号串分析前输出生成式和分析表
	
public:
	SyntacticAna(string &name);
	//构造函数 参数为文件名 构造函数中完成读入生成式 消除左递归 求FIRST、FOLLOW集 构造分析表 
	//读入Input字符串操作
	void show_res();//调试
	void show_errors();
	void solve();//进行符号串分析

};