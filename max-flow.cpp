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
	m_nodeNum = 0;
	m_edgeNum = 0;
	m_totalNum = 0;
	m_src = INT_MAX;
	m_dst = INT_MAX;
	m_topo = NULL;
}

MaxFlow::~MaxFlow()
{
	FlowEdge *cur, *next;
	for (int i = 0; i < m_totalNum; i++) {
		cur = m_nodes[i].first;
		while (cur) {
			next = cur->next;
			delete cur;
			cur = next;
		}
	}
}

void MaxFlow::Calculate(Topology *topo, int depth, int max)
{
	Initialize(topo, depth, max);

	while (1) {
		ResetNodeFlag();
		AddTag();
		if (AdjustFlow() == 1) break;			// 已无法找到增广链
	}

	while (1) {
		vector<int> choose = GetChooseNode();	// 返回流量从小到大的非零节点编号
		auto it = choose.begin();
		for (; it != choose.end(); it++) {
			m_nodes[*it].choose = false;
			ReleaseFlow(*it);
			while (1) {
				ResetNodeFlag();
				AddTag();
				if (AdjustFlow() == 1) break;			// 已无法找到增广链
			}
			if (IsCoverAll()) break;
			
			m_nodes[*it].choose = true;
			while (1) {
				ResetNodeFlag();
				AddTag();
				if (AdjustFlow() == 1) break;			// 已无法找到增广链
			}
		}
		if (it == choose.end()) break;
	}

	cout << "node:" << m_nodeNum << " edge:" << m_edgeNum << "\nCalculate over!\n";
}

void MaxFlow::Initialize(Topology *topo, int depth, int max)  
{
	m_topo = topo;
	m_nodeNum = topo->GetNodeNum();
	m_edgeNum = topo->GetEdgeNum();
	m_totalNum = m_nodeNum + m_edgeNum + 2;	// Total flow node = edge + node + 2 (source node and destination node)
	m_src = m_totalNum - 2;
	m_dst = m_totalNum - 1;

	vector<map<int, int> > edge2node;		// Edge to node
	for (int i = 0; i < m_edgeNum; i++)
		edge2node.push_back(topo->GetEdgeAdjacentNode(i, depth));

	/*
	* Create Flow Topology
	*/
	m_nodes.resize(m_totalNum);		// Initialize the nodes

	FlowEdge **cur, *next;
	cur = &m_nodes[m_src].first;		// From source node to the first layer
	for (int i = 0; i < m_edgeNum; i++) {
		next = new FlowEdge;
		next->dst = i;
		
		*cur = next;
		cur = &next->next;
	}

	for (int i = 0; i < m_edgeNum; i++) {	// From the first layer to the second layer
		cur = &m_nodes[i].first;
		map<int, int> &temp = edge2node[i];
		for (map<int, int>::iterator it = temp.begin(); it != temp.end(); it++) {
			next = new FlowEdge;
			next->dst = m_edgeNum + it->first;
			next->cost = it->second;

			*cur = next;
			cur = &next->next;
		}
	}

	for (int i = m_edgeNum; i < m_src; i++) {	// From the second layer to destination node
		next = new FlowEdge;
		next->dst = m_dst;
		// next->cost = IN_MAX;		// need to rewrite
		next->cap = max;
		m_nodes[i].first = next;
	}
}

void MaxFlow::AddTag(void)
{
	m_nodes[m_src].flag = m_src;
	m_nodes[m_src].left = INT_MAX;

	set<int> cur, next;
	cur.insert(m_src);
	while (!cur.empty()) {
		for (auto it = cur.begin(); it != cur.end(); it++) {
			map<int, vector<int> > temp;
			FlowEdge *p = m_nodes[*it].first;
			while (p) {		
				if (p->flow < p->cap && m_nodes[p->dst].choose)
					temp[p->cost].push_back(p->dst);
				p = p->next; 
			}
			if (temp.empty()) continue;

			int min_cost = INT_MAX;
			for (auto iter = temp.begin(); iter != temp.end(); iter++) {
				if (iter->first < min_cost)
					min_cost = iter->first;
			}

			for (auto iter = temp[min_cost].begin(); iter != temp[min_cost].end(); iter++) {
				if (m_nodes[*iter].flag == INT_MAX) {
					m_nodes[*iter].flag = *it;
					m_nodes[*iter].left = 1;
					m_nodes[*iter].cost = min_cost;
					next.insert(*iter);
				}
				else {
					if (min_cost < m_nodes[*iter].cost) {
						m_nodes[*iter].flag = *it;
						m_nodes[*iter].cost = min_cost;
					}
				}
			}
		}
		cur = next;
		next.clear();
	}
}

int MaxFlow::AdjustFlow(void)
{
	/* 调整过程 */
	if (m_nodes[m_dst].flag == INT_MAX)
		return 1;		// 已找不到增广链, return 1

	int cur = m_dst;
	int pre = m_nodes[cur].flag;
	while (cur != m_src) {
		FlowEdge *p = m_nodes[pre].first;
		while (p && p->dst != cur)	
			p = p->next;
		p->flow += m_nodes[cur].left;

		cur = pre;
		pre = m_nodes[pre].flag;
	}
	return 0;	// 已对增广链上的流量进行调整, return 0
}

void MaxFlow::ResetNodeFlag(void)
{
	for (int i = 0; i < m_totalNum; i++)
		m_nodes[i].flag = INT_MAX;
}

void MaxFlow::ReleaseFlow(int node)
{
	m_nodes[node].first->flow = 0;

	// 查找与该node节点匹配的所有边节点，并撤销其流量
	set<int> temp;
	for (int i = 0; i < m_edgeNum; i++) {
		FlowEdge *p = m_nodes[i].first;
		while (p) {
			if (p->dst == node && p->flow == 1) {
				p->flow = 0;
				temp.insert(i);
				break;
			}
			p = p->next;
		}
	}
	
	// 撤销Src节点到对应边节点的流量
	FlowEdge *p = m_nodes[m_src].first;
	while (p) {
		if (temp.find(p->dst) != temp.end())
			p->flow = 0;
		p = p->next;
	}
}

vector<int> MaxFlow::GetLeftEdge(void)
{
	vector<int> left;
	for (int i = 0; i < m_edgeNum; i++) {
		FlowEdge *p = m_nodes[i].first;
		while (p && p->flow == 0)
			p = p->next;

		if (!p)
			left.push_back(i);
	}
	return left;
}

vector<int> MaxFlow::GetChooseNode(void)
{
	vector<int> choose;
	for (int i = m_edgeNum; i < m_src; i++) {
		if (m_nodes[i].first->flow != 0)
			choose.push_back(i);
	}
	sort(choose.begin(), choose.end(), [&](int a, int b){ return m_nodes[a].first->flow < m_nodes[b].first->flow; });
	return choose;
}

bool MaxFlow::IsCoverAll(void)
{
	FlowEdge *p = m_nodes[m_src].first;
	while (p && p->flow == 1)
		p = p->next;

	if (p)
		return false;
	else
		return true;
}

void MaxFlow::ShowNode(void)
{
	for (size_t i = 0; i < m_nodes.size(); i++)
		cout << i << " (" << m_nodes[i].flag << "," << m_nodes[i].left << ")\n";
}

void MaxFlow::ShowEdge(void)
{
	for (size_t i = 0; i < m_nodes.size(); i++) {
		FlowEdge *p = m_nodes[i].first;
		while (p) {
			if (p->flow != 0)
				cout << i << "-->" << p->dst << "(" << p->flow << ")\n";
			p = p->next;
		}
	}
}

void MaxFlow::ShowSolution(ostream &out)
{
	//out << "Show solution!\n";
	map<int, map<int, pair<int, int> > > result;

	for (int i = 0; i < m_edgeNum; i++) {	// Get result
		FlowEdge *p = m_nodes[i].first;
		while (p && p->flow == 0)
			p = p->next;
		if (p)
			result[p->dst - m_edgeNum][i] = m_topo->GetEdge(i);
		else {
			pair<int, int> edge = m_topo->GetEdge(i);
			out << "Edge <" << edge.first << "," << edge.second << "> can't found the node!\n";
		}
	}

	out << "result = " << result.size() << endl;
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
		FlowEdge *p = m_nodes[i].first;
		for (int j = 0; j < m_nodeNum; j++) {
			if (p && p->dst - m_edgeNum == j) {
				fout << 1 << ",";
				p = p->next;
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

