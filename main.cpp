/*
* author: wangxing
* date: 2017.12.11
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include "topology.h"
#include "max-flow.h"

using namespace std;

const int DEPTH = 2;
const int MAX = 9;
bool cplex = true;

int main(int argc, const char *argv[])
{
	Topology topo;
	if (topo.Create(NORMAL_3) == 0) {
		system("PAUSE");
		return 0;
	}
	
	MaxFlow maxflow;
	maxflow.Calculate(&topo, DEPTH, MAX);
	//maxflow.ShowNode();
	//maxflow.ShowEdge();
	maxflow.ShowSolution();
	if (cplex)
		maxflow.OutputFile(MAX);

	system("PAUSE");
	return 0;
}