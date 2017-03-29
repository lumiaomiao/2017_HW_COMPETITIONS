#ifndef couplemcmf_h
#define couplemcmf_h
#endif

#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <limits>
#include <string>
using namespace std;

struct Node_c{
	int p;//节点的对偶变量
	int f1;//实际中指向该点的边的起点
	int f2;//实际的流量
	Node_c(int p = 0, int f1 = INT_MAX, int f2 = INT_MAX):p(p),f1(f1),f2(f2){}
};

struct Edge_c{
	int u;
	int v;
	int cost;
	int flow;
	int cap;
	int next;
	int prv;
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
		int *second;
		int node_num, edge_num, cust_num;
		stringstream ss;
		map<int,int> custome;
		map<pair<int,int>,int> node_to_edge;
		Edge_c *edge;
		Node_c *node;
		vector<int> server;
	public:
		MinCostMaxFlow_c(bool bit[1000], int size);
		~MinCostMaxFlow_c();
		void add_edge(int u, int v, int cost, int cap);
		void Initialize(char * topo[MAX_EDGE_NUM], int line_num);
		int couple(char * topo[MAX_EDGE_NUM], int line_num);
		int find_min_seta(vector<int> &S);
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
	/*int a[] = {6,7,13,17,35,41,48};
	vector<int> t(a,a+7);
	server = t;*/
}

MinCostMaxFlow_c::~MinCostMaxFlow_c(){
	delete[] first;
	delete[] second;
	delete[] edge;
	delete[] node;
}
//添加边
void MinCostMaxFlow_c::add_edge(int u, int v, int cost, int cap){
	edge[edgenum].u = u;
	edge[edgenum].v = v;
	node_to_edge[make_pair(u,v)] = edgenum;
	edge[edgenum].cost = cost;
	edge[edgenum].flow = 0;
	edge[edgenum].cap = cap;
	edge[edgenum].next = first[u];
	edge[edgenum].prv = second[u];
	edge[edgenum].rev = edgenum + 1;
	first[u] = edgenum;
	//cout <<node_to_edge[make_pair(u,v)] <<"("<<u<<","<<v<<")="<<edge[edgenum].cost<<endl;
	second[u] = edgenum++;
	edge[edgenum].u = v;
	edge[edgenum].v = u;
	node_to_edge[make_pair(v,u)] = edgenum;
	edge[edgenum].cost = cost;
	edge[edgenum].flow = 0;
	edge[edgenum].cap = cap;
	edge[edgenum].next = first[v];
	edge[edgenum].prv = second[v];
	edge[edgenum].rev = edgenum - 1;
	first[v] = edgenum;
	//cout <<node_to_edge[make_pair(v,u)] << "("<<v<<","<<u<<")="<<edge[edgenum].cost<<endl;
	second[v] = edgenum++;
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
		second = new int[node_num + 3];
		for(int i = 1; i < node_num + 3; i++){
			first[i] = -1;
			second[i] = -1;
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
		price += server_cost * server.size();
		//cout << "price:" << price << endl;
		
	}
}

int MinCostMaxFlow_c::find_min_seta(vector<int> &S){
	int seta = INT_MAX;
	for(int i = 0; i < S.size(); i++){
		//cout <<i<<" "<<first[S[i]]<<endl;
		for(int k = first[S[i]]; k != -1; k = edge[k].next){
			//cout << edge[k].u <<" "<< edge[k].v <<" "<< edge[k].cost<<endl;
			int j = edge[k].v;
			int tmp = edge[k].cost - node[S[i]].p + node[j].p;
			if(tmp > 0 && tmp < seta){
				seta = tmp;
				//cout << edge[k].cost <<"-" << node[S[i]].p<<"+"<< node[j].p <<"="<< tmp<<endl;
			}
		}
		for(int k = second[S[i]]; k != -1; k = edge[k].prv){
			int j = edge[k].u;
			int tmp = node[j].p - node[S[i]].p - edge[k].cost;
			if(tmp > 0 && tmp < seta){
				seta = tmp;
				//cout << edge[k].cost <<"-" << node[S[i]].p<<"+"<< node[j].p <<"="<< tmp<<endl;
			}
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

int MinCostMaxFlow_c::couple(char * topo[MAX_EDGE_NUM], int line_num){
	//step 0
	/*for(int i = 0; i < edgenum; i++){
		cout << edge[i].u <<" " << edge[i].v << " " << edge[i].cost << " "<<edge[i].cap << endl;
	}*/
	Initialize(topo, line_num);
	vector<int> S;
	S.push_back(s);
	//cout<<"--------------------------------------------------------"<<endl;
	bool *belong = new bool[node_num + 3]();
	while(1){
		//step 1
		
		int seta = find_min_seta(S);
		//cout << "seta:" << seta << endl;
		if(seta == INT_MAX){
			//当前已为最大流，算法结束
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
			for(int i = 0; i < S.size(); i++){
				for(int k = first[S[i]]; k != -1; k = edge[k].next){
					int j = edge[k].v;
					if(edge[k].cost == node[edge[k].u].p - node[j].p && !belong[j] && edge[k].flow < edge[k].cap){
						node[j].f1 = edge[k].u;
						node[j].f2 = min(node[edge[k].u].f2, edge[k].cap - edge[k].flow);
						S.push_back(j);
						belong[j] = 1;
					}
				}
				for(int k = second[S[i]]; k != -1; k = edge[k].prv){
					int j = edge[k].u;
					if(edge[k].cost == node[j].p - node[edge[k].u].p && !belong[j] && edge[edge[k].rev].flow > 0){
						node[j].f1 = -edge[k].v;
						node[j].f2 = min(node[edge[k].v].f2, edge[k].flow);
						S.push_back(j);
						belong[j] = 1;
					}
				}
			}
			
			
			
			if(belong[t]){
				path_num++;
				int delta_t = node[t].f2;
				int n1 = node[t].f1, n2 = t;
				//cout << "delta_t: " << delta_t <<endl;
				true_flow += delta_t;
				while(1){
					//cout << n1 - 1 <<" ";
					if(n1 > 0){
						int tmp_edge = node_to_edge[make_pair(n1,n2)];
						if(edge[tmp_edge].flow < edge[tmp_edge].cap){
							edge[tmp_edge].flow += delta_t;
							if(n1 != s){
								//cout << tmp_edge<<"("<<n1<<","<<n2<<")="<<edge[tmp_edge].cost<<" ";
								//cout << "cost:"<< edge[tmp_edge].cost << endl;
								price += delta_t * edge[tmp_edge].cost;
								//cout << "price:" << price << endl;
							}
						}
					}
					if(n1 < 0){
						n1 = -n1;
						int tmp_edge = node_to_edge[make_pair(n1,n2)];
						if(edge[tmp_edge].flow > 0){
							if(n1 != s){
								edge[tmp_edge].flow -= delta_t;
								price -= delta_t * edge[tmp_edge].cost;
								//cout << "price:" << price << endl;
							}
						}
					}
					if(n1 == s){
						ss << custome[n2 - 1]<<" ";
						break;
					} 
					ss << n1 - 1 <<" ";
					n2 = n1;
					n1 = node[n1].f1;
				}
				ss << delta_t <<"\n";
				//cout << endl;
				//cout << true_flow<< " " << request_flow<<endl;
				if(true_flow >= request_flow){
					//cout << "price:" << price << endl;
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

void MinCostMaxFlow_c::get_path(string &out){
	stringstream ss_tmp;
	ss_tmp << path_num <<"\n\n";
	out = ss_tmp.str() + ss.str();
	out = out.substr(0,out.length()-1);
}