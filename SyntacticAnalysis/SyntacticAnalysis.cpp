#include "SyntacticAnalysis.h"
#include <iomanip>
#include <ios>
using std::setw;
using std::setfill;
using std::right;
using std::setiosflags;

SyntacticAna::SyntacticAna(string & name)
{
	in.open(name);
	if (!in.is_open()) {
		cout << "Open file filed\n";
		return;
	}
	get_generate();
	handle_generate_raw();
	first_and_follow_set();
	create_table();

	char buffer[200];
	in.getline(buffer, 200);
	input = buffer;

}

void SyntacticAna::check_token(string & token)
{
	if (symbols.find(token) == symbols.end()) {
		symbols[token] = symbol_num++;
	}
}

void SyntacticAna::get_generate()
{
	in >> generate_num;
	generate_raw.resize(generate_num);
	generate.resize(generate_num);
	char buffer[200];
	in.getline(buffer, 200);
	for (int i = 0; i < generate_num; i++) {
		in.getline(buffer, 200);
		generate_raw[i] = buffer;
	}
}

void SyntacticAna::handle_generate_raw()
{
	string token;
	for (int j = 0; j < generate_num; j++) {
		token.clear();
		auto res = generate_raw[j].find("->");
		if (res == -1) {
			error("No -> in generate " + generate_raw[j]);
			continue;
		}
		int i;
		for (i = 0; i < res; i++) {
			if (generate_raw[j][i] != ' ')token += generate_raw[j][i];
			else break;
		}
		check_token(token);
		nonter.insert(symbols[token]);
		generate[j].first = symbols[token];
		token.clear();
		int flag = 0;
		while (i < res) {
			if (generate_raw[j][i] != ' ') {
				error("Too many symbols at left of generate " + generate_raw[j]);
				flag = 1;
				break;
			}
			i++;
		}
		if (flag)continue;
		for (i = res + 2; i < generate_raw[j].size(); i++) {
			if (generate_raw[j][i] == ' ') {
				if (token.size() == 0)continue;
				check_token(token);
				generate[j].second.push_back(symbols[token]);
				token.clear();
			}
			else {
				token += generate_raw[j][i];
			}
		}
		if (token.size()) {
			check_token(token);
			generate[j].second.push_back(symbols[token]);
		}
	}
	string tem="__e";
	check_token(tem);
	symbols_hash.resize(symbol_num);
	for (auto &it : symbols) {
		symbols_hash[it.second] = it.first;
	}
	handle_recursion();
	reconstruct_generate_raw();
}

void SyntacticAna::get_next_token(string & s, int &cur)
{
	s.clear();
	while (cur < input.size()) {
		if (input[cur] == ' ' ) {
			cur++;
			if (s.size()) {
				return;
			}
		}
		else {
			s+= input[cur++];
		}
	}
}



void SyntacticAna::first_and_follow_set()
{
	first.resize(symbols.size());
	follow.resize(symbols.size());
	vector<int> finished(symbols.size(), 0), has__e(symbols.size(), 0);
	for (int i = 0; i < symbols.size(); i++) {
		if (nonter.find(i) == nonter.end()) {
			first[i].insert(i);
			finished[i] = 1;
			if (symbols_hash[i] == "__e")has__e[i] = 1;
		}
	}
	for (int i = 0; i < symbols.size(); i++) {
		if (finished[i])continue;
		dfs_first(i, finished, has__e);
	}
	int end = 0;
	follow[0].insert(symbols["__$"]);
	while (!end) {
		end = 1;
		for (auto &it : generate) {
			auto generate_left = it.first;
			auto generate_right = it.second;
			for (int i = 0; i < generate_right.size(); i++) {
				if (nonter.find(generate_right[i]) == nonter.end()) {
					continue;
				}
				else {
					auto symbol = generate_right[i];	//正在处理的非终结符
					int num = follow[symbol].size();
					int j;
					for (j = i + 1; j < generate_right.size(); j++) {
						int break_flag = 1;
						if (nonter.find(generate_right[j]) == nonter.end()) {
							follow[symbol].insert(generate_right[j]);	//insert
							break;
						}
						else {
							for (auto it2 : first[generate_right[j]]) {
								if (symbols_hash[it2] == "__e") {
									break_flag = 0;
								}
								else {
									follow[symbol].insert(it2);	////insert
								}
							}
						}
						if (break_flag)break;
					}
					if (j == generate_right.size()) {
						for (auto it3 : follow[generate_left]) {
							follow[symbol].insert(it3);	////insert
						}
					}
					if (follow[symbol].size() != num)end = 0;
				}
			}
		}
	}


}

void SyntacticAna::dfs_first(int symbol, vector<int>& finished, vector<int>& has__e)
{
	for (int i = 0; i < generate.size(); i++) {
		if (generate[i].first != symbol)continue;
		auto generate_right = generate[i].second;
		int j;
		for (j = 0; j < generate_right.size();j++) {
			auto token = generate_right[j];	//生成式右边 正在处理的符号
			if (nonter.find(token) == nonter.end()) {//遇到终结符直接停止
				first[symbol].insert(token);
				if (symbols_hash[token] == "__e")has__e[token] = 1;
				break;
			}
			else {
				if (!finished[token])dfs_first(token, finished, has__e);
				for (auto it : first[token]) {
					first[symbol].insert(it);
				}
				if (!has__e[token])break;	//非终结符不致空 直接结束
			}
		}
		if (j == generate_right.size())has__e[symbol] = 1;
	}
	finished[symbol] = 1;
	
}

void SyntacticAna::reconstruct_generate_raw()
{
	generate_raw.clear();
	generate_raw.resize(generate_num);
	for (int i = 0; i < generate_num; i++) {
		string tem;
		tem = symbols_hash[generate[i].first]+" ->";
		for (auto it : generate[i].second)tem += (" "+symbols_hash[it] );
		generate_raw[i] = tem;
	}
}

void SyntacticAna::handle_recursion()
{
	int added_generate = 0;
	for (int i = 0; i < symbol_num; i++) {
		if (nonter.find(i) == nonter.end())continue;
		int exist_left = 0;
		vector<int> left, no_left;
		for (int j = 0; j < generate_num; j++) {
			if (generate[j].first == i) {
				if (generate[j].second[0] == i) {
					exist_left = 1;
					left.push_back(j);
				}
				else {
					no_left.push_back(j);
				}
			}
		}
		if (exist_left) {
			string symbol_ = symbols_hash[i] + '\'';

			check_token(symbol_);
			nonter.insert(symbols[symbol_]);
			for (auto it : no_left) {
				generate[it].second.push_back(symbols[symbol_]);
			}
			for (auto it : left) {
				generate[it].first = symbols[symbol_];
				int size = generate[it].second.size();
				for (int j = 0; j < size - 1; j++) {
					generate[it].second[j] = generate[it].second[j + 1];
				}
				generate[it].second[size - 1] = symbols[symbol_];

			}
			pair<int, vector<int>> res;
			res.first = symbols[symbol_];
			res.second.push_back(symbols["__e"]);
			generate.emplace_back(res);
			added_generate++;
		}
	}
	symbols["__$"] = symbol_num++;
	symbols_hash.clear();
	symbols_hash.resize(symbol_num);
	for (auto &it : symbols) {
		symbols_hash[it.second] = it.first;
	}
	if (added_generate) {
		cout << "消除左递归完成";
		generate_num += added_generate;
	}
}

void SyntacticAna::create_table()
{
	for (int i = 0; i < generate_num; i++) {
		set<int> first_;
		auto generate_left = generate[i].first;
		auto generate_right = generate[i].second;
		int j;
		for (j = 0; j < generate_right.size(); j++) {
			int break_flag = 1;
			if (nonter.find(generate_right[j]) == nonter.end()) {
				if (generate_right[j] == symbols["__e"]) {
					j++; break;
				}else
				first_.insert(generate_right[j]);
			}
			else {
				for (auto it2 : first[j]) {
					if (it2 == symbols["__e"]) {
						break_flag = 0;
					}
					else {
						first_.insert(it2);
					}
				}
			}
			if (break_flag)break;
		}
		for (auto it2 : first_) {
			table[{generate_left, it2}] = i;
		}
		if (j == generate_right.size()) {
			for (auto it2 : follow[generate_left]) {
				table[{generate_left, it2}] = i;
			}
		}
	}
}



void SyntacticAna::show_res()
{
	if (errors.size()) {
		cout << "Errors:\n";
		for (auto &it : errors)cout << it << "\n";
	}
	else {
		cout << "No errors found.\n";
	}
	cout << "Symbols:\n";
	for (int i = 0; i < symbols_hash.size();i++) {
		cout << i << " " << symbols_hash[i] << "\n";
	}
	cout << "First:\n";
	for (int i = 0; i < symbol_num; i++) {
		if (nonter.find(i) == nonter.end())continue;
		cout << symbols_hash[i] << ": ";
		for (auto &it : first[i])cout << symbols_hash[it] << " ";
		cout << "\n";
	}
	cout << "Follow:\n";
	for (int i = 0; i < symbol_num; i++) {
		if (nonter.find(i) == nonter.end())continue;
		cout << symbols_hash[i] << ": ";
		for (auto &it : follow[i])cout << symbols_hash[it] << " ";
		cout << "\n";
	}
	cout << "Generate:\n";
	for (auto &it : generate) {
		cout << it.first << " -> ";
		for (auto &it2 : it.second) {
			cout << it2 << " ";
		}
		cout << "\n";
	}
	cout << "Reconstruct generate:\n";
	for (auto &it : generate_raw) {
		cout << it << '\n';
	}
	cout << "Table:\n";
	for (auto &it : table) {
		cout << it.first.first << " " << it.first.second << " " << it.second << "\n";
	}
}

void SyntacticAna::solve()
{
	const int output_stack = 30,output_input = 30,output_output = 20;
	stack_.resize(100);
	stack_cur = 0;
	stack_[++stack_cur] = symbols["__$"];
	stack_[++stack_cur] = 0;
	int cur = 0,temcur=0;
	string token;
	get_next_token(token,cur);
	cout <<"STACK"<< setw(output_stack-5)<<"";
	cout << setw(output_input) << "INPUT";
	cout << setw(output_output) << "OUTPUT" << "\n";
	while (stack_cur>=0) {
		int count = 0;
		for (int i = 1; i <= stack_cur;i++) {
			auto s = symbols_hash[stack_[i]];
			count +=(s.size() + 1);
			cout <<  s<< ' ';
		}
		cout << setw(output_stack - count) << setfill(' ') << "";
		cout << setw(output_input)<<input.substr(temcur);
		cout << setw(output_output);
		auto top = stack_[stack_cur];
		if (top == symbols[token] && token=="__$") {
			cout << "SUCCESS\n";
			return;
		}
		else if (top==symbols[token]) {
			cout << "识别\n";
			stack_cur--;
			temcur = cur;
			get_next_token(token, cur);
			
		}else
		{
			stack_cur--;
			auto table_c = table[{top, symbols[token]}];
			cout << "使用" << generate_raw[table_c]<<"\n";
			for (int i = generate[table_c].second.size()-1; i >= 0; i--) {
				auto num = generate[table_c].second[i];
				if (symbols_hash[num] == "__e")continue;
				stack_[++stack_cur] = num;
			}
		}

	}

}
