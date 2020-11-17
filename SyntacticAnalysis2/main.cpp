// SyntacticAnalysis2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "SyntacticAnalysis2.h"
int main(int argc, char* argv[])
{
	string file_name="G3.in";
	if (argc == 1) {
		//cout << "请输入要分析文件的绝对路径或相对路径:\n";
		//std::cin >> file_name;
	}
	else if (argc == 2) {
		file_name = argv[1];
	}
	else {
		cout << "参数应为要分析文件的绝对路径或相对路径:\n";
		return 0;
	}
	SyntacticAna2 SA(file_name);
	SA.show_errors();
	SA.show_res();
	//SA.solve();
	system("PAUSE");
	return 0;
}

