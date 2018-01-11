/*
* author: wangxing
* date: 2017.12.11
*/

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <vector>
#include <map>

using namespace std;

enum TopoType {
	NORMAL_2,
	NORMAL_3
};

/* The definition of Topology */
class Topology
{
public:
	Topology();
	~Topology();

	/*
	 * type = 1, normal 2-layer
	 * type = 2, normal 3-layer
	 */
	int Create(TopoType type);
	void Show(void);

	int GetNodeNum(void);
	int GetEdgeNum(void);
	pair<int, int> GetEdge(int edge);

	int Dist(int node, int edge);
	vector<int> GetPath(int src, int dst);
	
	map<int, int> GetNodeAdjacentEdge(int node, int depth = 1);	// ´ý¼ìÑé
	map<int, int> GetEdgeAdjacentNode(int edge, int depth = 1);

private:
	void GetNormal2LyerFile(ofstream &fout);
	void GetNormal3LyerFile(ofstream &fout);

	/* The definition of edge */
	struct Edge {
		int index;
		int src;
		int dst;
	};

	/* The definition of adjacent edge */
	struct AdjacentEdge
	{
		int index;
		int dst;
		AdjacentEdge *next;
	};

	/* The definition of node */
	struct Node {
		int index;
		AdjacentEdge *firstEdge;
	};

	vector<Node> m_nodes;
	vector<Edge> m_edges;
};

#endif