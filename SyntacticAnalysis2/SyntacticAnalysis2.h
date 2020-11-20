#pragma once
#include <fstream>
#include <ios>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <set>
#include <queue>
using std::ifstream;
using std::string;
using std::unordered_map;
using std::pair;
using std::vector;
using std::cout;
using std::hash;
using std::set;
using std::queue;
struct hashfunc
{
	template<typename T, typename U>
	size_t operator() (const pair<T, U> &i) const
	{
		return hash<T>()(i.first) ^ hash<U>()(i.second);
	}
};

class SyntacticAna2 {
private:
	ifstream in;		//读入文件流
	int generate_num = 0;	//生成式的数量
	vector<string> generate_raw;	//记录原始生成式 消左递归后会改为消除左递归后的生成式
	int symbol_num = 0;		//符号的数量 包括终结符、非终结符、ε和$
	unordered_map<string, int> symbols;		//记录所有的符号与int的对应
	set<int> nonter;		//非终结符集合
	set<int> termi;			//终结符集合
	int __e,__$;				//记录ε在symbols中对应的数 
	vector<string> symbols_hash;	//symbols中记录所有符号后 建立所有符号int到string的对应
	vector<pair<int, vector<int> > >  generate;
	//格式化后所有生成式 pair中第一个int为生成式左边非终结符对应的int,vector<int>为生成式右面所有符号对应的int
	unordered_map<pair<int, int>,pair< int,int>, hashfunc> table;
	//分析表 pair两个int表示栈顶符号和遇到的符号 第三个int为模式 1S 2R 3goto 4ACC第四个生成式或状态 int为-1表示synch
	vector<string> errors;	//记录所有错误信息
	vector< set<int>> first, follow;	//记录所有符号的first follow集 vector下标为某符号对应的int
	vector<int> stack_;		//使用vector模拟字符串识别过程中的栈
	int stack_cur;			//记录栈顶位置
	string input;			//读入的要识别的字符串 末尾没有$会自动补上
	vector<vector<pair<vector<int>, set<int> > > >  dfa;	
	//其中每一个元素都是一个项目集 项目集中每一个元素都是一个项目 pair的fisrt为一个vector<int>
	//为LR(0)项目每个字符对应的数字 其中.为-1 pair的second代表向前看字符串中的字符的符号
	unordered_map<pair<int,int>,int,hashfunc> relation;	//代表项目集的转移方式 pair为当前项目和读到的符号 第三个int为转以后的项目集编号
	int table_conflict = 0;
	inline void error(string message) {
		errors.push_back(message);
	}
	void check_token(string &token);	//检查token是否存在于symbols中 不存在则加入
	void get_generate();		//从文件读入生成式
	void create_dfa();
	void create_table();		//根据生成式、FIRST、FOLLOW集构造分析表
	void handle_generate_raw();//处理读入的原始生成式
	void reconstruct_generate_raw();//消左递归后重新构造生成式
	void first_and_follow_set();//计算所有非终结符的FIRST、FOLLOW集
	void show_table_and_generate();//符号串分析前输出生成式和分析表
	void get_next_token(string &s, int &cur);//语法分析中 读取输入字符串的下一个字符
	void out_dfa(vector<pair<vector<int>, set<int> > > &tem);	//输出一个项目集的信息
	void closure(vector<pair<vector<int>, set<int> > > &closure);	//计算当前项目集的闭包
	int closure_check(pair<vector<int>, set<int>> &tem, vector<pair<vector<int>, set<int> > > &closure); 
	//检查当前元素是否与闭包中元素重复 返回值为0-size代表可以合并 -1重复 -2新的
	void generate_to_vector(pair<vector<int>, set<int>> &tem,int i);
	int check_vector_vector(vector<pair<vector<int>, set<int> > > &tem);//-1加入 0-n重复 
public:
	SyntacticAna2(string &name);
	//构造函数 参数为文件名 构造函数中完成读入生成式 消除左递归 求FIRST、FOLLOW集 构造分析表 
	//读入Input字符串操作
	void show_res();//调试
	void show_errors();//输出所有错误
	void solve();//进行符号串分析
};