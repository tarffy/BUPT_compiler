#include "LexicalAnalysis.h"

void LexicalAna::_getchar()
{
	if (file.eof()) {
		end = 1;
		return;
	}
	C = file.get();
}



void LexicalAna::run()
{
	while (!end) {
		switch (state)
		{
		case 0: {	//初始状态
			token.clear();
			_getchar();
			switch (C)
			{
			case '<':state = 8; break;
			case '>':state = 9; break;
			case '=':state = 10; break;
			case '/':state = 11; break;
			case '!':state = 12; break;
			case '&':state = 13; break;
			case '|':state = 14; break;
			case '\'':
			case '\"':str_token = C; state = 15; break;
			case '-':state=16; break;
			case '+':state = 17; break;
			case '*':state = 18; break;
			case '^':table.emplace_back("relop", "XOR"); break;
			case '#':state = 20; break;
			
			case '(':table.emplace_back("(", ""); break;
			case ')':table.emplace_back(")", ""); break;
			case '[':table.emplace_back("[", ""); break;
			case ']':table.emplace_back("]", ""); break;
			case '{':table.emplace_back("{", ""); break;
			case '}':table.emplace_back("}", ""); break;
			case ';':table.emplace_back(";", ""); break;
			case ':':table.emplace_back(":", ""); break;	//:=赋值要改
			case '?':table.emplace_back("?", ""); break;
			case '\\':table.emplace_back("\\", ""); break;
			case '%':table.emplace_back("%", ""); break;
			case ',':table.emplace_back(",", ""); break;
			case '.':table.emplace_back(".", ""); break;
			case '\t':
			case '\n':
			case ' ':break;
			default: {
				if (is_letter()) {
					state = 1; break;
				}
				if (is_digit()) {
					state = 2; break;
				}
				error(string("state 0 invalid letter ")+C+" "+std::to_string(int(C)));
				break;
			}
			}
			break;
		}
		case 1: {	//标识符
			cat();
			_getchar();
			if (is_letter() || is_digit()) {
				//state = 1;
			}
			else {
				retract();
				state = 0;
				auto iskey = keys.find(token);
				if (iskey != keys.end()) {
					table.emplace_back(token, "");

				}
				else {
					auto res = symbol_table.find(token);
					if (res == symbol_table.end()) {
						symbol_table.insert(token);
					}
					table.emplace_back("id", token);
				}
			}
			break;
		}
		case 2: {//常数
			cat();
			_getchar();
			switch (C)
			{
			case '.':state = 3; break;
			case 'E':state = 5; break;
			default: {
				if (is_digit()) {
					state = 2; break;
				}
				retract();
				state = 0;
				table.emplace_back("num", 'i' + token);
				break;
			}
			}
			break;
		}
		case 3: {	//小数点
			cat();
			_getchar();
			if (is_digit())state = 4;
			else {
				error("wrong letter after .");
				state = 0;
			}

			break;
		}
		case 4: {	//小数
			cat();
			_getchar();
			switch (C)
			{
			case 'E':state = 5; break;
			default:
				if (is_digit()) {
					//state = 4;
					break;
				}
				retract();
				state = 0;
				table.emplace_back("num", 'd' + token);
				break;
			}

			break;
		}
		case 5: {	//指数
			cat();
			_getchar();
			switch (C)
			{
			case '+':
			case '-':state = 6; break;
			default: {
				if (is_digit()) {
					state = 7;
					break;
				}
				retract();
				error("Wrong letter after E");
				state = 0;
				break;
			}

			}
			break;
		}
		case 6: {	//after E+ or E-
			cat();
			_getchar();
			if (is_digit())state = 7;
			else {
				retract();
				error("No digit afetr E+ or E-");
				state = 0;
				break;
			}
			break;
		}
		case 7: {
			cat();
			_getchar();
			if (!is_digit()) {
				retract();
				state = 0;
				table.emplace_back("num", 's' + token);
			}
			break;
		}
		case 8: {	// <
			cat();
			_getchar();
			if (C == '=') {
				table.emplace_back("relop", "LE"); 
			}
			else {
				retract();
				table.emplace_back("relop", "LT"); 
			}
			state = 0;
			break;
		}
		case 9: {	//	>
			cat();
			_getchar();
			if(C=='=')table.emplace_back("relop", "GE");
			else {
				retract();
				table.emplace_back("relop", "GT");
			}
			state = 0;
			break;
		}
		case 10: {	//	:=赋值	要改	改后= ==
			cat();
			_getchar();
			if (C == '=') {
				table.emplace_back("relop", "EQ");
			}
			else {
				retract();
				table.emplace_back("assign-op", "");
			}
			state = 0;
			break;
		}
		case 11: {	//	/
			cat();
			_getchar();
			switch (C)
			{
			case '/':note_line = 1; state = 19; break;
			case '*':note_lines = 1; state = 19; break;
			case '=':table.emplace_back("/=", ""); break;
			default:retract(); state = 0; table.emplace_back("/", ""); break;
			}
			
			break;
		}
		case 12: {	//!
			cat();
			_getchar();
			if (C == '=') {
				table.emplace_back("relop", "NE"); 
			}
			else {
				retract();
				table.emplace_back("!", "");
			}
			state = 0;
			break;
		}
		case 13:{	//&
			cat();
			_getchar();
			if (C == '&') {
				table.emplace_back("relop", "AND");
			}
			else {
				retract();
				table.emplace_back("&", "");
			}
			state = 0;
			break;
		}
		case 14: {	//|
			cat();
			_getchar();
			if (C == '|') {
				table.emplace_back("relop", "OR"); 
			}
			else {
				retract();
				table.emplace_back("|", "");
			}
			state = 0;
			break;
		}
		case 15: {	//字符串识别
			cat();
			_getchar();
			if (C == str_token) {
				table.emplace_back("str",token.substr(1));
				state = 0;
			}
			break;
		}
		case 16: {	//-
			cat();
			_getchar();
			switch (C)
			{
			case '>':table.emplace_back("->", ""); break;
			case '-':table.emplace_back("--", ""); break;
			case '=':table.emplace_back("-=", ""); break;
			default:retract(); table.emplace_back("-", ""); break;
			}
			state = 0;
			break;
		}
		case 17: {	//+
			cat();
			_getchar();
			switch (C)
			{
			case '+':table.emplace_back("++", ""); break;
			case '=':table.emplace_back("+=", ""); break;
			default:retract(); table.emplace_back("+", ""); break;
			}
			state = 0;
			break;
		}
		case 18: {	//*
			cat();
			_getchar();
			switch (C)
			{
			case '=':table.emplace_back("*=", ""); break;
			default:retract(); table.emplace_back("*", ""); break;
			}
			state = 0;

			break;
		}
		case 19: {	//处理注释
			_getchar();
			if (note_line&&C == '\n') {
				note_line = 0;
				state = 0;
				break;
			}
			if (note_lines&&C == '*') {
				_getchar();
				if (C == '/') {
					note_lines = 0;
					state = 0;
					break;
				}
				else {
					retract();
				}
			}
			break;
		}
		case 20: {	// #
			_getchar();
			switch (C)
			{
			case 'i': {
				while (C != '<')_getchar();
				token.clear();
				_getchar();
				while (C != '>') {
					cat();
					_getchar();
				}
				table.emplace_back("#include", token);
				while(C!='\n')_getchar();
				break;
			}
			case 'd': {
				while (C != ' ')_getchar();
				while (C == ' ')_getchar();
				token.clear();
				while (C != '\n') {
					cat();
					_getchar();
				}
				table.emplace_back("#define", token);
				break;
			}
			case 't': {
				while (C != ' ')_getchar();
				while (C == ' ')_getchar();
				token.clear();
				while (C != '\n') {
					cat();
					_getchar();
				}
				table.emplace_back("#typedef", token);
				break;
			}
			}
			state = 0;
			break;
		}
		}
	}
}

void LexicalAna::show_res()
{
	cout << "table:\n";
	for (auto &it : table) {
		cout << it.first << " " << it.second << '\n';
	}
	cout << "symbols:\n";
	for (auto &it : symbol_table) {
		cout << it << '\n';
	}
	cout << "errors:\n";
	for (auto &it : errors) {
		cout << it << '\n';
	}
}
