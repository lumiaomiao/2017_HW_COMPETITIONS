#ifndef couplemcmf_h
#define couplemcmf_h
#endif

#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <limits>
#include <string>
#include <stdlib.h>
#include <algorithm>
using namespace std;

extern int save[1000];
struct Node_c{
	int p = 0;//节点的对偶变量
	int f1 = INT_MAX;//实际中指向该点的边的起点
	int f2 = INT_MAX;//实际的流量
	int all_cost = 0;
};
struct Edge_c{
	int u;
	int v;
	int cost;
	int flow;
	int cap;
	int next;
	int rev;
};
class MinCostMaxFlow_c{
	private:
		int edgenum;
		int s,t;
		int server_cost;
		int request_flow;
		int shi;
		int price;
		int true_flow;
		int path_num;
		int *first;
		int node_num, edge_num, cust_num;
		unordered_map<int,int> custome;
		Edge_c *edge;
		Node_c *node;
		vector<int> server;
	public:
		MinCostMaxFlow_c(bool bit[1000], int size);
		~MinCostMaxFlow_c();
		void add_edge(int u, int v, int cost, int cap);
		void Initialize(char * topo[MAX_EDGE_NUM], int line_num);
		int couple(char * topo[MAX_EDGE_NUM], int line_num, bool bit[1000]);
		int find_min_seta(vector<int> &S);
		void findpath(int row, int &flow,string &out,string str);
		void get_path(string &out);
};

//初始化
MinCostMaxFlow_c::MinCostMaxFlow_c(bool bit[1000], int size){
	edgenum = 0;
	request_flow = 0;
	true_flow = 0;
	price = 0;
	shi = 0;
	path_num = 0;
	for(int i = 0; i < size; i++){
		if(bit[i]){
			server.push_back(i);
		}
	}
}

MinCostMaxFlow_c::~MinCostMaxFlow_c(){
	delete[] first;
	delete[] edge;
	delete[] node;
}
//添加边
void MinCostMaxFlow_c::add_edge(int u, int v, int cost, int cap){
	edge[edgenum].u = u;
	edge[edgenum].v = v;
	edge[edgenum].cost = cost;
	edge[edgenum].flow = 0;
	edge[edgenum].cap = cap;
	edge[edgenum].next = first[u];
	edge[edgenum].rev = edgenum + 1;
	first[u] = edgenum++;
	//cout <<node_to_edge[make_pair(u,v)] <<"("<<u<<","<<v<<")="<<edge[edgenum].cost<<endl;
	edge[edgenum].u = v;
	edge[edgenum].v = u;
	edge[edgenum].cost = cost;
	edge[edgenum].flow = 0;
	edge[edgenum].cap = cap;
	edge[edgenum].next = first[v];
	edge[edgenum].rev = edgenum - 1;
	first[v] = edgenum++;
	//cout <<node_to_edge[make_pair(v,u)] << "("<<v<<","<<u<<")="<<edge[edgenum].cost<<endl
}

//初始化所有边、节点数据
void MinCostMaxFlow_c::Initialize(char * topo[MAX_EDGE_NUM], int line_num){
	if(line_num <= 0){
		return ;
	}else{
		int i = 0;
		stringstream ss;
		ss << topo[i];
		ss >> node_num >> edge_num >> cust_num;
		//s是源节点，t是目的节点
		s = node_num + 1;
		t = node_num + 2;
		first = new int[node_num + 3];//初始化
		for(int i = 1; i < node_num + 3; i++){
			first[i] = -1;
		}
		edge = new Edge_c[(edge_num + cust_num + server.size()) * 2];
		node = new Node_c[node_num + 3];//初始化
		//cout<<"node_num:"<<node_num<<" edge_num:"<<edge_num<<" custome_num:"<<custome_num<<endl;
		
		i += 2;
		if(i < line_num){
			ss << topo[i];
			ss >> server_cost;
		//	cout<<"部署服务器成本为："<<server_cost<<endl;
		}
		
		i += 2;
		int edge_tmp = 1;
		while(edge_tmp++ <= edge_num && i < line_num){
			ss << topo[i++];
			int u,v,tmp_cap,tmp_cost;
			ss >> u >> v >> tmp_cap >> tmp_cost;
			add_edge(u+1, v+1, tmp_cost, tmp_cap);
			shi += tmp_cost;
		}
		
		//将消费节点和网络节点之间的连接关系映射到一个数组中
		i += 1;
		while(i < line_num){
			ss << topo[i++];
			int tmp_custome,tmp_netnode,tmp_flow;
			ss >>tmp_custome >> tmp_netnode >> tmp_flow;
			custome[tmp_netnode] = tmp_custome;
			add_edge(s, tmp_netnode + 1, 1, tmp_flow);
			request_flow += tmp_flow;
			shi += 1;
			
		}
		for(int i = 0; i < server.size(); i++){
			add_edge(t, server[i]+1, 0, INT_MAX);
		}
		//price += server_cost * server.size();
		//cout << "price:" << price << endl;
		
	}
}

int MinCostMaxFlow_c::find_min_seta(vector<int> &S){
	int seta = INT_MAX;
	/*for(int i = 0; i < S.size(); i++){
		cout << S[i]<<" ";
	}*/
	//cout << endl;
	for(int m = 0; m < S.size(); m++){
		for(int k = first[S[m]]; k != -1; k = edge[k].next){
			int i = edge[k].u;
			int j = edge[k].v;
			int rev = edge[k].rev;
			int tmp = edge[k].cost - node[i].p + node[j].p;
			if(tmp > 0 && tmp < seta){
				seta = tmp;
			}
			/*tmp = node[j].p - node[i].p - edge[rev].cost;
			if(tmp > 0 && tmp < seta){
				seta = tmp;
				//cout <<"*********************" << j << "->"<<i<<"************" << endl;
				//cout << tmp <<"=" << node[j].p << "-" << node[i].p << "-" <<edge[rev].cost<<endl;
			}*/
		}
	}
	//cout << S.size()<<endl;
	//cout << node[s].p<<endl;
	//cout <<"all_shi:"<<shi << endl;
	
	if(node[s].p > shi){
		return INT_MAX;
	}
	return seta;
}

int MinCostMaxFlow_c::couple(char * topo[MAX_EDGE_NUM], int line_num, bool bit[1000]){
	//step 0

	Initialize(topo, line_num);
	vector<int> S;
	S.push_back(s);
	int times = 10, old_seta = 0, old_size = 0;
	bool *belong = new bool[node_num + 3]();
	while(1){
		//step 1
		
		int seta = find_min_seta(S);
		//cout << "seta:" << seta << endl;
		if(seta == INT_MAX){
			//当前已为最大流，算法结束
			return INT_MAX;
		}
		if(abs(seta - old_seta) <= 1 && S.size() == old_size){
			times--;
		}else{
			times = 10;
		}
		old_seta = seta;
		old_size = S.size();
		if(times == 0){
			return INT_MAX;
		}
		for(int i = 0; i < S.size(); i++){
			node[S[i]].p += seta;
			//cout << S[i] << ".p=" << node[S[i]].p<<endl; 
		}
		//cout << endl;
		//step 2
		//step 3
		while(1){
			S.clear();
			for(int i = 1; i < node_num + 3; i++){
				belong[i] = 0;
			}
			node[s].f1 = 0;
			node[s].f2 = INT_MAX;
			S.push_back(s);
			belong[s] = 1;
			for(int m = 0; m < S.size(); m++){
				for(int k = first[S[m]]; k != -1; k = edge[k].next){
					int i = S[m];
					int j = edge[k].v;
					int rev = edge[k].rev;
					if(edge[k].cost == node[i].p - node[j].p && !belong[j] && edge[k].flow < edge[k].cap){
						node[j].f1 = i;
						node[j].f2 = min(node[i].f2, edge[k].cap - edge[k].flow);
						S.push_back(j);
						belong[j] = 1;
					}
				}
		
			}

			if(belong[t]){
				int delta_t = node[t].f2;
				int n1 = node[t].f1, n2 = t;
				//cout << "delta_t: " << delta_t <<endl;
				true_flow += delta_t;
				while(1){
					if(n1 > 0){
						int tmp_edge;
						for(int k = first[n1]; k != -1; k = edge[k].next){
							if(edge[k].v == n2){
								tmp_edge = k;
								break;
							}
						}
						edge[tmp_edge].flow += delta_t;
						int mins = min(edge[tmp_edge].flow , edge[edge[tmp_edge].rev].flow);
						if(mins){
							edge[tmp_edge].flow -= mins;
							edge[edge[tmp_edge].rev].flow -= mins;
							price -= 2 * mins * edge[tmp_edge].cost;
						}
						if(n1 != s){
							price += delta_t * edge[tmp_edge].cost;
						}
					}
					if(n1 == s){
						break;
					} 
					n2 = n1;
					n1 = node[n1].f1;
				}
				if(true_flow >= request_flow){
					int s_num = 0;
					for(int k = first[t]; k != -1; k = edge[k].next){
						if(edge[edge[k].rev].flow){
							price += server_cost;
							s_num++;
						}
						else{
							bit[edge[edge[k].rev].u - 1] = 0;
							save[edge[edge[k].rev].u - 1]++;
							//cout << edge[edge[k].rev].u - 1 << endl;
						}
					}
					return price;
				}else{
					//可删除
					for(int i = 0; i < node_num; i++){
						node[i].f1 = -1;
						node[i].f2 = INT_MAX;
					}
					continue;
				}
			}else{
				break;
			}
		}
	}

}

//out为输出字符串，只进不出，所以用引用，str相对很短，且变化多，所以用传值
void MinCostMaxFlow_c::findpath(int row, int &flow,string &out,string str){	
	for(int k = first[row]; k != -1; k = edge[k].next){
		stringstream ss;
		ss << row - 1 << " ";
		int rev = edge[k].rev;
		if(flow == 0){
			return;
		}
		if(edge[rev].flow !=0){
			int min_flow = min(flow, edge[rev].flow);
			flow -= min_flow;
			edge[rev].flow -= min_flow;
			if(edge[rev].flow == 0) first[row] = edge[k].next;
			if(edge[rev].u == s){
				ss << custome[row - 1] << " " << min_flow << "\n";
				out += str + ss.str();
				path_num++;
			}else{
				findpath(edge[rev].u, min_flow, out, str + ss.str());
			}
		}
		else{
			first[row] = edge[k].next;
		}
	}
}
//返回流量路径
void MinCostMaxFlow_c::get_path(string &out){
	path_num = 0;
	stringstream ss;
	string str;
	for(int k = first[t]; k != -1; k = edge[k].next){
		int rev = edge[k].rev;
		if(edge[rev].flow){
			findpath(edge[rev].u, edge[rev].flow, out, str);
		}
	}
	ss << path_num <<"\n\n";
	out =  ss.str() + out.substr(0, out.length()-1);
	//cout << out << endl;
	
}