/*
* author: wangxing
* date: 2017.12.11
*/

#include <iostream>
#include "topology.h"
#include "max-flow.h"

using namespace std;

const int DEPTH = 2;
const int MAX = 8;
bool cplex = true;

int main(int argc, const char *argv[])
{
	Topology topo;
	if (topo.CreateFatTree(4) == 1) {	// Clos3(2, 3, 4)   FatTree(4)
		cout << "create topology failed!\n";
		system("PAUSE");
	}

	/*for (int i = 2; i < 5; i++) {
		for (int j = 8; j < 34; j ++) {
			MaxFlow maxflow;
			maxflow.Calculate(&topo, i, j);
			cout << "depth = " << i << ", max = " << j << ":  ";
			maxflow.ShowSolution();
		}
		cout << endl;
	}*/

	MaxFlow maxflow;
	maxflow.Calculate(&topo, DEPTH, MAX);
	maxflow.ShowSolution();
	if (cplex) maxflow.OutputFile(MAX);

	system("PAUSE");
	return 0;
}