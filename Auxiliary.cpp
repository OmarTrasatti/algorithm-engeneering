/*
 * Auxiliary.cpp
 *
 *  Created on: 01 feb 2016
 *  Author: Mattia D'Emidio
 */

#include "Auxiliary.h"
#include "progressBar.h"

#include <tuple>
#include <boost/algorithm/string.hpp>

Auxiliary::Auxiliary() {}


void Auxiliary::read(std::string source, bool scc_, NetworKit::Graph** g){


	std::ifstream ifs(source);

	if (!ifs)
		throw std::runtime_error("Error opening File ");




	int vertices = -1, edges = -1, weighted = -1, directed = -1;
	ifs >> vertices >> edges >> weighted >> directed;
	assert((weighted==0||weighted==1) && (directed==0||directed==1) && (vertices>=0 && edges>=0));
	ProgressStream reader(edges);
	std::string t1 = weighted==0 ? " unweighted" : " weighted";
	std::string t2 = directed==0 ? " undirected" : " directed";
	reader.label() << "Reading"<< t1 << t2 << " graph in "<<source<<" (CUSTOM FORMAT) containing " << vertices << " vertices and " << edges << " edges ";
	NetworKit::Graph* graph = new NetworKit::Graph(vertices,weighted,directed);
	int time,v1,v2,weight;

	while(true){

		ifs >> time >> v1 >> v2 >> weight;
		if(ifs.eof())
			break;

		++reader;

		assert(weighted || weight == 1 || weight == -1);

		if(v1==v2)
			continue;

		assert(graph->hasNode(v1) && graph->hasNode(v2));
		if(graph->hasEdge(v1,v2))
			std::cout<<"SKIPPING ROW"<<std::endl;
		else{
			graph->addEdge(v1,v2,weight);
			#ifndef NDEBUG
				if(!directed){
					if(!graph->hasEdge(v1,v2) && !graph->hasEdge(v2,v1))
						throw std::runtime_error("wrong edge insertion during construction");
				}
				else{
					if(!graph->hasEdge(v1,v2))
						throw std::runtime_error("wrong edge insertion during construction");
				}
				#endif
		}
	}



	ifs.close();
	graph->indexEdges();

	INFO("whole graph considered");
	*g = graph;

}

Auxiliary::~Auxiliary() {
}




