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
	/* ��ʼ��������������ߵ��ڵ�Ķ���ͼ */
	void Initialize(Topology *topo, int depth, int max);
	/* Ford-Fulkerson �ⷨ��Ź��̣�Ѱ������·�� */
	/* return 0 ��ʾ���ҵ�����·��, retrun 1 ��ʾ�޷��ҵ�����·�� */
	void AddTag(void);
	int Adjust(void);
	void ResetFlag(void);

	void ReleaseFlow(int node);

	vector<int> GetChoosedNode(void);
	bool IsCoverAll(void);

	struct FlowEdge {
		int src;	// Դ�ڵ�
		int dst;	// Ŀ�Ľڵ�
		int cost;	// ���ã��˴�Ϊ�߾���monitor������
		int cap;	// ����
		int flow;	// ��ռ�õ�����
		int next;	// ָ����һ���ٽӱ�

		FlowEdge() { cost = 0; cap = 1; flow = 0; next = INT_MAX; }
	};

	struct FlowNode {
		bool choose;	// �Ƿ�ѡ��
		int flag;		// ��ţ�ָʾ������·��ǰһ���ڵ�
		int left;		// ʣ�������� ָʾ������·�п����ӵ�����
		int cost;		// ���ۣ�ָʾ������·��ǰһ���ڵ㵽�ýڵ�Ĵ���
		int adjEdge;	// ��һ���ٽӱ�

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