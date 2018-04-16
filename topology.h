/*
* author: wangxing
* date: 2017.12.11
*/

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <vector>
#include <map>

using namespace std;

/* The definition of Topology */
class Topology
{
public:
	Topology();
	~Topology();

	int CreateFatTree(int k);
	int CreateClos2(int topSwNum, int bottomSwNum);
	int CreateClos3(int coreSwNum, int podNum, int podSwNum);

	void Show(void);

	int GetNodeNum(void);
	int GetEdgeNum(void);
	pair<int, int> GetEdge(int edge);
	
	map<int, int> GetNodeAdjacentEdge(int node, int depth = 1);	// ´ý¼ìÑé
	map<int, int> GetEdgeAdjacentNode(int edge, int depth = 1);

private:
	int Init(void);

	/* The definition of edge */
	struct Edge {
		int index;
		int src;
		int dst;
	};

	/* The definition of adjacent edge */
	struct AdjacentEdge {
		int index;
		int dst;
		AdjacentEdge *next;
	};

	/* The definition of node */
	struct Node {
		int index;
		AdjacentEdge *firstEdge;
	};

	string m_fileName;
	vector<Node> m_nodes;
	vector<Edge> m_edges;
};

#endif