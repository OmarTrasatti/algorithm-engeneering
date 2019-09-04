//============================================================================
// Name        : graph_diameter_stats.cpp
// Author      : Livio Guidotti, Omar Trasatti
// Description : This program implements fringe-based algorithm for computing
//////////////// the input graph diameter. The results will be compared with
//////////////// NetworKit's "SumSweep" implementation.
//============================================================================



#include "Auxiliary.h"
#include <boost/program_options.hpp>

using namespace std; 


bool sortbysec(const pair<int,int> &a, 
              const pair<int,int> &b) 
{ 
    return (a.second < b.second); 
} 




	
int main(int argc, char** argv) {
	
	
	// START ALGORITHM SETUP 
	// Passing input variables for computing algorithm

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");

	desc.add_options()
	("graph_location,g", po::value<string>(), "Input Graph File Location (in case of -a 1)")
	("threads,t", po::value<int>(), "Number of councurrent threads");


	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify( vm);

	// End parameters passing

	// Compute source and graph path location

	string graph_location;
	int numberOfThreads = 8;

	if (vm.empty()){
		cout << desc << "\n";
		throw runtime_error("Empty options");
	}

	if (vm.count("graph_location"))
		graph_location = vm["graph_location"].as<string>();
    
	if (vm.count("threads"))
		numberOfThreads = vm["threads"].as<int>();

	if(graph_location == ""){
		cout << desc << "\n";
	    throw runtime_error("wrong graph_location");
	}
	
	cout << ">> [concurrent-threads]:" <<  numberOfThreads << "\n";

	// Read the input graph
	NetworKit::KONECTGraphReader readerKonect;
	NetworKit::Graph graph = readerKonect.read(graph_location);

	vector<NetworKit::node> graphNodes = graph.nodes();


	// Start chrono
	NetworKit::count maxDegree = graph.maxDegree();

	NetworKit::node firstSource;
	

	// Getting max degree node

	graph.forNodes([maxDegree, &firstSource, &graph](NetworKit::node node) {
		 	if(maxDegree == graph.degree(node)) {
				firstSource = node;
			 }
		});

	// End getting Get maxDegree node


	// BFS from retrieved source
	NetworKit::BFS firstBfs(graph, firstSource);
	firstBfs.run();

	int BFS_executed = 1;

	vector<NetworKit::edgeweight> distances = firstBfs.getDistances();
	

	// Get u's eccentricity, alias i (last level of the first BFS)
	double fringeLevel = 0;
	
	#pragma omp parallel for num_threads(numberOfThreads)
	for (int j = 0; j < distances.size() ; j++) {
	
		if (distances[j] <= graph.numberOfNodes() && fringeLevel <= distances[j]) {
			fringeLevel = distances[j];
		}
	}
	

	

	// Pick the farest nodes from u (source of initial BFS)
	vector<NetworKit::node> listNodesInFringe;

	#pragma omp parallel for num_threads(numberOfThreads)
	for (int i = 0; i < distances.size() ; i++) {
		
		if (distances[i] == fringeLevel){
			
			listNodesInFringe.push_back(graphNodes[i]);

		}

	}

	
	cout << "\n \n";

	double lowerBound = fringeLevel;
	

	NetworKit::node element;
	int newEccentricity = 0;
	auto start = chrono::steady_clock::now();
	
	
	
	while (lowerBound <= (2 * (fringeLevel - 1))) {

		int numberOfBfsOnFringe = 0;

		// For all the nodes in the actual fringe..
		int listNodesInFringeSize = listNodesInFringe.size();

		#pragma omp parallel for num_threads(numberOfThreads)
		for (int i = 0; i < listNodesInFringeSize; i++){

			bool result = lowerBound <= (2 * fringeLevel);

			if (lowerBound <= (2 * fringeLevel)) {

			newEccentricity = 0;
			
			// Execute a BFS from actual
			NetworKit::BFS onFringeBfs(graph, listNodesInFringe[i]);
			onFringeBfs.run();


			// Update the number of BFS executed (on fringe and total)
			numberOfBfsOnFringe++;
			BFS_executed++;


			// Calculate the eccentricity of actual node 
			vector<NetworKit::edgeweight> onFringeNodeDistances = onFringeBfs.getDistances();

			for (int j = 0; j < onFringeNodeDistances.size() ; j++) {
				if (onFringeNodeDistances[j] <= graph.numberOfNodes() && newEccentricity < onFringeNodeDistances[j]) {
					newEccentricity = onFringeNodeDistances[j];
				}
			}

			if (newEccentricity > lowerBound) { 
				#pragma omp atomic write
				lowerBound = newEccentricity;
				} 
			}

		
		}

		// Decrease fringe's level
		fringeLevel = fringeLevel - 1;

		// Populate new fringe (indeed, level i-1), ordering nodes by MaxDegree

		listNodesInFringe.clear();

		for (int j = 0; j < distances.size(); j++) {
			if (distances[j]==fringeLevel){
				listNodesInFringe.push_back(graphNodes[j]);
			}
		}

	};
	
	
	auto end = chrono::steady_clock::now();


	// Fringe Algorithm Implementation 
	cout << ">> Fringe Algorithm Results >>> >>> >>> >>> >>> >>> >>> >>> >>>\n";
	cout << ">> [BFS-executed]=\"" << BFS_executed << "\"\n";
	cout << ">> [fringe-graph-diameter-computed]=\"" << lowerBound << "\"\n";
	cout << ">> [fringe-elapsed-time]=\"" << chrono::duration_cast<chrono::milliseconds>(end - start).count()	<< " ms\"\n";
	cout << ">>\n";
	cout << ">>\n";
	// SumSweep Implementation Results (NetworKit)

	cout << ">> SumSweep Algorithm Results >>> >>> >>> >>> >>> >>> >>> >>> >>>\n";
	auto start1 = chrono::steady_clock::now();

	NetworKit::Diameter diameter(graph);
	diameter.run();
	pair<NetworKit::count,NetworKit::count> truediameter = diameter.getDiameter();

  	auto end1 = chrono::steady_clock::now();
	cout << ">> [sumsweep-graph-diameter-computed]:\" " << truediameter.first << "\"\n";
	cout << ">> [sumsweep-elapsed-time]=\"" << chrono::duration_cast<chrono::milliseconds>(end1 - start1).count() << " ms\"\n ";


}



