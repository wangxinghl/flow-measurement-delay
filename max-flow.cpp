/*
* author: wangxing
* date: 2017.12.12
*/

#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>
#include <assert.h>
#include "max-flow.h"

MaxFlow::MaxFlow()
{
	m_topo = NULL;
	m_nodeNum = 0;
	m_edgeNum = 0;
	m_totalNum = 0;
	m_src = INT_MAX;
	m_dst = INT_MAX;
}

MaxFlow::~MaxFlow()
{
	m_nodes.clear();
	m_edges.clear();
}

void MaxFlow::Calculate(Topology *topo, int depth, int max)
{
	Initialize(topo, depth, max);
	MaxFlowCalculate();		// get the initial results
	//ShowSolution();

	//vector<FlowNode> nodes_copy;
	vector<FlowEdge> edges_copy;
	while (1) {
		edges_copy = m_edges;
		vector<int> choosed = GetChoosedNode();	// 返回流量从小到大的非零节点编号
		while (!choosed.empty()) {
			int first = choosed[0];
			ReleaseFlow(first);
			m_edges[m_nodes[first].adjEdge].cap = 0;
			MaxFlowCalculate();

			if (IsCoverAll())
				break;
			else {
				choosed.erase(choosed.begin());
				m_edges = edges_copy;
			}
		}
		if (choosed.empty()) break;
		//ShowSolution();
	}

	//cout << "node:" << m_nodeNum << " edge:" << m_edgeNum << "\nCalculate over!\n";
}

void MaxFlow::Initialize(Topology *topo, int depth, int max)  
{
	m_topo = topo;
	m_nodeNum = topo->GetNodeNum();
	m_edgeNum = topo->GetEdgeNum();
	m_totalNum = m_nodeNum + m_edgeNum + 2;	// Total flow node = edge + node + 2 (source node and destination node)
	m_src = m_totalNum - 2;
	m_dst = m_totalNum - 1;

	vector<map<int, int> > edge2node;		// Edge to node, vector<edgeId>, map<nodeId, distance>
	for (int i = 0; i < m_edgeNum; i++) {
		edge2node.push_back(topo->GetEdgeAdjacentNode(i, depth));
	}

	/* Create Flow Topology */
	m_nodes.resize(m_totalNum);

	m_nodes[m_src].adjEdge = 0;		// From source node to the first layer
	for (int i = 0; i < m_edgeNum; i++) {
		m_adj[m_src][i] = m_edges.size();
		FlowEdge edge;
		edge.src = m_src;
		edge.dst = i;
		edge.next = m_edges.size() + 1;
		m_edges.push_back(edge);
	}
	m_edges.back().next = INT_MAX;

	for (int i = 0; i < m_edgeNum; i++) {	// From the first layer to the second layer
		m_nodes[i].adjEdge = m_edges.size();
		map<int, int> &temp = edge2node[i];
		for (map<int, int>::iterator it = temp.begin(); it != temp.end(); it++) {
			m_adj[i][m_edgeNum + it->first] = m_edges.size();
			FlowEdge edge;
			edge.src = i;
			edge.dst = m_edgeNum + it->first;
			edge.cost = it->second;
			edge.next = m_edges.size() + 1;
			m_edges.push_back(edge);
		}
		m_edges.back().next = INT_MAX;
	}

	for (int i = m_edgeNum; i < m_src; i++) {	// From the second layer to destination node
		m_nodes[i].adjEdge = m_edges.size();
		m_adj[i][m_dst] = m_edges.size();
		FlowEdge edge;
		edge.src = i;
		edge.dst = m_dst;
		//edge.cost = INT_MAX;	// need to rewrite
		edge.cap = max;
		m_edges.push_back(edge);
	}
}

void MaxFlow::MaxFlowCalculate(void)
{
	while (1) {
		ResetFlag();
		AddTag();
		if (m_nodes[m_dst].flag == INT_MAX)
			break;
		Adjust();
	}
}

void MaxFlow::AddTag(void)
{
	int adjEdge;
	FlowEdge *p, *q;
	
	/* from source node to the first layer */
	adjEdge = m_nodes[m_src].adjEdge;
	while (adjEdge != INT_MAX) {
		p = &m_edges[adjEdge];
		if (p->flow < p->cap) {
			m_nodes[p->dst].flag = m_src;
			m_nodes[p->dst].left = 1;
		}
		adjEdge = p->next;
	}
	
	/* from the first layer to the second layer */
	for (int i = 0; i < m_edgeNum; i++) {
		if (m_nodes[i].flag != INT_MAX) {	// link have flag
			adjEdge = m_nodes[i].adjEdge;
			while (adjEdge != INT_MAX) {
				p = &m_edges[adjEdge];		// the first layer ---> the second layer
				q = &m_edges[m_nodes[p->dst].adjEdge];	// the second layer ---> destination
				if (p->flow < p->cap && q->flow < q->cap) {	// 有可用容量，目的节点可行备选节点
					if (m_nodes[p->dst].flag == INT_MAX) {  // have no flag, give flag
						m_nodes[p->dst].flag = i;
						m_nodes[p->dst].left = 1;
						m_nodes[p->dst].cost = p->cost;
					}
					else if (p->cost < m_nodes[p->dst].cost) {	// have flag, modify flag. minimize distance
						m_nodes[p->dst].flag = i;
						m_nodes[p->dst].cost = p->cost;
					}
				}
				adjEdge = p->next;
			}
		}
	}

	/* from the second layer to destination node */
	for (int i = m_edgeNum; i < m_src; i++) {
		if (m_nodes[i].flag != INT_MAX ) {
			if (m_nodes[m_dst].flag == INT_MAX) {
				m_nodes[m_dst].flag = i;
				m_nodes[m_dst].left = 1;
				m_nodes[m_dst].cost = m_nodes[i].cost;
			}
			else if (m_nodes[i].cost < m_nodes[m_dst].cost) {
				m_nodes[m_dst].flag = i;
				m_nodes[m_dst].cost = m_nodes[i].cost;
			}
		}
	}


	//int min_cost = INT_MAX;
	//for (int i = m_edgeNum; i < m_src; i++) {
	//	if (m_nodes[i].flag != INT_MAX && m_nodes[i].cost < INT_MAX)
	//		min_cost = m_nodes[i].cost;
	//}
	//if (min_cost == INT_MAX) return;
	//for (int i = m_edgeNum; i < m_src; i++) {
	//	if (m_nodes[i].flag != INT_MAX && m_nodes[i].cost == min_cost) {
	//		p = &m_edges[m_nodes[i].adjEdge];
	//		if (m_nodes[m_dst].flag == INT_MAX) {  // have no flag, give flag
	//			m_nodes[m_dst].flag = i;
	//			m_nodes[m_dst].left = 1;
	//			m_nodes[m_dst].cost = p->flow;
	//		}
	//		else if (p->flow > m_nodes[m_dst].cost) {	// have flag, modify flag. minimize distance
	//			m_nodes[m_dst].flag = i;
	//			m_nodes[m_dst].cost = p->flow;
	//		}
	//	}
	//}
}

void MaxFlow::Adjust(void)
{
	int cur = m_dst;
	while (cur != m_src) {
		m_edges[m_adj[m_nodes[cur].flag][cur]].flow++;
		cur = m_nodes[cur].flag;
	}
}

void MaxFlow::ResetFlag(void)
{
	for (int i = 0; i < m_totalNum; i++)
		m_nodes[i].flag = INT_MAX;
}

void MaxFlow::ReleaseFlow(int node)
{
	/* from the second layer to destination node */
	m_edges[m_nodes[node].adjEdge].flow = 0;

	set<int> temp;
	/* from the first layer to the second layer */
	int adjEdge;
	FlowEdge *p;
	for (int i = 0; i < m_edgeNum; i++) {
		adjEdge = m_nodes[i].adjEdge;
		while (adjEdge != INT_MAX) {
			p = &m_edges[adjEdge];
			if (p->dst == node && p->flow == 1) {
				p->flow = 0;
				temp.insert(i);
				break;
			}
			adjEdge = p->next;
		}
	}
	
	/* from source node to the first layer */
	adjEdge = m_nodes[m_src].adjEdge;
	while (adjEdge != INT_MAX) {
		p = &m_edges[adjEdge];
		if (temp.find(p->dst) != temp.end()) {
			p->flow = 0;
		}
		adjEdge = p->next;
	}
}

vector<int> MaxFlow::GetChoosedNode(void)
{
	vector<int> choose;
	for (int i = m_edgeNum; i < m_src; i++) {
		if (m_edges[m_nodes[i].adjEdge].flow != 0) {
			choose.push_back(i);
		}
	}
	sort(choose.begin(), choose.end(), [&](int a, int b){ return m_edges[m_nodes[a].adjEdge].flow < m_edges[m_nodes[b].adjEdge].flow; });
	return choose;
}

bool MaxFlow::IsCoverAll(void)
{
	int adjEdge = m_nodes[m_src].adjEdge;
	while (adjEdge != INT_MAX){
		if (m_edges[adjEdge].flow == 0) {
			return false;
		}
		adjEdge = m_edges[adjEdge].next;
	}
	return true;
}

void MaxFlow::ShowNode(void)
{
	for (size_t i = 0; i < m_nodes.size(); i++) {
		cout << "node " << i << " (" << m_nodes[i].flag << "," << m_nodes[i].left << ")\n";
	}
}

void MaxFlow::ShowEdge(void)
{
	int adjEdge;
	FlowEdge *p;
	for (size_t i = 0; i < m_nodeNum; i++) {
		adjEdge = m_nodes[i].adjEdge;
		while (adjEdge != INT_MAX) {
			p = &m_edges[adjEdge];
			if (p->flow != 0) {
				cout << "node " << i << "-->" << p->dst << "(" << p->flow << ")\n";
			}
			adjEdge = p->next;
		}
	}
}

void MaxFlow::ShowSolution(ostream &out)
{
	int adjEdge;
	FlowEdge *p;
	map<int, map<int, pair<int, int> > > result;	// map<node_id, map<link_id, link>>
	for (int i = 0; i < m_edgeNum; i++) {	// Get result
		adjEdge = m_nodes[i].adjEdge;
		while (adjEdge != INT_MAX) {
			p = &m_edges[adjEdge];
			if (p->flow != 0) {
				result[p->dst - m_edgeNum][i] = m_topo->GetEdge(i);
				break;
			}
			adjEdge = p->next;
		}
		if (adjEdge == INT_MAX) {
			pair<int, int> edge = m_topo->GetEdge(i);
			out << "Edge <" << edge.first << "," << edge.second << "> can't found the node!\n";
		}
	}

	out << "result = " << result.size() << endl;
	//out << result.size() << endl;
	for (auto it = result.begin(); it != result.end(); it++) {
		out << "node " << it->first << "(" << it->second.size() << "): ";
		for (auto iter = it->second.begin(); iter != it->second.end(); iter++)
			out << iter->first << "<" << iter->second.first << "," << iter->second.second << ">, ";
		out << endl;
	}

}

void MaxFlow::OutputFile(int max)
{
	ofstream fout("../cplex/maxflow/maxflow.dat");
	fout << "nodeN=" << m_nodeNum << ";\n";
	fout << "edgeM=" << m_edgeNum << ";\n";
	fout << "K=" << max << ";\n";
	fout << "delta=[\n";

	for (int i = 0; i < m_edgeNum; i++) {
		fout << "[";
		int adjEdge = m_nodes[i].adjEdge;
		//FlowEdge *p = m_nodes[i].first;
		for (int j = 0; j < m_nodeNum; j++) {
			if (adjEdge != INT_MAX && m_edges[adjEdge].dst - m_edgeNum == j) {
				fout << 1 << ",";
				adjEdge = m_edges[adjEdge].next;
			}
			else {
				fout << 0 << ",";
			}
		}
		fout << "],\n";
	}
	fout << "];\n";
	fout.close();
}

