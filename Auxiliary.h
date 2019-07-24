/*
 * Labeling.h
 *
 *  Created on: 01 feb 2016
 *      Author: Mattia D'Emidio
 */

#ifndef AUXILIARY_H_
#define AUXILIARY_H_

#include <NetworKit/graph/Graph.h>
#include <NetworKit/io/SNAPGraphReader.h>
#include <NetworKit/io/KONECTGraphReader.h>
#include <NetworKit/io/METISGraphReader.h>
#include <NetworKit/distance/BFS.h>
#include <NetworKit/distance/Diameter.h>
#include <NetworKit/generators/HavelHakimiGenerator.h>
#include <boost/filesystem.hpp>
#include <dirent.h>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
class Auxiliary {

public:
	Auxiliary();
	virtual ~Auxiliary();

	static void read(std::string,bool,NetworKit::Graph**);



};

#endif /* AUXILIARY_H_ */
