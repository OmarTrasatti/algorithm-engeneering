CXX = g++-8 -std=c++14
DBG = -g
OPT = -O3 -DNDEBUG

TARGET_A = graph_diameter_stats

OPTIONS = -lnetworkit -lboost_serialization -lboost_program_options -lboost_iostreams -lboost_system -lboost_filesystem -fopenmp -lboost_timer
INCLUDEPATH = $(HOME)/univaq/networkit/build_lib/include/ -I/usr/include/valgrind
PATHLIB = $(HOME)/univaq/networkit/build_lib/
SOURCES_A = $(TARGET_A).cpp Auxiliary.cpp

debug:
	$(CXX) $(DBG) -o $(TARGET_A) -I$(INCLUDEPATH) -L$(PATHLIB) $(SOURCES_A) $(OPTIONS)

clean:
	rm -rf $(TARGET_A) 
