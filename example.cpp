//============================================================================
// Name        : example.cpp
// Author      : Mattia D'Emidio
// Version     :
// Copyright   : Copyright © 2019 by Mattia D'Emidio
// Description :
//============================================================================



#include "Auxiliary.h"
#include <boost/program_options.hpp>

int main(int argc, char** argv) {


	// START ALGORITHM SETUP 
	
	// Passing (?) variables for computing algorithm
	namespace po = boost::program_options;
	po::options_description desc("Allowed options");

	desc.add_options()
	("graph_location,g", po::value<std::string>(), "Input Graph File Location (in case of -a 1)")
	("source,s", po::value<int>(), "Source Node")
	;


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
	

	NetworKit::Graph* graph;
    Auxiliary::read(graph_location,0,&graph);
	

	// END ALGORITHM SETUP 


	// ALGORTHM COMPUTING SECTION


    // Creates a max heap
    std::priority_queue <std::pair<int,int>> pq;
	pq.push(std::make_pair(source,0));
	std::vector<bool> touched;
	std::vector<bool> extracted;



	// One by one extract items from max heap
	while (pq.empty() == false)
	{
		int current = pq.top().first;
		int distance = pq.top().second;

		std::cout << current << " " << distance<<std::endl;
		pq.pop();
		graph->forNeighborsOf(current, [&](int u) {


		});
	}

	return 0;
	
	// END COMPUTING SECTION

	return EXIT_SUCCESS;

	
  
}
