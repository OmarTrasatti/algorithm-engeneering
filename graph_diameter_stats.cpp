//============================================================================
// Name        : graph_diameter_stats.cpp
// Author      : Livio Guidotti, Omar Trasatti
// Description : This program implements a fast bla bla bla
//////////////// scrivici qualcosa please
//////////////// la teoria è robba tua... lollololol rotrfmaocopter
//============================================================================



#include "Auxiliary.h"
#include <NetworKit/io/METISGraphReader.h>
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

	NetworKit::METISGraphReader reader;
	NetworKit::Graph graph = reader.read(graph_location);

	// Get the last node with maxDegree
	// @input: graph; 
	// @output: node


	std::vector<NetworKit::node> graphNodes = graph.nodes();

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
	std::vector<NetworKit::edgeweight> distances = firstBfs.getDistances();
	
	// end BFS from retrieved source


	// Get u's eccentricity, alias i (last level of the first BFS)

	int maxElement = 0;


	for (int i = 0; i < distances.size(); i++) {
		if (distances[i] > maxElement) {
			maxElement = distances[i];
		}
	}

	// Pick the farest nodes from u (source of initial BFS)
	
	std::list<int> listPointersToMaxElements;
	
	for (int i = 0; i < distances.size() ; i++) {
		
		if (distances[i] == maxElement){
			listPointersToMaxElements.push_back(i);

		}

	}

	int maxEccentricity = 0;
	int discoveredFringeLevels = 0;

	if (listPointersToMaxElements.size() > 0) {
		
		int element;
		int maxEccentricity = 0;
		int newEccentricity;

		do {
			while(!listPointersToMaxElements.empty()) {
				
				element = listPointersToMaxElements.front();
				listPointersToMaxElements.pop_front();
				
				NetworKit::BFS onFringeBfs(graph, graphNodes[element]);
				onFringeBfs.run();

				std::vector<NetworKit::edgeweight> onFringeNodeDistances = onFringeBfs.getDistances();

				int newEccentricity = *std::max_element(onFringeNodeDistances.begin(), onFringeNodeDistances.end());

				if (newEccentricity > maxEccentricity) { 
					maxEccentricity = newEccentricity;
					} 
				// maxEccentricity = Becc(u)
				// std::cout << "maxEccentricity is" << maxEccentricity;
			}
			
			
			// Decrease fringe's level
			maxElement--;
				for (int i = 0; i < distances.size() ; i++) {
			
					if (distances[i] == maxElement) {

					listPointersToMaxElements.push_back(i);

					}

				}

			// End decrease fringe's level

			discoveredFringeLevels++;

		} while (maxEccentricity <= 2 * maxElement && maxElement > 0);

	std::cout << ">> [graph-diameter]=\"" << maxEccentricity << "\"\n";
	std::cout << ">> [discovered-fringe-levels]=\"" << discoveredFringeLevels << "\"\n";
	std::cout << ">> [BFS executed]=\"" << discoveredFringeLevels++ << "\"\n";

	} else {
		std::cout << ">> [graph-diameter]=\"" << maxEccentricity << "\"\n";
	}

	// NetworKit::Diameter diameter(graph);
	// diameter.run();
	// std::pair<NetworKit::count,NetworKit::count> truediameter = diameter.getDiameter();
	// std::cout << "il vero diametro è : " << truediameter.first;

	
  
}
