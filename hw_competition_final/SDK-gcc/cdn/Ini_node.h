#ifndef INI_NODE_H
#define INI_NODE_H
#endif
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <stdio.h>
using namespace std;
class Ini_node{
		int node_num, edge_num, cust_num;
		int s_num;
		int *edge;
		float *cost;
		vector<pair<int,double>> weight;
		vector<int> net_cust;
		stringstream ss;
	public:
		Ini_node(){};
		~Ini_node(){};
		void Initialize(char * topo[MAX_EDGE_NUM], int line_num); 
		void sort_w();
		void prepare(char * topo[MAX_EDGE_NUM], int line_num);
		void get_ini_node(bool bit[1000], bool flag);
};

void Ini_node::Initialize(char * topo[MAX_EDGE_NUM], int line_num){
	if(line_num <= 0){
		return ;
	}else{
		int i = 0;
		stringstream ss;
		ss << topo[i];
		ss >> node_num >> edge_num >> cust_num;
		edge = new int[node_num];//初始化
		cost = new float[node_num];
		for(int i = 0; i < node_num; i++){
			edge[i] = 0;
			cost[i] = 0;
		}
		i += 4;
		int edge_tmp = 1;
		while(edge_tmp++ <= edge_num && i < line_num){
			ss << topo[i++];
			int u,v,tmp_cap,tmp_cost;
			ss >> u >> v >> tmp_cap >> tmp_cost;
			edge[u]++;
			edge[v]++;
			cost[u] += 1.0 * tmp_cap/tmp_cost;
			cost[v] += 1.0 * tmp_cap/tmp_cost;
		}
		i += 1;
		while(i < line_num){
			ss << topo[i++];
			int tmp_custome,tmp_netnode,tmp_flow;
			ss >>tmp_custome >> tmp_netnode >> tmp_flow;
			net_cust.push_back(tmp_netnode);
			
		}
	}
}

void Ini_node::sort_w(){
	for(int i = 0; i < node_num; i++){
		weight.push_back(make_pair(i, cost[i]));
	}
	for(int i = 0; i < node_num; i++){
		for(int j = i+1; j < node_num; j++){
			if(weight[i].second < weight[j].second){
				swap(weight[i], weight[j]);
			}
		}
	}
	/*for(int i = 0; i < node_num; i++){
		cout << weight[i].first <<" "<<weight[i].second << endl;
	}*/
}
void Ini_node::prepare(char * topo[MAX_EDGE_NUM], int line_num){
	Initialize(topo, line_num);
	sort_w();
}
void Ini_node::get_ini_node(bool bit[1000], bool flag){
	srand(time(0));
	int rang = cust_num;
	int n_num = cust_num * 2 / 3;
	/*for(int i = 0; i < cust_num; i++){
		int random = rand()%node_num;
		bit[rang] = 1;
	}*/
	for(int i = 0; i < cust_num; i++){
		int random = rand()%rang;
		bit[net_cust[i]] = 1;
	}


}