#include "LexicalAnalysis.h"
using std::to_string;
void LexicalAna::_getchar()
{
	if (buff_status == -1) {//起始时填充第一块缓冲区
		file.get(buff1, BUFFER_SIZE+1, EOF);//读取直到EOF 最多BUFFER_SIZE个字符会将buff1[BUFFER_SIZE]处置'\0'
		buff_status = 0;
		C = buff1[0];
		cur = 1;
	}
	else if (buff_status == 0) {
		if (buff1[cur] == '\0') {
			if (file.eof()) {
				end = 1;
				return;
			}
			file.get(buff2, BUFFER_SIZE + 1, EOF);
			buff_status = 1;
			C = buff2[0];
			cur = 1;
		}
		else {
			C = buff1[cur++];
		}
	}
	else {	//buff_status==1
		if (buff2[cur] == '\0') {
			if (file.eof()) {
				end = 1;
				return;
			}
			file.get(buff1, BUFFER_SIZE + 1, EOF);
			buff_status = 0;
			C = buff1[0];
			cur = 1;
		}
		else {
			C = buff2[cur++];
		}
	}
	
	if (C == '\n') { lines++; tem_letter_new_line = letter_new_line; letter_new_line = 0; }
	else if(C=='\t')letter_new_line+=4;
	else letter_new_line++;
	letters++;
}

void LexicalAna::retract()
{
	if (C == '\n') { lines--; letter_new_line = tem_letter_new_line; }
	else letter_new_line--;
	letters--;
	cur--;
}

void LexicalAna::error( string str)
{
	string tem = "Line " + to_string(lines+1) + ",pos " + to_string(letter_new_line) + ":" + str;
	errors.emplace_back(tem);
}



void LexicalAna::run()
{
	if (!file.is_open()) {
		return;
	}
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
			case '%':state = 21; break;
			case '~':state=22; break;

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
				error(string("Invalid letter ")+C+" ascii("+to_string(int(C)) + ") found.");
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
					counts[2]++;
				}
				else {
					counts[0]++;
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
				counts[1]++;
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
				error("Invalid character after \'.\'.");
				state = 0;
			}

			break;
		}
		case 4: {	//小数
			cat();
			_getchar();
			switch (C)
			{
			case 'e':
			case 'E':state = 5; break;
			default:
				if (is_digit()) {
					//state = 4;
					break;
				}
				retract();
				state = 0;
				table.emplace_back("num", 'd' + token);
				counts[1]++;
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
				error("Invalid character after E.");
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
				error("No digit afetr E+ or E-.");
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
				counts[1]++;
			}
			break;
		}
		case 8: {	// <
			cat();
			_getchar();
			if (C == '=') {
				table.emplace_back("<=", ""); 
			}
			else if (C == '<') {
				cat();
				_getchar();
				if(C=='=')table.emplace_back("<<=", "");
				else { table.emplace_back("<<", ""); retract();
				}
			}else {
				retract();
				table.emplace_back("<", ""); 
			}
			state = 0;
			break;
		}
		case 9: {	//	>
			cat();
			_getchar();
			if(C=='=')table.emplace_back(">=", "");
			else if (C == '>') {
				cat();
				_getchar();
				if (C == '=')table.emplace_back(">>=", "");
				else {
					table.emplace_back(">>", ""); retract();
				}
			}else {
				retract();
				table.emplace_back(">", "");
			}
			state = 0;
			break;
		}
		case 10: {	//	=
			cat();
			_getchar();
			if (C == '=') {
				table.emplace_back("==", "");
			}
			else {
				retract();
				table.emplace_back("=", "");
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
			else if (C == '=') {
				table.emplace_back("&=", "");
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
			else if (C == '=') {
				table.emplace_back("|=", "");
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
				counts[3]++;
				state = 0;
			}
			else if (C == '\n') {
				retract();
				error(string("No ") + str_token + " at the end of the line.");
				_getchar();
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
		case 21: {//%
			cat();
			_getchar();
			switch (C)
			{
			case '=':table.emplace_back("%=", ""); break;
			default:retract(); table.emplace_back("%", ""); break;
			}
			state = 0;
			break;
		}
		case 22: {	// ^
			cat();
			_getchar();
			if (C == '=') {
				table.emplace_back("^=", "");
			}
			else {
				retract();
				table.emplace_back("&", "");
			}
			state = 0;
			break;
		}
		}
	}
}

void LexicalAna::show_res()
{
	cout << "Total lines:" << lines<<'\n';
	cout << "Total characters:" << letters << '\n';
	cout << "Total id: " << counts[0] << "   Total num: " << counts[1]
		<< "   Total reserved word: " << counts[2] <<"   Total string: "<<counts[3]<<"\n";
	cout << "\ntable:\n";
	for (auto &it : table) {
		cout << '<' <<it.first << " , " << it.second << ">\n";
	}
	cout << "\nsymbols:\n";
	for (auto &it : symbol_table) {
		cout << it << '\n';
	}
	if (errors.size()) {
		cout << "\nerrors:\n";
		for (auto &it : errors) {
			cout << it << '\n';
		}
	}

}


