#ifndef GA_h
#define GA_h
#endif


#include <stdio.h>
//#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <sstream>
//#include "lib_io.h"
#include "MinimumCostMaximumFlow.h"
using namespace std;


const int n=1000;  //                                             n是网络节点的个数    20需要改正
const int M = 4;  //                                              暂定种群规模为M

int linenum;
int node_num, edge_num, cust_num;

typedef struct Chrom                           // 结构体类型，为单个染色体的结构;
{
	bool bit[n];//
	int size;             //服务器节点的实际个数
	int fit;//适应值
	double rfit;//相对的fit值，即所占的百分比
	double cfit;//积累概率
}chrom;
//定义将会用到的几个函数； 
void GA(char * topo_s[MAX_EDGE_NUM],int line_num);                                                           
void *evpop(chrom popcurrent[M],int node_num,int s_num,char * topo[MAX_EDGE_NUM]);//进行种群的初始化             根据网络节点 成为服务器节点的可能性 进行初始化                                     
int fitness(chrom &chom,char * topo[MAX_EDGE_NUM]);
void *pickchroms(chrom popcurrent[M]);//选择操作
void *pickchroms_new(chrom popcurrent[M]); // 轮盘赌算法
void *crossover(chrom popcurrent[M],char * topo[MAX_EDGE_NUM]);//交叉操作
void *mutation(chrom popcurrent[M],char * topo[MAX_EDGE_NUM]);//突变
chrom popcurrent[M];   // 初始种群规模为M；
chrom popnext[M];  //变化时的种群


void GA(char * topo[MAX_EDGE_NUM],int line_num,bool result[1000])                                    // 主函数；
{
	
	int num=20;                                    // 迭代次数；                暂定迭代次数是100
	int i, l, Min, k;
	linenum = line_num;
	stringstream ss;
	ss << topo[0];
	ss >> node_num >> edge_num >> cust_num;
	srand(time(0));                   //                              设置随机数的种子，否则每次运行得到的随机数都是一样的。
	evpop(popcurrent, node_num, cust_num,topo);    // 产生初始种群；
	Min = popcurrent[0].fit;//对Min值进行初始化
	for (i = 0; i< num; i++)                          // 开始迭代；
	{

		//cout<<"i is "<<i<<endl;                 // 输出当前迭代次数；

		
		pickchroms(popcurrent);                    // 挑选优秀个体；
		crossover(popcurrent,topo);                     // 交叉得到新个体；
		mutation(popcurrent,topo);                      // 变异得到新个体；

	}  
	for (l = 0; l<M; l++)
	{
		if (popcurrent[l].fit < Min)
		{
			Min = popcurrent[l].fit;
			k = l;
		}

	}
	for(i = 0; i < node_num; i++){
		result[i] = popcurrent[k].bit[i];
	}
	//cout <<"The location is"<< popcurrent[k].bit << endl;
	//system("pause");
}




int fitness(chrom &chom,char * topo[MAX_EDGE_NUM])
{  
	//使用最小费用流计算消费值作为适应值,把值赋给 .fit
	MinimumCostMaximumFlow mincostmaxclow(chom.bit, node_num);
	chom.fit = mincostmaxclow.get_cost(topo,linenum);
	return chom.fit;
}

void *evpop(chrom popcurrent[M],int node_num, int s_num,char * topo[MAX_EDGE_NUM])   //                                需要初始化M个极有可能可行的解
{
	//                      在此处初始化M个染色体，记为popcurrent[j]

	for(int i = 0; i < M; i++){
		for(int j = 0; j < s_num ; j++){
			int random = rand()% node_num;
			popcurrent[i].bit[random] = 1;
		}
		if(fitness(popcurrent[i],topo) == 0){
			for(int q = 0; q < node_num; q++){popcurrent[i].bit[q] = 0;}
			i--;
		}
		/*for(int q = 0; q < node_num; q++){
			if(popcurrent[i].bit[q]) cout <<q <<" ";
		}
		cout <<endl;
		*/
	}
	return (0);
}

//基于轮盘赌选择方法，进行基因型的选择
void *pickchroms_new(chrom popcurrent[M])//计算概率
{
	double p;
	double sum = 0.0;
	//find the total fitness of the population
	for (int men = 0; men < M; men++)
	{
		sum = sum + popcurrent[men].fit;
	}
	//calculate the relative fitness of each member
	for (int men = 0; men < M; men++)
	{
		popcurrent[men].rfit = 1-popcurrent[men].fit / sum;         //最小费用流越小越好，故在进行轮盘赌算法前用1去减  
	}
	//calculate the cumulative fitness,即计算积累概率
	popcurrent[0].cfit = popcurrent[0].rfit;
	for (int men = 1; men < M; men++)
	{
		popcurrent[men].cfit = popcurrent[men - 1].cfit + popcurrent[men].rfit;
	}

	for (int i = 0; i < M; i++)
	{//产生0~1之间的随机数
		p = rand() % 2;//
		if (p < popcurrent[0].cfit)
		{
			popnext[i] = popcurrent[0];
			continue;
		}
		for (int j = 0; j < M-1; j++)
		{
			if (popcurrent[j].cfit <= p && p < popcurrent[j + 1].cfit)
			{
				popnext[i] = popcurrent[j + 1];
			}
		}
	}
	//  Overwrite the old population with the new one.
	//
	for (int i = 0; i < M; i++)
	{
		popcurrent[i] = popnext[i];
	}
	return(0);
}
void *pickchroms(chrom popcurrent[M])          // 函数：选择个体；
{
	int i, j;
	chrom temp;                                // 中间变量

	for (i = 0; i<M-1; i++)                           // 根据个体适应度来排序；（冒泡法）
	{
		for (j = 0; j< M-1-i; j++)
		{
			if (popcurrent[j + 1].fit>popcurrent[j].fit)
			{
				temp = popcurrent[j + 1];
				popcurrent[j + 1] = popcurrent[j];
				popcurrent[j] = temp;

			}
		}
	}
	
	return(0);
}


void *crossover(chrom popcurrent[M],char * topo[MAX_EDGE_NUM])              // 函数：交叉操作；
{

	int random;
	int i;
	//srand(time(0)); 
	random = rand();                             // 随机产生交叉点；
	random = random % n;
	for (int j = 0; j < M / 2; j++)                    //只保留适应度比较大的前M/2个，剩余M/2个变为交叉之后的结果
	{
		// 交叉点控制在0到n-1之间；随机产生0到n-1的整数
		for (i = 0; i < random; i++)
		{
			popcurrent[M / 2 + j].bit[i] = popcurrent[j].bit[i];
		}

		for (i = random; i < n; i++)                      // crossing the bits beyond the cross point index
		{
			popcurrent[M / 2 + j].bit[i] = popcurrent[M / 2 - 1 - j].bit[i];
		}
	}
	for (i = 0; i<M; i++)
	{
		popcurrent[i].fit = fitness(popcurrent[i],topo);        // 为新个体计算适应度值；
	}

	return(0);
}

void *mutation(chrom popcurrent[M],char * topo[MAX_EDGE_NUM])               // 函数：变异操作；
{

	int random;
	int row, col;
	//srand(time(0)); 
	random = rand() % 50;  // 随机产生到之间的数；
	//变异操作也要遵从一定的概率来进行，一般设置为0到0.5之间
	//
	if (random >10 && random <=15)                              //   假定是以10%的概率进行变异
	{
		col = rand() % n;                            // 随机产生要变异的基因位号；
		row = rand() % M;                            // 随机产生要变异的染色体号；

		popcurrent[row].bit[col] = 1- popcurrent[row].bit[col];             // 0变为1,1变为0；
		popcurrent[row].fit = fitness(popcurrent[row],topo);     // 计算变异后的适应度值；
	}

	return(0);
}
