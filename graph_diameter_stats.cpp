//============================================================================
// Name        : graph_diameter_stats.cpp
// Author      : Livio Guidotti, Omar Trasatti
// Description : This program implements a fast bla bla bla
//////////////// scrivici qualcosa please
//////////////// la teoria è robba tua... lollololol rotrfmaocopter
//============================================================================



#include "Auxiliary.h"
#include <NetworKit/io/SNAPGraphReader.h>
#include <NetworKit/io/KONECTGraphReader.h>
#include <NetworKit/io/METISGraphReader.h>
#include <NetworKit/distance/BFS.h>
#include <NetworKit/distance/Diameter.h>
#include <boost/program_options.hpp>


using namespace std; 


bool sortbysec(const pair<int,int> &a, 
              const pair<int,int> &b) 
{ 
    return (a.second > b.second); 
} 

int main(int argc, char** argv) {
	
	// START ALGORITHM SETUP 
	// Passing (?) variables for computing algorithm

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");

	desc.add_options()
	("graph_location,g", po::value<string>(), "Input Graph File Location (in case of -a 1)")
	("source,s", po::value<int>(), "Source Node");


	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify( vm);

	// end parameters passing

	// Compute source and graph path location

	int source = -1;
	string graph_location;

	if (vm.empty()){
		cout << desc << "\n";
		throw runtime_error("Empty options");
	}
	if (vm.count("source"))
		source = vm["source"].as<int>();

	if (vm.count("graph_location"))
		graph_location = vm["graph_location"].as<string>();
    
	if(source<0){
        cout << desc << "\n";
        throw runtime_error("wrong source");
	}

	if(graph_location == ""){
		cout << desc << "\n";
	    throw runtime_error("wrong graph_location");
	}
	
	// Read the input graph
	





	// NetworKit::METISGraphReader readerMetis;
	// NetworKit::Graph graph = readerMetis.read(graph_location);

	NetworKit::KONECTGraphReader readerKonect;
	NetworKit::Graph graph = readerKonect.read(graph_location);


	// NetworKit::SNAPGraphReader readerSnap;
	// NetworKit::Graph graph = readerSnap.read(graph_location);






	// Get the last node with maxDegree
	// @input: graph; 
	// @output: node


	vector<NetworKit::node> graphNodes = graph.nodes();

	cout << "Numero di nodi nel grafo: " << graph.numberOfNodes() << "\n";
	cout << "Numero di nodi nel grafo: " << graph.numberOfEdges() << "\n";

	NetworKit::count maxDegree = graph.maxDegree();

	cout << "Il degree più alto è: " << maxDegree << "\n";
	NetworKit::node firstSource;
	
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

	cout << "Faccio la prima BFS, a partire dalla sorgente " << firstSource <<  "\n \n";

	vector<NetworKit::edgeweight> distances = firstBfs.getDistances();
	

	// Get u's eccentricity, alias i (last level of the first BFS)

	double fringeLevel = 0;

	for (int j = 0; j < distances.size() ; j++) {
	
		if (distances[j] <= graph.numberOfNodes() && fringeLevel <= distances[j]) {
			fringeLevel = distances[j];
		}
	}
	

	// Pick the farest nodes from u (source of initial BFS)

	list<NetworKit::node> listNodesInFringe;
	cout << "compongo la frangia iniziale mettendo nella lista i nodi a massima distanza " << fringeLevel << "\n";

	for (int i = 0; i < distances.size() ; i++) {
		
		if (distances[i] == fringeLevel){
			
			listNodesInFringe.push_back(graphNodes[i]);

		}

	}

	cout << "\n \n";

	double lowerBound = fringeLevel;
	int discoveredFringeLevels = 0;
	

	NetworKit::node element;
	int newEccentricity = 0;

	
	while (lowerBound <= (2 * fringeLevel)) {

		cout << "La frangia a distanza " << fringeLevel << " contiene " << listNodesInFringe.size() << " nodi \n \n";
		int numberOfBfsOnFringe = 0;
		// For all the nodes in the actual fringe..
		while(!listNodesInFringe.empty()) {

			newEccentricity = 0;

			// Take one of these nodes
			element = listNodesInFringe.front();
			cout << "Il nodo sul quale faccio la bfs è il nodo " << element << "\n";

			listNodesInFringe.pop_front();


			// Execute a BFS from actual
			NetworKit::BFS onFringeBfs(graph, element);
			onFringeBfs.run();


			// Update the number of BFS executed (on fringe and total)
			numberOfBfsOnFringe++;
			BFS_executed ++;
			cout << "Sto alla " << numberOfBfsOnFringe << "_a BFS, eseguita sulla frangia a distanza " << fringeLevel;


			// Calculate the eccentricity of actual node 
			vector<NetworKit::edgeweight> onFringeNodeDistances = onFringeBfs.getDistances();
	
			for (int j = 0; j < onFringeNodeDistances.size() ; j++) {
				
	
				if (onFringeNodeDistances[j] <= graph.numberOfNodes() && newEccentricity < onFringeNodeDistances[j]) {
					newEccentricity = onFringeNodeDistances[j];
				}
			}

			cout << ", e l'eccentricità del nodo, risultante dalla BFS è: " << newEccentricity << "\n \n";

		
			// 
			if (newEccentricity > lowerBound) { 
				lowerBound = newEccentricity;
				} 

			if (lowerBound > (2 * (fringeLevel-1))) {
				cout << "BREAK!!! Ho trovato che la massima eccentricità " << lowerBound << " supera l'upperbound " << 2*(fringeLevel-1) << " \n \n";
				break;
			}
		}

		if (lowerBound > (2 * (fringeLevel-1))) {			
			break;
		}
		
		cout << "La massima eccentricità per i nodi sulla frangia a distanza " << fringeLevel << " è: " << lowerBound << "\n \n";

		// Decrease fringe's level
		fringeLevel = fringeLevel - 1;

		// Populate new Fringe (indeed, level i-1), ordering nodes by MaxDegree

		for (int j = 0; j < distances.size() ; j++) {
	
			if (distances[j] == fringeLevel) {
			
			listNodesInFringe.push_back(graphNodes[j]);

			}

		}

		//End populate new Fringe



		// //Check nodes in new fringe

		// cout << "i nodi sulla frangia a distanza " << fringeLevel << " sono: " << listNodesInFringe.size() << ", in particolare: ";
		// list<NetworKit::node> temp = listNodesInFringe;
		// int tempSize = temp.size();
		// for (int j = 0; j < tempSize ; j++) {
		// 	cout << temp.front() <<" -";
		// 	temp.pop_front();
		// }

		// cout << "\n";

		// // end check nodes in new fringe



		// Populate vector of pair <node,degree> and sort it

		vector<pair<NetworKit::node, NetworKit::count>> arrayInFringe;

		int listNodesInFringeSize = listNodesInFringe.size();
		for (int j = 0; j < listNodesInFringeSize ; j++) {
			arrayInFringe.push_back(make_pair(listNodesInFringe.front(),graph.degree(listNodesInFringe.front())));
			listNodesInFringe.pop_front();
		}

		sort(arrayInFringe.begin(), arrayInFringe.end(), sortbysec); 

		// end populate vector



		// // Check right order

		// cout << "Vediamo se sono ordinati per grado: ";
		// for(auto t=arrayInFringe.begin(); t!=arrayInFringe.end(); ++t){
		// 	cout << std::get<1>(*t) << " -";
		// }
		// cout << "\n e vediamo se li prende per grado: il nodo " << std::get<0>(arrayInFringe.front()) << " con grado " << std::get<1>(arrayInFringe.front()) << "\n";

		// // end check order
		


		// Populate again fringe by degree 
		
		for (int j = 0; j < arrayInFringe.size() ; j++) {	
		
			listNodesInFringe.push_back(graphNodes[std::get<0>(arrayInFringe[j])]);
		}

		// End populate 
		// End decrease fringe's level

		discoveredFringeLevels++;

	};



	// Result of algo

	cout << ">> [graph-diameter]=\"" << lowerBound << "\"\n";
	cout << ">> [discovered-fringe-levels]=\"" << discoveredFringeLevels<< "\"\n";
	cout << ">> [BFS-executed]=\"" << BFS_executed << "\"\n";







	// Result of NetworKit-algo

	NetworKit::Diameter diameter(graph);
	diameter.run();
	pair<NetworKit::count,NetworKit::count> truediameter = diameter.getDiameter();
	cout << "il vero diametro è : " << truediameter.first << "\n";

  
}




// in un ordine 1632691