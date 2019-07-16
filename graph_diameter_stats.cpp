//============================================================================
// Name        : graph_diameter_stats.cpp
// Author      : Livio Guidotti, Omar Trasatti
// Description : This program implements a fast bla bla bla
//////////////// scrivici qualcosa please
//////////////// la teoria è robba tua... lollololol rotrfmaocopter
//============================================================================



#include "Auxiliary.h"
#include <NetworKit/io/SNAPGraphReader.h>
#include <NetworKit/distance/BFS.h>
#include <NetworKit/distance/Diameter.h>
#include <boost/program_options.hpp>

int main(int argc, char** argv) {
	
	// START ALGORITHM SETUP 
	// Passing (?) variables for computing algorithm

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");

	desc.add_options()
	("graph_location,g", po::value<std::string>(), "Input Graph File Location (in case of -a 1)")
	("source,s", po::value<int>(), "Source Node");


	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify( vm);

	// end parameters passing

	// Compute source and graph path location

	int source = -1;
	std::string graph_location;

	if (vm.empty()){
		std::cout << desc << "\n";
		throw std::runtime_error("Empty options");
	}
	if (vm.count("source"))
		source = vm["source"].as<int>();

	if (vm.count("graph_location"))
		graph_location = vm["graph_location"].as<std::string>();
    
	if(source<0){
        std::cout << desc << "\n";
        throw std::runtime_error("wrong source");
	}

	if(graph_location == ""){
		std::cout << desc << "\n";
	    throw std::runtime_error("wrong graph_location");
	}
	
	// Read the input graph

	NetworKit::SNAPGraphReader reader;
	NetworKit::Graph graph = reader.read(graph_location);

	// Get the last node with maxDegree
	// @input: graph; 
	// @output: node


	std::vector<NetworKit::node> graphNodes = graph.nodes();

	std::cout << "Numero di nodi nel grafo: " << graph.numberOfNodes() << "\n";

	NetworKit::count maxDegree = graph.maxDegree();
	NetworKit::node nodeWrapper = graphNodes[0];
	
	graph.forNodes([maxDegree, &nodeWrapper, &graph](NetworKit::node node) {
		 	if(maxDegree == graph.degree(node)) {
				nodeWrapper = node;
				// std::cout << nodeWrapper << "\n";
			 }
		});

	// End getting Get maxDegree node


	// BFS from retrieved source
	
	NetworKit::BFS firstBfs(graph, nodeWrapper);
	firstBfs.run();

	int BFS_executed = 1;

	std::cout << "Faccio la " << BFS_executed << "a BFS a partire dalla sorgente \n";

	std::vector<NetworKit::edgeweight> distances = firstBfs.getDistances();
	

	// Get u's eccentricity, alias i (last level of the first BFS)

	double fringeLevel = *max_element(distances.begin(), distances.end());
	

	// Pick the farest nodes from u (source of initial BFS)

	std::list<NetworKit::node> listNodesInFringe;
	
	for (int i = 0; i < distances.size() ; i++) {
		
		if (distances[i] == fringeLevel){

			listNodesInFringe.push_back(graphNodes[i]);

		}

	}


	double maxEccentricity = fringeLevel;
	int discoveredFringeLevels = 0;
	
	NetworKit::node element;
	int newEccentricity;

	while (maxEccentricity <= (2 * fringeLevel)) {

		std::cout << "La frangia a distanza " << fringeLevel << " contiene " << listNodesInFringe.size() << " nodi \n";
		
		// For all the nodes in the actual fringe..
		while(!listNodesInFringe.empty()) {

			int numberOfBfsOnFringe = 0;

			// Take one of these nodes
			element = listNodesInFringe.front();
			listNodesInFringe.pop_front();


			// Execute a BFS from actual
			NetworKit::BFS onFringeBfs(graph, element);
			onFringeBfs.run();


			// Update the number of BFS executed (on fringe and total)
			numberOfBfsOnFringe ++;
			BFS_executed ++;
			std::cout << "Sto alla " << numberOfBfsOnFringe << "a BFS, eseguita sulla frangia a distanza " << fringeLevel << "\n";


			// Calculate the eccentricity of actual node 
			std::vector<NetworKit::edgeweight> onFringeNodeEccentricity = onFringeBfs.getDistances();
			double newEccentricity = *std::max_element(onFringeNodeEccentricity.begin(), onFringeNodeEccentricity.end());

		
			// 
			if (newEccentricity > maxEccentricity) { 
				maxEccentricity = newEccentricity;
				} 

			if (maxEccentricity > (2*(fringeLevel-1))) {
				std::cout << "POSSO BREAKARE \n";
				break;
			}
		}

		std::cout << "La massima eccentricità per i nodi sulla frangia a distanza " << fringeLevel << " è: " << maxEccentricity << "\n";
		// Decrease fringe's level
		fringeLevel = fringeLevel - 1;

		
		// Populate new Fringe (indeed, level i-1)
			for (int j = 0; j < distances.size() ; j++) {
		
				if (distances[j] == fringeLevel) {
				
				listNodesInFringe.push_back(graphNodes[j]);

				}

			}

		

		// End decrease fringe's level

		discoveredFringeLevels++;

	} ;

	std::cout << "La massima eccentricità per i nodi sulla frangia a distanza " << fringeLevel << " è: " << maxEccentricity << "\n";
	




	// Result of algo

	std::cout << ">> [graph-diameter]=\"" << maxEccentricity << "\"\n";
	std::cout << ">> [discovered-fringe-levels]=\"" << discoveredFringeLevels << "\"\n";
	std::cout << ">> [BFS executed]=\"" << BFS_executed << "\"\n";




	// Result of NetworKit algo

	NetworKit::Diameter diameter(graph);
	diameter.run();
	std::pair<NetworKit::count,NetworKit::count> truediameter = diameter.getDiameter();
	std::cout << "il vero diametro è : " << truediameter.first;

	
  
}
