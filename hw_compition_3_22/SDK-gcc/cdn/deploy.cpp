#include "deploy.h"
//#include "MinimumCostMaximumFlow.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <time.h>
#include "GA.h"
using namespace std;
bool a[1000];

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	//time_t start,stop;
	//start = time(NULL);
	char * topo_file;
	string out;
	GA(topo, line_num, a);
	//a[7] = a[13]= a[15] = a[22] = a[37] = a[38] = a[43] = 1;
	//b[6]= b[7] = b[13] = b[17] = b[35] = b[41] = b[48] = 1;
	/*for(int q = 0; q < node_num; q++){
		if(a[q]) cout <<q <<" ";
	}
	cout <<endl;*/
	MinimumCostMaximumFlow mincostmaxflow(a, sizeof(a));
	mincostmaxflow.get_cost(topo, line_num);
	mincostmaxflow.get_allpath(out);

	int len = out.length();
	topo_file = new char[len+1];
	strcpy(topo_file,out.c_str());
	
	write_result(topo_file, filename);
	//stop = time(NULL);
	//printf("Use Time:%ld\n",(stop-start));
	//cout<<"Use time:"<<stop - start<<endl;
	// 需要输出的内容 char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23"; 
	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)

}
