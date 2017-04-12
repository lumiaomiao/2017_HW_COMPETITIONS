#ifndef GA_h
#define GA_h
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
//#include "lib_io.h"
#include "MinimumCostMaximumFlow.h"
#include "couplemcmf.h"
#include "Ini_node.h"
using namespace std;


const int n = 1000;  //                                             n是网络节点的个数    20需要改正
const int M = 16;  //                                              暂定种群规模为M

int linenum;
int node_num, edge_num, cust_num;
bool bit[M][n];
int test_node = 0;
int save[n];
int ssrach;

typedef struct Chrom                           // 结构体类型，为单个染色体的结构;
{
	int row;//
	int size;             //服务器节点的实际个数
	int fit;//适应值
	double rfit;//相对的fit值，即所占的百分比
	double cfit;//积累概率
}chrom;

//定义将会用到的几个函数； 
void GA(char * topo_s[MAX_EDGE_NUM],int line_num);                                                           
void *evpop(chrom popcurrent[M],char * topo[MAX_EDGE_NUM]);//进行种群的初始化             根据网络节点 成为服务器节点的可能性 进行初始化                                     
int fitness(chrom &chom,char * topo[MAX_EDGE_NUM]);
void *pickchroms(chrom popcurrent[M], int new_m);//选择操作
void *crossover(chrom popcurrent[M],char * topo[MAX_EDGE_NUM], int new_m);//交叉操作
void *mutation(chrom popcurrent[M],char * topo[MAX_EDGE_NUM], int new_m, int times, int count_time,int time0,int bound);//突变
chrom popcurrent[M];   // 初始种群规模为M；
chrom popnext[M];  //变化时的种群


void GA(char * topo[MAX_EDGE_NUM],int line_num)                                    // 主函数；
{
	int num = INT_MAX;                                    // 迭代次数；                暂定迭代次数是100
	int i, Min, k;
	int fin_min = INT_MAX, fin_tim = 15;
	time_t start,end;
	extern bool result[1000];
	linenum = line_num;
	stringstream ss;
	srand(clock());
	ss << topo[0];
	ss >> node_num >> edge_num >> cust_num;
	start = time(NULL);
	evpop(popcurrent, topo);    // 产生初始种群；
	Min = popcurrent[0].fit;//对Min值进行初始化
	int m ;
	int times;
	int time0;
	int bound;
	if(node_num < 300 ) {
		m = 16;
		times=16; 
		ssrach = 1000;
		time0=45;
		bound=60;
		}
	else if(node_num < 600) {
		m = 16;
		times = 16;
		ssrach = 800;
		time0=25;
		bound=60;
		}
	else {m = 16;
		times=20;
		ssrach = 500;
		time0=25;
		bound=60;
		}
	for (i = 0; i< num; i++)                          // 开始迭代；
	{
		end = time(NULL);
		int count_time = end - start;
		if(count_time > 87) {
			break;
		}
		pickchroms(popcurrent, m);                    // 挑选优秀个体；
		crossover(popcurrent,topo, m);                     // 交叉得到新个体；
		mutation(popcurrent,topo, m, times, count_time,time0,bound);                      // 变异得到新个体；
		for (int i = 1; i < m; i++)
		{
			fitness(popcurrent[i],topo);        // 为新个体计算适应度值；
		}
		for(int i = 0; i < node_num; i++){
			if(bit[popcurrent[0].row][i]){
				save[i]++;
			}
		}
		// 提交时候需要注释
		cout << count_time << ":";
		for(int i = 0 ; i < m; i++){
			cout << popcurrent[i].fit << " ";
		}
		cout << endl; 

	}  
	cout << "迭代次数:" << i << endl;
	for (i = 0; i< m; i++)
	{
		if (popcurrent[i].fit < Min)
		{
			Min = popcurrent[i].fit;
			k = i;
		}
	}
	
	cout <<"price:" <<Min<<endl;
	for(i = 0; i < node_num; i++){	
		result[i] = bit[popcurrent[k].row][i];
		//if(result[i]) cout << i <<":"<<result[i] << ","<< save[i]<<"   " << endl;
	}
	//cout <<"The location is"<< popcurrent[k].bit << endl;
	//system("pause");
}

int fitness(chrom &chom,char * topo[MAX_EDGE_NUM])
{  
	//使用最小费用流计算消费值作为适应值,把值赋给 .fit
	MinCostMaxFlow_c mincostmaxflow_c(bit[chom.row], node_num);
	chom.fit = mincostmaxflow_c.couple(topo, linenum, bit[chom.row]);
	return chom.fit;
}

void *evpop(chrom popcurrent[M],char * topo[MAX_EDGE_NUM])   //                                需要初始化M个极有可能可行的解
{
	Ini_node ini_node;
	ini_node.prepare(topo, linenum);	

	for(int i = 0; i < M; i++){
		//int random;
		popcurrent[i].row = i;
		ini_node.get_ini_node(bit[i], i);
		if(fitness(popcurrent[i],topo) == INT_MAX){
			i--;
		}
	}
	return (0);
}


void *pickchroms(chrom popcurrent[M], int new_m)          // 函数：选择个体；
{
	int i, j;
	int temp;                                // 中间变量
	//cout<<"pickchroms"<<endl;
	int row = popcurrent[i].row;
	for (i = 0; i < new_m; i++)                           // 根据个体适应度来排序；（冒泡法）
	{
		for (j = i+1; j< new_m; j++)
		{
			if (popcurrent[j].fit < popcurrent[i].fit)
			{
				temp = popcurrent[j].row;
				popcurrent[j].row = popcurrent[i].row;
				popcurrent[i].row = temp;
				temp = popcurrent[j].fit;
				popcurrent[j].fit = popcurrent[i].fit;
				popcurrent[i].fit = temp;

			}
		}
	}
	/*if(popcurrent[0].row == row){
		int random = rand()% 1000;
		if(random < 1){
			int temp = popcurrent[0].row;
			popcurrent[0].row = popcurrent[1].row;
			popcurrent[1].row = temp;
			temp = popcurrent[0].fit;
			popcurrent[0].fit = popcurrent[1].fit;
			popcurrent[1].fit = temp;
		}
	}*/
	return(0);
}

 void *crossover(chrom popcurrent[M],char * topo[MAX_EDGE_NUM], int new_m)              // 函数：交叉操作；
{
	int random;
	int i;
	random = rand();                             // 随机产生交叉点；
	random = random % node_num ;
	for (int j = 0; j < new_m / 2; j++)                    //只保留适应度比较大的前M/2个，剩余M/2个变为交叉之后的结果
	{
		// 交叉点控制在0到n-1之间；随机产生0到n-1的整数
		for (i = 0; i < random; i++)
		{
			bit[popcurrent[new_m / 2 + j].row][i] = bit[popcurrent[j].row][i];
		}
		//cout << "1 ";
		for (i = random; i < node_num; i++)                      // crossing the bits beyond the cross point index
		{
			bit[popcurrent[new_m / 2 + j].row][i] = bit[popcurrent[new_m / 2 - 1 - j].row][i];
		}
	}
	return(0);
}  

void *mutation(chrom popcurrent[M],char * topo[MAX_EDGE_NUM], int new_m, int times, int count_time,int time0,int bound)               // 函数：变异操作；
{
	int random;
	int row, col, col2;
	//random = rand() % 50;  // 随机产生到之间的数；
	//变异操作也要遵从一定的概率来进行，一般设置为0到0.5之
	
	while(times-- > 0 )//&& random >0 && random <=50)                              //   假定是以20%的概率进行变异
	{
		col = rand() % node_num;
		row = rand() % ( new_m - 1 ) + 1; 
		if(count_time < 20 ){
			col2 = rand() % node_num;
			bit[popcurrent[row].row][col2] ^= 1; 
		}
		bit[popcurrent[row].row][col] ^= 1;             // 0变为1,1变为0；
	}
	return(0);
}

