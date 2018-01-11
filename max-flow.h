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
	int AdjustFlow(void);
	void ResetNodeFlag(void);

	void ReleaseFlow(int node);

	vector<int> GetLeftEdge(void);
	vector<int> GetChooseNode(void);
	bool IsCoverAll(void);

	struct FlowEdge {
		int dst;		// �ٽӱߵ�Ŀ�Ľڵ�
		int cost;		// ���ã��˴�Ϊ�߾���monitor������
		int cap;		// �������
		int flow;		// ��ǰ����
		FlowEdge *next;	// ָ����һ���ٽӱ�

		FlowEdge() { cost = 0; cap = 1; flow = 0; next = NULL; }
	};

	struct FlowNode {
		bool choose;			// �Ƿ�ѡ��

		// ��Ź���ʹ��
		int flag;			// ��ţ�ָʾ������·��ǰһ���ڵ�
		int left;			// ʣ�������� ָʾ������·�п����ӵ�����
		int cost;			// ���ۣ�ָʾ������·��ǰһ���ڵ㵽�ýڵ�Ĵ���
		
		FlowEdge *first;	// �����ٽӱߵĵ�һ����

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