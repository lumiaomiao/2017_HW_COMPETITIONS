#include "deploy.h"
//#include "MinimumCostMaximumFlow.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include "GA.h"
#include <time.h>
//#include "couplemcmf.h"
using namespace std;
bool result[1000];

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	//time_t start,stop;
	//start = time(NULL);
	char * topo_file;
	string out;
	time_t start,end;
	int p;
	GA(topo, line_num);
	MinCostMaxFlow_c mincostmaxflow_c(result, node_num);
	p = mincostmaxflow_c.couple(topo, line_num, result);
	mincostmaxflow_c.get_path(out);
	int len = out.length();
	topo_file = new char[len + 1];
	strcpy(topo_file,out.c_str());
	
	write_result(topo_file, filename);
	// 需要输出的内容 char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23"; 
	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)

}
