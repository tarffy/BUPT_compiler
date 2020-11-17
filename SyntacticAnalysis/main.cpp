
#include <iostream>
#include "SyntacticAnalysis.h"
int main(int argc,char* argv[])
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
	SyntacticAna SA(file_name);
	SA.show_errors();
	//SA.show_res();
	SA.solve();
	system("PAUSE");
	return 0;
}

