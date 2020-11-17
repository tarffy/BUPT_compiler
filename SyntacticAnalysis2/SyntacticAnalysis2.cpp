#include "SyntacticAnalysis2.h"
#include <iomanip>
using std::setw;
#define EPSILON "_e"
#define DOLLAR "_$"
#define DOT -1
#define STACK_SIZE 100
#define WIDTH 6
void SyntacticAna2::out_dfa(vector<pair<vector<int>, set<int> > >& tem)
{
	int first_;
	for(auto &it2:tem){
		first_ = 1;
		for (auto it : it2.first) { 
			string str= ".";
			if (it != -1) {
				str = symbols_hash[it];
			}
			cout << str << ' '; 
			if (first_) {
				first_ = 0; cout << " -> ";
			}
		}
	cout << "\t[ ";
	first_ = 1;
	for (auto it : it2.second) { 
		if (first_) {
			first_--;
		}
		else {
			cout << " / ";
		}
		cout << symbols_hash[it]; }
	cout << "]\n";
	}
}
void SyntacticAna2::closure(vector<pair<vector<int>, set<int>>>& closure_)
{
	int count = 1;
	vector<pair<vector<int>, set<int>>> added;
	vector<int > status;	//-2 添加 0-n更新到
	while (count) {
		//out_dfa(closure_);
		//cout << '\n';
		count = 0;
		added.clear();
		status.clear();
		for (auto &it : closure_)
			count += (it.first.size() + it.second.size());
		for (auto it : closure_) {	//闭包中的每个式子
			int dot_;
			for (int i = 0; i < it.first.size(); i++)if (it.first[i] == -1) {
				dot_ = i; break;
			}
			if (dot_ == it.first.size() - 1)continue;
			set<int> after_dot2;
			if (dot_ + 2 == it.first.size())after_dot2.insert(__e);
			else if (termi.find(it.first[dot_ + 2]) != termi.end())after_dot2.insert(it.first[dot_ + 2]);
			else for (auto it : first[it.first[dot_ + 2]])after_dot2.insert(it);
			int nonterminal = it.first[dot_+1];
			if (nonter.find(nonterminal) != nonter.end()) {
				for (int j = 0; j < generate_num; j++) {
					if (generate[j].first == nonterminal) {
						pair < vector<int>, set<int> >tem2;
						generate_to_vector(tem2, j);
						for (auto &it2 : after_dot2) {
							if (it2 == __e) {
								for (auto it3 : it.second)
									tem2.second.insert(it3);
							}
							else {
								tem2.second.insert(it2);
							}
						}
						auto res = closure_check(tem2, closure_);
						if (res == -2) { added.emplace_back(tem2); status.push_back(-2); }
						else if (res != -1) {//合并两个pair的向前看字符串
							added.push_back(tem2);
							status.push_back(res);
						}
					}
				}
			}
		}
		//cout << "ADDing:\n";
		for (int i = 0; i < added.size(); i++) {
			int choice = status[i];
			if (choice == -2) { 
				auto res = closure_check(added[i], closure_);
				if(res==-2)
				closure_.emplace_back(added[i]); }
			else for (auto it : added[i].second)
				closure_[choice].second.insert(it);
	}
	for (auto &it : closure_)
		count -= (it.first.size() + it.second.size());
	}
}

int SyntacticAna2::closure_check(pair<vector<int>, set<int>>& tem, vector<pair<vector<int>, set<int>>>& closure_)
{
	/*cout << "check\n";
	for (auto it : tem.first)cout << it << " ";
	cout << '\n';
	for (auto it : tem.second)cout << it << " ";
	cout << "\n";*/
	int continue_flag ;
	for (int i = 0; i < closure_.size(); i++) {
		continue_flag = 0;
		if (closure_[i].first.size() != tem.first.size() )continue;
		for (int j = 0; j < closure_[i].first.size(); j++)
			if (tem.first[j] != closure_[i].first[j]) {
				continue_flag = 1;	//生成式不同
				break;
			}
		if (continue_flag)continue;
		for (auto &it:tem.second)
			if (closure_[i].second.find(it) == closure_[i].second.end()) {
				/*for (auto it : closure_[i].first)cout << it << " ";
				cout << '\n';
				for (auto it : closure_[i].second)cout << it << " ";
				cout << "\n";
				cout << i << '\n';*/
				return i;
			}
		
			/*for (auto it : closure_[i].first)cout << it << " ";
			cout << '\n';
			for (auto it : closure_[i].second)cout << it << " ";
			cout << "\n";
			cout << -1 << '\n';*/
			return -1;
		
	}
	//cout << -2<<"\n";
	return -2;
}
void SyntacticAna2::generate_to_vector(pair<vector<int>, set<int>>& tem, int i)
{
	tem.first.push_back(generate[i].first);
	tem.first.push_back(DOT);
	for (auto it : generate[i].second)
		tem.first.push_back(it);
}
int SyntacticAna2::check_vector_vector(vector<pair<vector<int>, set<int>>> &tem)
{
	for (int i = 0; i < dfa.size(); i++) {//比较每个项目生成式数量
		if (dfa[i].size() != tem.size())continue;
		int continue_flag = 0;
		for (int j = 0; j < tem.size(); j++) {//比较每个生成式情况
			auto cmp1 = tem[j];
			auto cmp2 = dfa[i][j];
			if (cmp1.first.size() != cmp2.first.size() || cmp1.second.size() != cmp2.second.size()) {
				continue_flag = 1; break;
			}
		}
		if (continue_flag)continue;
		continue_flag = 0;
		for (int j = 0; j < tem.size(); j++) {
			auto cmp1 = tem[j];
			auto cmp2 = dfa[i][j];
			for (int k = 0; k < cmp1.first.size(); k++) {
				if (cmp1.first[k] != cmp2.first[k]) { 
					continue_flag = 1; 
					break;
				}
			}
			for (auto it : cmp1.second) {
				if (cmp2.second.find(it) == cmp2.second.end()) {
					continue_flag = 1;
					break;
				}
			}
		}
		if (continue_flag)continue;
		return i;
	}
	return -1;
}
SyntacticAna2::SyntacticAna2(string & name)
{
	in.open(name, std::ios::in);
	if (!in.is_open()) {
		error("打开文件失败。");
		return;
	}
	get_generate();
	handle_generate_raw();
	first_and_follow_set();
	create_dfa();
	create_table();
}

void SyntacticAna2::show_res()
{
	cout << "Symbols:\n";
	for (int i = 0; i < symbols_hash.size(); i++) {
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
	cout << "Non\n";
	for (auto it : nonter)cout << it << ' ';
	cout << "\n";
	cout << "Termi\n";
	for (auto it : termi)cout << it << ' ';
	cout << '\n';
	cout << "dfa size:" << dfa.size();
	for (int i = 0; i < dfa.size(); i++) {
		cout << "第" << i << ":\n";
		out_dfa(dfa[i]);
	}
	cout << "Relations:\n";
	for (auto &it : relation) {
		cout << it.first.first << " " << symbols_hash[it.first.second] << " " << it.second << "\n";
	}
	string slash = string(WIDTH / 2, ' ') + '|' + string(WIDTH / 2, ' ');
	cout << "Table:\n";
	cout << "状态" << slash<< setw((termi.size()) * WIDTH) <<"Action"+ string(termi.size() / 2 * WIDTH, ' ')  <<slash
		<< setw((nonter.size()  -1) * WIDTH) << "goto"+string((nonter.size()-1) / 2 * WIDTH, ' ')<<"\n";
	cout << "    "+slash;
	for (auto it : termi)cout << setw(WIDTH) << symbols_hash[it];
	cout << slash;
	for (auto it : nonter) { 
		if (it == 0)continue;
		cout << setw(WIDTH) << symbols_hash[it]; }
	cout << "\n";
	for (int i = 0; i < dfa.size(); i++) {
		cout << setw(4) << i << slash;
		for (auto it : termi) {
			string tem="";
			if (table.find({ i,it }) != table.end()) {
				auto res = table[{i, it}];
				if (res.first == 1)tem +=( "S"+ std::to_string(res.second));
				else if (res.first == 2)tem += ("R"+ std::to_string(res.second));
				else if (res.first == 4)tem += "ACC";
			}
			cout << setw(WIDTH) << tem ;
		}
		cout << slash;
		for (auto it : nonter) {
			if (it == 0)continue;
			string tem = "";
			if (table.find({ i,it }) != table.end()) {
				auto res = table[{i, it}];
				tem += std::to_string(res.second);
			}
			cout << setw(WIDTH) << tem;
		}
		cout << "\n";
	}
}

void SyntacticAna2::show_errors()
{
	if (errors.size()) {
		cout << "Errors:\n";
		for (auto &it : errors)cout << it << "\n";
		if (table_conflict) {
			cout << "分析表存在冲突，如果要继续分析请按任意键。\n";
			system("PAUSE");
		}
	}
	else {
		cout << "No errors found.\n";
	}
}

void SyntacticAna2::solve()
{
	vector<pair<int, int> > stack(STACK_SIZE);
	int cur = -1;
	stack[++cur] = { 0,__$ };
	while (cur >= 0) {
		auto top = stack[cur--];

	}
}

void SyntacticAna2::check_token(string & token)
{
	if (symbols.find(token) == symbols.end()) {
		symbols[token] = symbol_num++;
	}
}

void SyntacticAna2::get_generate()
{
	in >> generate_num;
	generate_raw.resize(generate_num);
	generate.resize(generate_num+1);
	char buffer[200];
	in.getline(buffer, 200);
	for (int i = 0; i < generate_num; i++) {
		in.getline(buffer, 200);
		generate_raw[i] = buffer;
	}
}

void SyntacticAna2::create_dfa()
{
	vector<pair<vector<int>, set<int> > > first;
	pair<vector<int>, set<int>> tem;
	generate_to_vector(tem,0);
	tem.second.insert(__$);
	first.emplace_back(tem);
	
	//out_dfa(first);
	
	queue<vector<pair<vector<int>, set<int> > > > need_hanle;
	queue<pair<int,int>> parent;	//来自状态几 来的时候的符号
	int cur = -1;
	need_hanle.push(first);
	parent.push({-1,__e});
	while (!need_hanle.empty()) {
		auto handling = need_hanle.front();
		auto p = parent.front();
		need_hanle.pop();
		parent.pop();
		closure(handling);
		int res=check_vector_vector(handling);
		if (res==-1) {
			dfa.emplace_back(handling);
			relation[p] = dfa.size()-1;
			set<int> next_step;
			vector<int> next_step_in_order;
			for (auto &it : handling) {//寻找所有handling下一个接受的字符
				int dot_;
				for (int i = 0; i < it.first.size(); i++) {
					if (it.first[i] == -1) {
						dot_ = i;
						break;
					}
				}
				if (dot_ == it.first.size() - 1)continue;
				if (next_step.find(it.first[dot_ + 1]) == next_step.end()) {
					next_step_in_order.push_back(it.first[dot_ + 1]);
					next_step.insert(it.first[dot_ + 1]);
				}
				
			}
			for (auto it : next_step_in_order) {
				vector<pair<vector<int>, set<int> > > tem;
				for (int i = 0; i < handling.size(); i++) {
					auto tem_part = handling[i];
					int dot_;
					for (int i = 0; i < tem_part.first.size(); i++) {
						if (tem_part.first[i] == -1) {
							dot_ = i;
							break;
						}
					}
					if (dot_ == tem_part.first.size() - 1)continue;
					if (it != handling[i].first[dot_+1])continue;
					auto tem_ = tem_part.first[dot_];
					tem_part.first[dot_] = tem_part.first[dot_ + 1];
					tem_part.first[dot_ + 1] = tem_;
					tem.emplace_back(tem_part);
				}
				need_hanle.push(tem);
				parent.push({ dfa.size()-1,it });
			}
		}
		else {
			relation[p] = res;
		}
	}
}

void SyntacticAna2::create_table()
{
	for (int i = 0; i < dfa.size(); i++) {//每个项目簇
		auto part = dfa[i];
		for (int j = 0; j < part.size(); j++) {//判断每个生成式是否归约
			if (part[j].first[part[j].first.size() - 1] == -1) {//规约项目
				int continue_flag;
				for (int q = 0; q < generate_num; q++) {
					continue_flag = 0;
					if (generate[q].second.size() != part[j].first.size() - 2)continue;
					if (generate[q].first != part[j].first[0])continue;
					for (int k = 0; k < generate[q].second.size(); k++) {
						if (generate[q].second[k] != part[j].first[k + 1]) {
							continue_flag = 1;
							break;
						}
					}
					if (continue_flag)continue;
					else {
						for (auto it : part[j].second) {
							table[{i, it}] = { 2,q };
						}
					}
				}
			}
		}
		//判断goto
		for (auto it : nonter) {
			if (relation.find({i, it}) != relation.end()) {
				table[{i, it}] = { 3,relation[{i,it}] };
			}
		}
		//判断shift
		for (auto it : termi) {
			if (relation.find({ i, it }) != relation.end()) {
				table[{i, it}] = { 1,relation[{i,it}] };
			}
		}
	}
	table[{1, __$}] = { 4,0 };
}

void SyntacticAna2::handle_generate_raw()
{
	generate_num++;//拓广文法
	string token="S'";
	check_token(token);
	nonter.insert(symbols[token]);
	generate[0].first = 0;
	generate[0].second.push_back(1);
	for (int j = 0; j <generate_num-1; j++) {
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
		generate[j+1].first = symbols[token];//+1
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
				generate[j+1].second.push_back(symbols[token]);
				token.clear();
			}
			else {
				token += generate_raw[j][i];
			}
		}
		if (token.size()) {
			check_token(token);
			generate[j+1].second.push_back(symbols[token]);
		}
	}
	string tem = EPSILON;
	check_token(tem);
	symbols[DOLLAR] = symbol_num++;
	symbols_hash.resize(symbol_num);
	for (auto &it : symbols) {
		symbols_hash[it.second] = it.first;
	}
	for (int i = 0; i < symbol_num; i++) {
		if (nonter.find(i) == nonter.end())termi.insert(i);
	}
	termi.erase(symbols[EPSILON]);
	__e = symbols[EPSILON];
	__$ = symbols[DOLLAR];
}

void SyntacticAna2::first_and_follow_set()
{
	first.resize(symbols.size());
	follow.resize(symbols.size());
	vector<int> finished(symbols.size(), 0), has__e(symbols.size(), 0);
	for (auto it : termi)first[it].insert(it);
	int end = 0, count;
	while (!end) {
		end = 1;
		count = 0;
		for (auto &it : first)count += it.size();
		for (auto &it : generate) {
			auto generate_left = it.first;
			auto generate_right = it.second;
			int i;
			for (i = 0; i < generate_right.size(); i++) {
				if (nonter.find(generate_right[i]) == nonter.end()) {
					first[generate_left].insert(generate_right[i]);
					break;
				}
				else if (__e == generate_right[i]) {
					first[generate_left].insert(__e);
					break;
				}
				else {
					int no__e = 1;
					for (auto it2 : first[generate_right[i]]) {
						if (it2 == __e) {
							no__e = 0;
							continue;
						}
						first[generate_left].insert(it2);
					}
					if (no__e)break;
				}
			}
			if (i == generate_right.size()
				&& first[generate_right[i - 1]].find(__e) != first[generate_right[i - 1]].end()) {
				first[generate_left].insert(__e);
			}
		}
		for (auto &it : first)count -= it.size();
		if (count != 0)end = 0;
	}


	end = 0;
	follow[0].insert(symbols[DOLLAR]);
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
								if (it2 == __e) {
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
