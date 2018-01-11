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
	int AdjustFlow(void);
	void ResetNodeFlag(void);

	void ReleaseFlow(int node);

	vector<int> GetLeftEdge(void);
	vector<int> GetChooseNode(void);
	bool IsCoverAll(void);

	struct FlowEdge {
		int dst;		// 临接边的目的节点
		int cost;		// 费用，此处为边距离monitor的跳数
		int cap;		// 最大流量
		int flow;		// 当前流量
		FlowEdge *next;	// 指向下一个临接边

		FlowEdge() { cost = 0; cap = 1; flow = 0; next = NULL; }
	};

	struct FlowNode {
		bool choose;			// 是否选用

		// 标号过程使用
		int flag;			// 标号，指示增广链路的前一个节点
		int left;			// 剩余流量， 指示增广链路中可增加的流量
		int cost;			// 代价，指示增广链路的前一个节点到该节点的代价
		
		FlowEdge *first;	// 所有临接边的第一条边

		FlowNode() {choose = true; flag = INT_MAX; left = 0; cost = INT_MAX; first = NULL; }
	};

	int m_nodeNum;
	int m_edgeNum;
	int m_totalNum;
	int m_src;
	int m_dst;

	Topology *m_topo;
	vector<FlowNode> m_nodes;
};

#endif