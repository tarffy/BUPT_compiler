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
	string token;
	int state=0;
	int end = 0;
	int note_line = 0, note_lines = 0;
	char str_token;//字符串的标记 单引号还是双引号
	vector<pair<string, string>> table;
	unordered_set<string> symbol_table;
	vector<string> errors;
	set<string> keys = { "auto","break","case","char","const","continue","default",
		"do","double","else","enum","extern","float","for","goto","if","int","long",
		"register","return","short","signed","sizeof","static","struct","switch",
		"typedef","union","unsigned","void","volatile","while"};
	void _getchar();
	void retract() {
		file.seekg(-1, ios::cur);
	}
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
	void note_handle();
public:
	LexicalAna(string file_name) {
		file.open(file_name, ios::in);
		if (file.is_open()) {
			cout << "打开成功";
		}
		else {
			cout << "打开失败";
		}
	}
	void run();
	void show_res();
};