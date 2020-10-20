#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <unordered_set>

#define BUFFER_SIZE 4096
using std::ifstream;
using std::ios;
using std::string;
using std::cout;
using std::stoi;
using std::stod;
using std::vector;
using std::pair;
using std::unordered_set;
using std::set;
class LexicalAna {
private:
	ifstream file;
	char C=0;
	string token;
	int state=0;
	int end = 0;	//while循环结束标志
	int note_line = 0, note_lines = 0;		//是否处于识别注释行或注释块状态
	char str_token;//识别字符串所用的标记 单引号还是双引号
	char buff1[BUFFER_SIZE+1], buff2[BUFFER_SIZE+1];		//两个缓冲区
	int buff_status = -1,cur, letter_new_line = 0,tem_letter_new_line;
		//缓冲区状态 缓冲区下标 当前字符所在行位置
	
	int letters = 0, lines = 0;//统计总字符数 行数
	int counts[4] = { 0 };//用于计数 id num 保留字 字符串
	vector<pair<string, string>> table;	//保存所有识别出的二元组
	unordered_set<string> symbol_table;	//保存标识符
	vector<string> errors;		//保存错误信息
	//保留字
	set<string> keys = { "auto","break","case","char","const","continue","default",
		"do","double","else","enum","extern","float","for","goto","if","int","long",
		"register","return","short","signed","sizeof","static","struct","switch",
		"typedef","union","unsigned","void","volatile","while","NULL"};
	void _getchar();
	void retract();
	inline bool is_letter() {
		return ((C >= 'a'&&C <= 'z') || (C >= 'A'&&C <= 'Z') || C=='_');
	}
	inline bool is_digit() {
		return C >= '0'&&C <= '9';
	}
	inline void cat() {
		token += C;
	}
	void error(string str);	//报错
public:
	LexicalAna(string file_name) {	//构造函数参数为文件路径
		buff1[BUFFER_SIZE] = buff2[BUFFER_SIZE] = '\0';
		file.open(file_name, ios::in);
	}
	bool open_success() {	//返回文件是否打开成功
		return file.is_open();
	}
	void run();			//运行词法分析
	void show_res();	//输出词法分析结果
};