/*
* author: wangxing
* date: 2017.12.11
*/

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include "topology.h"

Topology::Topology()
{
}

Topology::~Topology()
{
	for (vector<Node>::iterator it = m_nodes.begin(); it != m_nodes.end(); it++) {
		AdjacentEdge *q, *p = it->firstEdge;
		while (p) {
			q = p->next;
			delete p;
			p = q;
		}
	}
}

int Topology::Create(TopoType type)
{
	string str = "../topo-file.txt";
	ofstream fout(str);
	switch (type) {
		case NORMAL_2: GetNormal2LyerFile(fout);
			break;
		case NORMAL_3: GetNormal3LyerFile(fout);
			break;
		default:
			break;
	}
	fout.close();

	// Read topology file
	ifstream fin(str);
	if (fin.bad()) {
		cout << "Open topology file failed!\n";
		return 0;
	}
	
	// Initialize all the node
	int numSw;
	fin >> str >> numSw;
	m_nodes.resize(numSw);
	for (int i = 0; i < numSw; i++) {
		m_nodes[i].index = i;
		m_nodes[i].firstEdge = NULL;
	}

	AdjacentEdge *p, *q;
	Edge edge;
	while (fin >> edge.index >> edge.src >> edge.dst) {
		m_edges.push_back(edge);
		// for src
		q = new AdjacentEdge;
		q->index = edge.index;
		q->dst = edge.dst;
		q->next = NULL;
		p = m_nodes[edge.src].firstEdge;
		if (p) {
			while (p->next)	p = p->next;	// find the last edge
			p->next = q;
		}
		else {
			m_nodes[edge.src].firstEdge = q;
		}

		// for dst
		q = new AdjacentEdge;
		q->index = edge.index;
		q->dst = edge.src;
		q->next = NULL;
		p = m_nodes[edge.dst].firstEdge;
		if (p) {
			while (p->next)	p = p->next;	// find the last edge
			p->next = q;
		}
		else {
			m_nodes[edge.dst].firstEdge = q;
		}
	}
	fin.close();
	return numSw;
}

void Topology::Show(void)
{
	for (vector<Node>::iterator it = m_nodes.begin(); it != m_nodes.end(); it++) {
		cout << it->index;
		AdjacentEdge *p = it->firstEdge;
		while (p) {
			cout << "--->" << p->dst;
			p = p->next;
		}
		cout << endl;
	}
}

int Topology::GetNodeNum(void)
{
	return m_nodes.size();
}

int Topology::GetEdgeNum(void)
{
	return m_edges.size();
}

pair<int, int> Topology::GetEdge(int edge)
{
	return pair<int, int>(m_edges[edge].src, m_edges[edge].dst);
}

int Topology::Dist(int node, int edge)
{
	int dist = 0;
	set<int> nodes, temp;
	nodes.insert(node);
	while (!nodes.empty()) {
		dist++;
		for (set<int>::iterator it = nodes.begin(); it != nodes.end(); it++) {
			AdjacentEdge *p = m_nodes[*it].firstEdge;
			while (p) {
				if (p->index == edge)
					return dist;

				temp.insert(p->dst);
				p = p->next;
			}
		}
		nodes = temp;
		temp.clear();
	}
	return INT_MAX;
}

map<int, int> Topology::GetNodeAdjacentEdge(int node, int depth)
{
	map<int, int> result;
	vector<set<int> > res(depth);
	// depth = 1
	AdjacentEdge *p = m_nodes[node].firstEdge;
	while (p) {
		result[p->index] = 1;
		res[0].insert(p->dst);
		p = p->next;
	}
	// depth = others
	for (int i = 1; i < depth; i++) {
		set<int> &pre = res[i - 1];
		set<int> &now = res[i];
		for (set<int>::iterator it = pre.begin(); it != pre.end(); it++) {
			p = m_nodes[*it].firstEdge;
			while (p) {
				if (result.find(p->index) == result.end()) {
					result[p->index] = i + 1;
					now.insert(p->dst);
				}
				p = p->next;
			}
		}
	}
	return result;
}

map<int, int> Topology::GetEdgeAdjacentNode(int edge, int depth)
{
	map<int, int> result;
	vector<set<int> > res(depth);
	// depth = 1
	result[m_edges[edge].src] = 1;
	result[m_edges[edge].dst] = 1;
	res[0].insert(m_edges[edge].src);	
	res[0].insert(m_edges[edge].dst);
	// depth = other
	for (int i = 1; i < depth; i++) {
		set<int> &pre = res[i - 1];
		set<int> &now = res[i];
		for (set<int>::iterator it = pre.begin(); it != pre.end(); it++) {
			AdjacentEdge *p = m_nodes[*it].firstEdge;
			while (p) {
				if (result.find(p->dst) == result.end()) {
					result[p->dst] = i + 1;
					now.insert(p->dst);
				}
				p = p->next;
			}
		}
	}
	return result;
}

void Topology::GetNormal2LyerFile(ofstream &fout)
{
	cout << "Input the switch number of top layer and bottom layer: ";
	int numSw_top, numSw_bottom;
	cin >> numSw_top >> numSw_bottom;

	// Output to topology file
	fout << "switch_number: " << numSw_top + numSw_bottom << endl;
	
	int linkNum = 0;
	for (int i = 0; i < numSw_bottom; ++i) {
		for (int j = 0; j < numSw_top; ++j) {
			fout << linkNum << " " << i << " " << numSw_bottom + j << endl;
			++linkNum;
		}
	}
}

void Topology::GetNormal3LyerFile(ofstream &fout)
{
	cout << "Input the number of core switch, pod and switch in pod: ";
	int numCoreSw, numPOD, numSwPOD, numSwitchPOD;
	cin >> numCoreSw >> numPOD >> numSwPOD;	
	numSwitchPOD = numSwPOD / 2;   // Calculate the number of switch at each layer in POD

	// Outout to topology file
	fout << "switch_number: " << numCoreSw + numPOD * numSwPOD << endl;
	
	int linkNum = 0;
	for (int i = 0; i < numSwitchPOD * numPOD; ++i) {	// Links between POD switch and core switch
		for (int j = i % numSwitchPOD % 2; j < numCoreSw; j += 2) {
			fout << linkNum << " " << numSwitchPOD * numPOD + i << " " << numSwPOD * numPOD + j << endl;
			++linkNum;
		}
	}

	for (int i = 0; i < numPOD; ++i) {		// Links between switch and switch in POD
		for (int j = 0; j < numSwitchPOD; ++j) {
			for (int k = 0; k < numSwitchPOD; ++k) {
				fout << linkNum << " " << numSwitchPOD * i + j << " " << numSwitchPOD * numPOD + numSwitchPOD * i + k << endl;
				++linkNum;
			}
		}
	}
}