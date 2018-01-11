/*********************************************
 * OPL 12.7.1.0 Model
 * Author: wangxing
 * Creation Date: 2018年1月9日 at 下午7:20:02
 *********************************************/
int nodeN = ...;
int edgeM = ...;
int K = ...;

range node = 1..nodeN;
range edge = 1..edgeM;

int delta[edge][node] = ...;

dvar int+ x[node];
dvar int+ g[edge][node];

minimize
  sum(nodeI in node) x[nodeI];
  
subject to{
binary_for_node:
	forall(nodeI in node)
	  x[nodeI] <= 1;
binary_for_edge_to_node:
	forall(edgeI in edge, nodeI in node)
	  g[edgeI][nodeI] <= 1;
	  
each_edge_to_only_one_node:
	forall(edgeI in edge)
	  sum(nodeI in node) delta[edgeI][nodeI] * g[edgeI][nodeI] == 1;
each_node_can_not_more_than_K:
 	forall(nodeI in node)
 	  sum(edgeI in edge) delta[edgeI][nodeI] * g[edgeI][nodeI] <= x[nodeI]*K;
 }
 
 execute{
 	var ofile = new IloOplOutputFile("../../max-flow.txt");
 	
 	var cnt = 0;
 	ofile.write('x=[');
 	for(i in node){
 		ofile.write(x[i] + ',');
 		cnt += x[i];
 	} 		
 	ofile.write(']\ntotal_num = ' + cnt + '\n\n');
 	
 	for(i in node)
 		if(x[i] == 1){
 			ofile.write(i - 1 + ': ');
 			for(j in edge){
 				if(g[j][i] == 1)
 					ofile.write(j - 1 + ',');
    		} 					
 			ofile.write('\n');
		}
		 	
 	ofile.close();
 }