#ifndef MinimumCostMaximumFlow_h
#define MinimumCostMaximumFlow_h
#endif

//#include "lib_io.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <map>
#include <vector>
#include <sstream>
#include <limits.h>
#include <algorithm>
#include <time.h>
using namespace std;

#define max_node_num 1000

struct Edge{
	//链路的数据结构
	int cap;//最大容量
	int cost;//费用
	int flow;//流量
	Edge(int cap = 0, int cost = INT_MAX, int flow = 0):cap(cap),cost(cost),flow(flow){}
};


Edge **edge;
map<int,int> custome;
int **W;//流量矩阵
int **U,**S;
//剩余网络和费用矩阵
int **capacity,**cost;
int **sign;
bool ini = false;//所有矩阵是否已经申请内存空间？
		
		
class MinimumCostMaximumFlow{
	private:
		char * filename;
		vector<int> server;
		int nodenum, edgenum, customenum;
		int s,t;//s是源节点，t是目的节点
		int server_cost;//部署服务器节点的费用
		int request_flow;
		int true_flow;
		int price;
		int m,n;
		int path_num;
	private:
		bool InitializeFW(char * topo[MAX_EDGE_NUM], int line_num);
		void InitializeServer();
		void InitializeUS(int **x);
		void SearchByMaxCapPath();
		void ConstructCapCosMat();
		int add(int x,int y);
		int NegativeCycleFound();
		void NegativeCycleAjust();
		void MaxFlow(char * topo[MAX_EDGE_NUM], int line_num);
		void MinCostMaxFlow(char * topo[MAX_EDGE_NUM], int line_num);
		void findpath(int row, int &flow,string &out,string str);
	public:
		int node_num,edge_num,custome_num;
		MinimumCostMaximumFlow(bool s[1000], int size){
			cout <<"hellom";
			request_flow = 0;
			true_flow = 0;
			price = 0;
			if(s){
				for(int i = 0; i < size; i++){
					if(s[i]){
						cout << i<<" ";
						server.push_back(i);
					}
				}
				cout << endl;
			}
		}
		virtual ~MinimumCostMaximumFlow(){}
		void get_num(int &nodenum, int &edgenum, int &customenum){
			nodenum = node_num;
			edgenum = edge_num;
			customenum = custome_num;
			return;
		}
		int get_cost(char *topo[MAX_EDGE_NUM], int line_num);
		void get_allpath(string &out);
};

//获取比赛数据,初始化F，W
//若获取从成功则返回true，否则返回false
bool MinimumCostMaximumFlow::InitializeFW(char * topo[MAX_EDGE_NUM], int line_num){
	if(line_num <= 0){
		return false;
	}else{
		int i = 0;
		stringstream ss;
		ss << topo[i];
		ss >> node_num >> edge_num >> custome_num;
		//s是源节点，t是目的节点
		s = node_num;
		t = node_num + 1;
		//cout<<"node_num:"<<node_num<<" edge_num:"<<edge_num<<" custome_num:"<<custome_num<<endl;
		
		i += 2;
		if(i < line_num){
			ss << topo[i];
			ss >> server_cost;
		//	cout<<"部署服务器成本为："<<server_cost<<endl;
		}else{
			return false;
		}
		if(ini == false){
			edge = new Edge*[node_num + 2];
			W = new int*[node_num + 2];
			U = new int*[node_num + 2];
			S = new int*[node_num + 2];
			capacity = new int*[node_num + 2];
			cost = new int*[node_num + 2];
			sign = new int*[node_num + 2];
			for(int n_num = 0; n_num < node_num + 2; n_num++){
				edge[n_num] = new Edge[node_num + 2];
				W[n_num] = new int[node_num + 2];
				U[n_num] = new int[node_num + 2];
				S[n_num] = new int[node_num + 2];
				capacity[n_num] = new int[node_num + 2];
				cost[n_num] = new int[node_num + 2];
				sign[n_num] = new int[node_num + 2];
			}
			ini = true;
		}
		for(int row = 0; row < node_num + 2; row++){
			for(int col = 0; col < node_num + 2; col++){
				edge[row][col].cap = 0;
				edge[row][col].cost = 0;
				edge[row][col].flow = 0;
				W[row][col] = 0;
			}
		}
		
		i += 2;
		//edge = new Edge[1000][1000];
		//将图映射到一个二维数组中，且第node_num个节点是最小费用最大流的起始节点
		int edge_tmp = 1;
		while(edge_tmp++ <= edge_num && i < line_num){
			ss << topo[i++];
			int row,col,tmp_cap,tmp_cost;
			ss >> row >> col >> tmp_cap >> tmp_cost;
			edge[row][col].cap = tmp_cap;//路径的容量
			edge[row][col].cost = tmp_cost;//单位费用
			W[row][col] = tmp_cap;//权重
			edge[col][row].cap = tmp_cap;
			edge[col][row].cost = tmp_cost;
			W[col][row] = tmp_cap;
			
		}
		
		int node_tmp = 0;
		while(node_tmp < node_num + 2){
			//同一个节点的流量为无穷大，但默认值为0
			//同一个节点的费用为0，但默认值为无穷大
			edge[node_tmp][node_tmp].cap = INT_MAX;
			edge[node_tmp][node_tmp].cost = 0;
			W[node_tmp][node_tmp] = INT_MAX;
			node_tmp++;
		}
		
		//将消费节点和网络节点之间的连接关系映射到一个数组中
		i += 1;
		while(i < line_num){
			ss << topo[i++];
			int tmp_custome,tmp_netnode,tmp_flow;
			ss >>tmp_custome >> tmp_netnode >> tmp_flow;
			custome[tmp_netnode] = tmp_custome;
			//edge中的node_num个节点假设为源节点
			edge[node_num][tmp_netnode].cap = tmp_flow;
			edge[node_num][tmp_netnode].cost = 0;
			W[node_num][tmp_netnode] = tmp_flow;
			edge[tmp_netnode][node_num].cap = tmp_flow;
			edge[tmp_netnode][node_num].cost = 0;
			W[tmp_netnode][node_num] = tmp_flow;
			request_flow += tmp_flow;
			
		}
		return true;
	}
}

void MinimumCostMaximumFlow::InitializeServer(){
	for(unsigned int i = 0; i < server.size(); i++){
		edge[t][server[i]].cost = 0;
		edge[t][server[i]].cap = INT_MAX;
		W[t][server[i]] = INT_MAX;
		edge[server[i]][t].cost = 0;
		edge[server[i]][t].cap = INT_MAX;
		W[server[i]][t] = INT_MAX;
	}
}

//根据算法，初始化剩余网络和费用矩阵
void MinimumCostMaximumFlow::InitializeUS(int **x){
	//step 2,3
	for(int i = 0; i < node_num + 2; i++){
		for(int j = 0; j < node_num + 2; j++){
			U[i][j] = x[i][j];
			S[i][j] = j;
		}
	}
}

//若返回值为false，说明没有计算出最大流；若返回值为true，则说明计算出了最大值
//找出当前s到t的最大流
void MinimumCostMaximumFlow::SearchByMaxCapPath(){
	
	//根据遗传算法，获取目的节点与网络节点之间的关系	
	//step 4,5
	int i,j,k;
	int l,r;
	int seta;
	while(1){
		
		InitializeUS(W);
		/*cout<<"------------------------------W,U,S------------------------------"<<endl;
		/测试输出
		for(int row = 0; row < node_num+2; row++){
			for(int col = 0; col <node_num+2; col++){
				cout<<"("<<W[row][col]<<","<<U[row][col]<<","<<S[row][col]<<") ";
			}
			cout<<endl;
			cout<<endl;
		}
		cout<<"----------------------------------------------------------------"<<endl;*/
		for(k = 0; k < node_num + 2; k++){
			for(i = 0; i < node_num + 2; i++){
				for(j = 0; j < node_num + 2; j++){
					int tmp = min(U[i][k],U[k][j]);
					if(U[i][j] < tmp){
						U[i][j] = tmp;
						S[i][j] = S[i][k];
					}
				}
			}
		}
		if(U[s][t] == 0) break;
		else{
			seta = U[s][t];
			l = s;
			r = S[s][t];
			while(1){
				W[l][r] -= seta;
				W[r][l] += seta;
				edge[l][r].flow += seta - min(edge[r][l].flow ,seta);
				edge[r][l].flow -= min(edge[r][l].flow ,seta);
				if(r == t) break;
				else{
					l = r;
					r = S[r][t];
				}
			}
		}
		int tmp_flow = 0;
		for(int row = 0; row < node_num + 2; row++)
		{
			if(edge[row][node_num + 1].flow) tmp_flow += edge[row][node_num + 1].flow;
		}
		//cout << tmp_flow <<" "<<request_flow<<endl;
	}
/*	cout<<"------------------------------------Flow---------------------------------------"<<endl;
	for(int row = 0; row < node_num + 2; row++){
		for(int col = 0; col <node_num + 2; col++){
			cout<<edge[row][col].flow<<" ";
		}
		cout<<endl;
		cout<<endl;
	}
	cout<<"----------------------------------------------------------------"<<endl;*/
}

//建立N关于f的剩余网络的容量矩阵capacity和费用矩阵cost
void MinimumCostMaximumFlow::ConstructCapCosMat(){
	int i,j;
	for( i = 0; i < node_num + 2 ; i++){
		for( j = i; j < node_num + 2; j++){
			if ( i == j ){
				capacity[i][j] = INT_MAX;
				cost[i][j] = 0;
			}else{
				if(edge[i][j].cap > 0){
					if(edge[j][i].cap == 0){
						//这种情况不可能有，因为是无向图,所以c[i][j] == c[j][i]
						capacity[i][j] = edge[i][j].cap - edge[i][j].flow;
						capacity[j][i] = edge[i][j].flow;
						if(edge[i][j].flow < edge[i][j].cap){
							cost[i][j] = edge[i][j].cost ;
							sign[i][j] = 1;
						}else{
							cost[i][j] = INT_MAX;
							sign[i][j] = 0;
						}
						
						if(edge[i][j].flow > 0){
							cost[j][i] = - edge[i][j].cost;
							sign[j][i] = -1;
						}else{
							cost[j][i] = INT_MAX;
							sign[j][i] = 0;
						}
					}else{
						if(edge[i][j].flow == 0 && edge[j][i].flow > 0){
							capacity[j][i] = edge[j][i].cap - edge[j][i].flow;
							capacity[i][j] = edge[j][i].flow;
							cost[i][j] = -edge[j][i].cost;
							sign[i][j] = -1;
							if(edge[j][i].flow < edge[j][i].cap){
								cost[j][i] = edge[j][i].cost;
								sign[j][i] = 1;
							}else{
								//此时路不通了
								cost[j][i] = INT_MAX;
								sign[j][i] = 0;
							}
						}
						if(edge[i][j].flow > 0 && edge[j][i].flow == 0){
							capacity[i][j] = edge[i][j].cap - edge[i][j].flow;
							capacity[j][i] = edge[i][j].flow;
							cost[j][i] = -edge[i][j].cost;
							sign[j][i] = -1;
							if(edge[i][j].flow < edge[i][j].cap){
								cost[i][j] = edge[i][j].cost;
								sign[i][j] = 1;
							}else{
								cost[i][j] = INT_MAX;
								sign[i][j] = 0;
							}
							
						}
						//两边流量都为0时
						if(edge[i][j].flow == 0 && edge[j][i].flow == 0){
							capacity[i][j] = edge[i][j].cap;
							cost[i][j] = edge[i][j].cost;
							sign[i][j] = 1;
							capacity[j][i] = edge[j][i].cap;
							cost[j][i] = edge[j][i].cost;
							sign[j][i] = 1;
						}
					}
				}else{
					//当这条边的容量为0，也就是说这条边不存在
					if(edge[j][i].cap == 0){
						capacity[i][j] = 0;
						cost[i][j] = INT_MAX;
						sign[i][j] = 0;
						capacity[j][i] = 0;
						cost[j][i] = INT_MAX;
						sign[j][i] = 0;
					}else{
						//这种情况也不存在，因为无向图中，这两条边的容量一定是相同的
						capacity[j][i] = edge[j][i].cap - edge[j][i].flow;
						capacity[i][j] = edge[j][i].flow;
						if(edge[j][i].flow < edge[j][i].cap){
							cost[j][i] = edge[j][i].cost;
							sign[j][i] = 1;
						}else{
							cost[j][i] = INT_MAX;
							sign[j][i] = 0;
						}
						
						if(edge[j][i].flow > 0){
							cost[i][j] = - edge[j][i].cost;
							sign[i][j] = -1;
						}else{
							cost[i][j] = INT_MAX;
							sign[i][j] = 0;
						}
					}
				}
			}
		}
	}
}

//两个剩余容量想加
int MinimumCostMaximumFlow::add(int x,int y){
	int value;
	if((x == INT_MAX) || (y == INT_MAX)) value = INT_MAX;
	else value = x + y;
	return value;
}

int MinimumCostMaximumFlow::NegativeCycleFound(){
	//判断是否存在负费用回路
	int i, j, k;
	int value;
	
	InitializeUS(cost);
	
	m = 0;
	for(k = 0; k <= node_num ; k++){
		for(i = 0; i <= node_num + 1; i++){
			for(j = 0; j <= node_num + 1; j++){
				if(U[i][j] > add(U[i][k],U[k][j])){
					U[i][j] = add(U[i][k],U[k][j]);
					S[i][j] = S[i][k];
				}
			}
		}
		
		for(i = k+1; i <= node_num + 1; i++){
			if(U[i][i] < 0){
				//若有负费用回路，则m = 这个存在负费用回路的点
				m = i;
				value = 1;
				return value;
			}
		}
	}
	value = 0;
	return value;
}

void MinimumCostMaximumFlow::NegativeCycleAjust(){
	int l,r;
	int seta;
	l = m;
	seta = INT_MAX;
	while(1){
		r = S[l][m];
		if(seta > capacity[l][r]) seta = capacity[l][r];
		l = r;
		if(l == m) break;
	}
	l = m;
	while(1){
		r = S[l][m];
		if(sign[l][r] == 1) edge[l][r].flow = edge[l][r].flow + seta;
		else edge[r][l].flow -= seta;
		l = r;
		if(l == m) break;
	}
}

//先初始化，再寻找一条最大流
void MinimumCostMaximumFlow::MaxFlow(char * topo[MAX_EDGE_NUM], int line_num){
	//初始化所有矩阵
	InitializeFW(topo, line_num);
	InitializeServer();
	cout<<"--------------------------初始矩阵--------------------------------"<<endl;
	/*for(int row = 0; row < node_num+2; row++){
		for(int col = 0; col <node_num+2; col++){
			cout<<"("<<edge[row][col].cap<<","<<edge[row][col].cost<<") ";
		}
		cout<<endl;
		cout<<endl;
	}
	cout<<"-------------------------------------------------------------------"<<endl;*/
	//计算一条可行路径
	SearchByMaxCapPath();
}

//寻找最小费用最大流
void MinimumCostMaximumFlow::MinCostMaxFlow(char *topo[MAX_EDGE_NUM], int line_num){
	
	MaxFlow(topo,line_num);
	int rec_times = 5;//构造剩余网络的次数只能为5次
	while(rec_times--){
		//建立N关于f的剩余网络的容量矩阵capacity和费用矩阵cost
		ConstructCapCosMat();
		//查找是否还有可行路径，如果有则继续调整，如果没有，则跳出循环，说明路径已经最优
		if(NegativeCycleFound() == 1){
			NegativeCycleAjust();
		}
		else break;
		int tmp_flow = 0;
		for(int row = 0; row < node_num + 2; row++)
		{
			if(edge[row][node_num + 1].flow) tmp_flow += edge[row][node_num + 1].flow;
		}
		cout << tmp_flow <<endl;
	}
	for(int row = 0; row < node_num+2; row++){
		for(int col = 0; col < node_num+2; col++){
			if(edge[row][col].flow){
				price += edge[row][col].flow * edge[row][col].cost;
			}
		}

	}
	/*cout<<"-------------------------最终流量矩阵--------------------------------"<<endl;
	for(int row = 0; row < node_num+2; row++){
		for(int col = 0; col < node_num+2; col++){
			if(edge[row][col].flow){
				cout<<row << "->" <<col <<" :"<< edge[row][col].flow<<endl;
				price += edge[row][col].flow * edge[row][col].cost;
			}
		}
		cout<<"-------------------------------------------------------------------"<<endl;
		cout<<endl;
	}
	cout<<"-------------------------------------------------------------------"<<endl;*/
}

//若算出来的最大流小于消费节点需要的最大流，则返回false
//否则返回true，并返回所有的费用
int MinimumCostMaximumFlow::get_cost(char *topo[MAX_EDGE_NUM], int line_num){
	MinCostMaxFlow(topo,line_num);
	for(int i = 0; i < node_num + 2; i++){
		true_flow += edge[s][i].flow;
		if(edge[i][node_num + 1].flow){
			price += server_cost;
		}
	}
	for(int col = node_num + 1; col < node_num + 2; col++){
		for(int row = 0; row < node_num + 2; row++){
			if(edge[row][col].flow){
				cout<<row << "->" <<col <<" :"<< edge[row][col].flow<<endl;
			}
		}
		cout<<endl;
	}
	for(int i = 0; i < server.size(); i++){
		cout << server[i]<<" ";
	}
	
	cout<<endl;
	cout << "消费节点需要的流量：" << request_flow << endl;
	cout << "实际算出来的总流量：" << true_flow << endl;
	cout << "所有费用总和：" << price << endl;
	if(request_flow <= true_flow){
		return price;
	}else{
		return INT_MAX;
	}
	cout<<endl;
	cout<<endl;
}

//out为输出字符串，只进不出，所以用引用，str相对很短，且变化多，所以用传值
void MinimumCostMaximumFlow::findpath(int row, int &flow,string &out,string str){
	stringstream ss;
	ss << row << " ";
	for(int i = 0; i <= node_num; i++){
		if(flow == 0){
			return;
		}
		if(edge[i][row].flow !=0){
			int min_flow = min(flow,edge[i][row].flow);
			flow -= min_flow;
			edge[i][row].flow -= min_flow;
			if(i == node_num){
				ss <<custome[row] <<" " << min_flow <<"\n";;
				out += str + ss.str();
				path_num++;
			}else{
				findpath(i, min_flow, out, ss.str());
			}
		}
	}
}
//返回流量路径
void MinimumCostMaximumFlow::get_allpath(string &out){
	path_num = 0;
	stringstream ss;
	string str;
	/*for(int col = node_num + 1; col < node_num + 2; col++){
		for(int row = 0; row < node_num + 2; row++){
			if(edge[row][col].flow){
				cout<<row << "->" <<col <<" :"<< edge[row][col].flow<<endl;
			}
		}
		cout<<"-------------------------------------------------------------------"<<endl;
		cout<<endl;
	}
	for(int i = 0; i < server.size(); i++){
		cout << server[i]<<" ";
	}
	
	cout<<endl;*/
	for(int row = 0; row <= node_num; row++){
		if(edge[row][node_num+1].flow !=0){
			findpath(row, edge[row][node_num+1].flow, out, str);
		}
	}
	ss << path_num <<"\n\n";
	out =  ss.str() + out.substr(0,out.length()-2);
	//cout << out << endl;
	
}