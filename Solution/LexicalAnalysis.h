#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <unordered_set>
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
	char Ctest;
	string token;
	int state=0;
	int end = 0;
	int note_line = 0, note_lines = 0;		//是否处于注释行或注释块状态
	char str_token;//字符串的标记 单引号还是双引号
	char buff1[1025], buff2[1025];		//两个缓冲区
	int buff_status = -1,cur;
	int letters = 0, lines = 0;
	vector<pair<string, string>> table;
	unordered_set<string> symbol_table;
	vector<string> errors;
	set<string> keys = { "auto","break","case","char","const","continue","default",
		"do","double","else","enum","extern","float","for","goto","if","int","long",
		"register","return","short","signed","sizeof","static","struct","switch",
		"typedef","union","unsigned","void","volatile","while"};
	void _getchar();
	void retract();
	inline bool is_letter() {
		return ((C >= 'a'&&C <= 'z') || (C >= 'A'&&C <= 'Z') || C=='_');
	}
	inline bool is_digit() {
		return C >= '0'&&C <= '9';
	}
	inline void cat() {
		token.push_back(C);
	}
	void error(string str) {
		errors.emplace_back(str);
	}
public:
	LexicalAna(string file_name) {
		buff1[1024] = buff2[1024] = '\0';
		file.open(file_name, ios::in);
		if (file.is_open()) {
			cout << "打开成功\n";
		}
		else {
			cout << "打开失败\n";
		}
	}
	void run();
	void show_res();
};