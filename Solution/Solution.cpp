// Solution.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include "LexicalAnalysis.h"
int main(int argc,char *argv[])
{
	string file_name;
	if (argc == 1) {
		cout << "请输入要分析文件的绝对路径或相对路径:\n";
		std::cin >> file_name;
	}
	else if (argc == 2) {
		file_name = argv[1];
	}
	else {
		cout << "参数应为要分析文件的绝对路径或相对路径:\n";
		return 0;
	}	

	LexicalAna LA(file_name);
	if (!LA.open_success()) {
		cout << "文件打开失败，请检查文件路径！\n";
		return 0;
	}

	LA.run();
	LA.show_res();
	return 0;
}
