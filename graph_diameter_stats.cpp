//============================================================================
// Name        : graph_diameter_stats.cpp
// Author      : Livio Guidotti, Omar Trasatti
// Description : This program implements a fast bla bla bla
//////////////// scrivici qualcosa please
//////////////// la teoria è robba tua... lollololol rotrfmaocopter
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
	// Passing (?) variables for computing algorithm

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");

	desc.add_options()
	("graph_location,g", po::value<string>(), "Input Graph File Location (in case of -a 1)")
	("threads,t", po::value<int>(), "Number of councurrent threads");


	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify( vm);

	// end parameters passing

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
	
	// Read the input graph
	


	cout << "\n [concurrent-threads]:" <<  numberOfThreads << "\n";

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
	cout << "Numero di archi nel grafo: " << graph.numberOfEdges() << "\n";

	int sumOfDegrees = 0;
	graph.forNodes([&sumOfDegrees, &graph](NetworKit::node node) {
		 	sumOfDegrees = sumOfDegrees + graph.degree(node);
		});

	double averageDegrees = static_cast<double>(sumOfDegrees)/static_cast<double>(graphNodes.size());
	double variance = 0;
	double sumTerm = 0;

	for (int i=0;i<graphNodes.size();i++){
		sumTerm = pow((graph.degree(graphNodes[i])-averageDegrees), 2.0);
		variance = variance + (pow((graph.degree(graphNodes[i])-averageDegrees), 2.0));
	}

	double realVariance = static_cast<double>(variance)/static_cast<double>(graphNodes.size());
    
	cout << "Il grado medio è " << averageDegrees << "\n";
	cout << "La varianza del grado è: " << realVariance << "\n";
	
	double standardDeviation = sqrt (realVariance);

	cout << "La deviazione standard è: " << standardDeviation << "\n";

	int inDeviation = 0;
	for (int i=0;i<graphNodes.size();i++){
		if (graph.degree(graphNodes[i]) > (averageDegrees-standardDeviation) && graph.degree(graphNodes[i])<=(averageDegrees+standardDeviation)){
			inDeviation++;
		}
	}
cout << "INDEVIATION= " << inDeviation << "\n";
	double distributionPercentage = static_cast<double>(inDeviation)/static_cast<double>(graphNodes.size());
	cout << "La distribuzione rispetto alla media+-varianza è: " << distributionPercentage << "\n";

	// Start chrono
	
	

	NetworKit::count maxDegree = graph.maxDegree();

	cout << "Il degree più alto è: " << maxDegree << "\n";
	NetworKit::node firstSource;
	

	// getting max degree node

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

	// cout << "Faccio la prima BFS, a partire dalla sorgente " << firstSource <<  "\n \n";

	vector<NetworKit::edgeweight> distances = firstBfs.getDistances();
	

	// Get u's eccentricity, alias i (last level of the first BFS)

	double fringeLevel = 0;
	

	// try openMP
	
	
	#pragma omp parallel for num_threads(numberOfThreads)
	for (int j = 0; j < distances.size() ; j++) {
	
		if (distances[j] <= graph.numberOfNodes() && fringeLevel <= distances[j]) {
			fringeLevel = distances[j];
		}
	}
	

	

	// Pick the farest nodes from u (source of initial BFS)

	vector<NetworKit::node> listNodesInFringe;
	cout << "compongo la frangia iniziale mettendo nella lista i nodi a massima distanza " << fringeLevel << "\n";
	#pragma omp parallel for num_threads(numberOfThreads)
	for (int i = 0; i < distances.size() ; i++) {
		
		if (distances[i] == fringeLevel){
			
			listNodesInFringe.push_back(graphNodes[i]);

		}

	}

	
	cout << "\n \n";

	double lowerBound = fringeLevel;
	// int discoveredFringeLevels = 0;
	

	NetworKit::node element;
	int newEccentricity = 0;
	auto start = chrono::steady_clock::now();
	
	
	
	while (lowerBound <= (2 * (fringeLevel - 1))) {

		cout << "La frangia a distanza " << fringeLevel << " contiene " << listNodesInFringe.size() << " nodi \n \n";
		int numberOfBfsOnFringe = 0;
		// For all the nodes in the actual fringe..
		int listNodesInFringeSize = listNodesInFringe.size();

		#pragma omp parallel for num_threads(numberOfThreads)
		for (int i = 0; i < listNodesInFringeSize; i++){
		// while(!listNodesInFringe.empty()) 
			bool result = lowerBound <= (2 * fringeLevel);
			std::cout << ">>>>>>o<<<<<< [lowerBound <= (2 * fringeLevel)]=" << result << "\n";
			if (lowerBound <= (2 * fringeLevel)) {

			newEccentricity = 0;

			// Take one of these nodes
			
			cout << "Il nodo sul quale faccio la bfs è il nodo " << listNodesInFringe[i] << "\n";
			
			// // Execute a BFS from actual
			NetworKit::BFS onFringeBfs(graph, listNodesInFringe[i]);
			onFringeBfs.run();


			// // Update the number of BFS executed (on fringe and total)
			numberOfBfsOnFringe++;
			BFS_executed++;
			cout << "Sto alla " << numberOfBfsOnFringe << "_a BFS, eseguita sulla frangia a distanza " << fringeLevel << "\n";


			// // Calculate the eccentricity of actual node 
			vector<NetworKit::edgeweight> onFringeNodeDistances = onFringeBfs.getDistances();

			for (int j = 0; j < onFringeNodeDistances.size() ; j++) {
				if (onFringeNodeDistances[j] <= graph.numberOfNodes() && newEccentricity < onFringeNodeDistances[j]) {
					newEccentricity = onFringeNodeDistances[j];
				}
			}

			cout << ", e l'eccentricità del nodo, risultante dalla BFS è: " << newEccentricity << "\n \n";

		
			// // 
			if (newEccentricity > lowerBound) { 
				#pragma omp atomic write
				lowerBound = newEccentricity;
				} 
			}

		
		}

	
		
		//cout << "La massima eccentricità per i nodi sulla frangia a distanza " << fringeLevel << " è: " << lowerBound << "\n \n";

		// Decrease fringe's level
		fringeLevel = fringeLevel - 1;

		// Populate new Fringe (indeed, level i-1), ordering nodes by MaxDegree

		listNodesInFringe.clear();

		for (int j = 0; j < distances.size(); j++) {
			if (distances[j]==fringeLevel){
				listNodesInFringe.push_back(graphNodes[j]);
			}
		}

		// vector< pair<NetworKit::node,int> > coupleVector;
		
		// for (int j = 0; j < distances.size(); j++) {
		// 	if (distances[j] == fringeLevel) {
		// 	coupleVector.push_back(make_pair(graphNodes[j],graph.degree(j)));			
		// 	}
		// }

		// sort(coupleVector.begin(), coupleVector.end(), sortbysec); 
		// cout << "la frangia è ordinata per degree? \n";
		// for (int j = 0; j < coupleVector.size(); j++) {
		// 	cout << "(" << get<1>(coupleVector[j]) << "-";
		// 	listNodesInFringe.push_back(get<0>(coupleVector[j]));		
		// 	cout << get<0>(coupleVector[j]) << ") ";	
		// }

		

	};
	
	
	auto end = chrono::steady_clock::now();
	cout << "Tempo impiegato in millisecondi : " << chrono::duration_cast<chrono::milliseconds>(end - start).count()	<< " ms \n \n ";


	// Result of algo

	cout << ">> [graph-diameter]=\"" << lowerBound << "\"\n";
	// cout << ">> [discovered-fringe-levels]=\"" << discoveredFringeLevels<< "\"\n";
	cout << ">> [BFS-executed]=\"" << BFS_executed << "\"\n";





	// cout << "Tempo impiegato in secondi : " << chrono::duration_cast<chrono::seconds>(end - start).count() << " s \n \n ";




	// Result of NetworKit-algo

	auto start1 = chrono::steady_clock::now();

	NetworKit::Diameter diameter(graph);
	diameter.run();
	pair<NetworKit::count,NetworKit::count> truediameter = diameter.getDiameter();
	cout << "il vero diametro è : " << truediameter.first << "\n";

  	auto end1 = chrono::steady_clock::now();

	  cout << "Tempo impiegato in millisecondi da networkit : " 
		<< chrono::duration_cast<chrono::milliseconds>(end1 - start1).count()
		<< " ms \n \n ";

	cout << "Tempo impiegato in secondi da networkit: " 
	<< chrono::duration_cast<chrono::seconds>(end1 - start1).count()
	<< " s \n \n ";

}




// in un ordine 1632691



