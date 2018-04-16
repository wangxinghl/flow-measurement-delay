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
	m_fileName = "../topo-file.txt";
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
	m_nodes.clear();
	m_edges.clear();
}

int Topology::CreateFatTree(int k)
{
	int numCoreSw, numPOD, numSwPOD, numSwitchPOD;
	numCoreSw = pow(k / 2, 2);
	numPOD = k;
	numSwPOD = k;
	numSwitchPOD = numSwPOD / 2;   // Calculate the number of switch at each layer in POD

	ofstream fout(m_fileName);
	// switch number
	fout << numCoreSw + numPOD * numSwPOD << endl;
	// all the links
	int linkNum = 0;
	for (int i = 0; i < numSwitchPOD * numPOD; ++i) {	// Links between POD switch and core switch
		for (int j = i % numSwitchPOD % k; j < numCoreSw; j += (k / 2)) {
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
	return Init();
}

int Topology::CreateClos2(int topSwNum, int bottomSwNum)
{
	ofstream fout(m_fileName);
	// switch number
	fout << topSwNum + bottomSwNum << endl;
	// all the links
	int linkNum = 0;
	for (int i = 0; i < bottomSwNum; ++i) {
		for (int j = 0; j < topSwNum; ++j) {
			fout << linkNum << " " << i << " " << bottomSwNum + j << endl;
			++linkNum;
		}
	}
	return Init();
}

int Topology::CreateClos3(int coreSwNum, int podNum, int podSwNum)
{
	int numSwitchPOD = podSwNum / 2;   // Calculate the number of switch at each layer in POD

	ofstream fout(m_fileName);
	// switch number
	fout << coreSwNum + podNum * podSwNum << endl;
	// all the links
	int linkNum = 0;
	for (int i = 0; i < numSwitchPOD * podNum; ++i) {	// Links between POD switch and core switch
		for (int j = i % numSwitchPOD % 2; j < coreSwNum; j += 2) {
			fout << linkNum << " " << numSwitchPOD * podNum + i << " " << podSwNum * podNum + j << endl;
			++linkNum;
		}
	}
	for (int i = 0; i < podNum; ++i) {		// Links between switch and switch in POD
		for (int j = 0; j < numSwitchPOD; ++j) {
			for (int k = 0; k < numSwitchPOD; ++k) {
				fout << linkNum << " " << numSwitchPOD * i + j << " " << numSwitchPOD * podNum + numSwitchPOD * i + k << endl;
				++linkNum;
			}
		}
	}
	return Init();
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

int Topology::Init(void)
{
	// Read topology file
	ifstream fin(m_fileName);
	if (fin.bad()) {
		cout << "Open topology file failed!\n";
		return 1;
	}

	// Initialize all the node
	int numSw;
	fin >> numSw;
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
	return 0;
}