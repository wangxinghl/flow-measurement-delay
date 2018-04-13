/*
* author: wangxing
* date: 2017.12.12
*/

#ifndef MAX_FLOW_H
#define MAX_FLOW_H

#include <fstream>
#include "topology.h"
#include <map>

using namespace std;

class MaxFlow
{
public:
	MaxFlow();
	~MaxFlow();
	
	void Calculate(Topology *topo, int depth, int max);
	void ShowNode(void);
	void ShowEdge(void);
	void ShowSolution(ostream &out = cout);
	void OutputFile(int max);

private:
	/* 初始化变量，并构造边到节点的二部图 */
	void Initialize(Topology *topo, int depth, int max);
	/* Ford-Fulkerson 解法标号过程，寻找增广路径 */
	/* return 0 表示已找到增广路径, retrun 1 表示无法找到增广路径 */
	void AddTag(void);
	int Adjust(void);
	void ResetFlag(void);

	void ReleaseFlow(int node);

	vector<int> GetChoosedNode(void);
	bool IsCoverAll(void);

	struct FlowEdge {
		int src;	// 源节点
		int dst;	// 目的节点
		int cost;	// 费用，此处为边距离monitor的跳数
		int cap;	// 容量
		int flow;	// 已占用的流量
		int next;	// 指向下一个临接边

		FlowEdge() { cost = 0; cap = 1; flow = 0; next = INT_MAX; }
	};

	struct FlowNode {
		bool choose;	// 是否选用
		int flag;		// 标号，指示增广链路的前一个节点
		int left;		// 剩余流量， 指示增广链路中可增加的流量
		int cost;		// 代价，指示增广链路的前一个节点到该节点的代价
		int adjEdge;	// 第一条临接边

		FlowNode() { choose = true; flag = INT_MAX; left = 0; cost = INT_MAX; adjEdge = INT_MAX; }
	};

	Topology *m_topo;
	int m_nodeNum;		// node number at original network
	int m_edgeNum;		// edge number at original network
	int m_totalNum;		// include src and dst node
	int m_src;
	int m_dst;

	map<int, map<int, int> > m_adj;
	vector<FlowNode> m_nodes;
	vector<FlowEdge> m_edges;
};

#endif