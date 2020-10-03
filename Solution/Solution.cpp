// Solution.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include "LexicalAnalysis.h"
int main()
{
	LexicalAna LA("queue.c");
	LA.run();
	LA.show_res();
	return 0;
}
