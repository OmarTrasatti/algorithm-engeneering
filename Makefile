CXX = g++ -std=c++14
DBG = -g
OPT = -O3 -DNDEBUG
VALGRIND = -g -DNDEBUG

TARGET_A = example

OPTIONS = -lnetworkit -lboost_serialization -lboost_program_options -lboost_system -lboost_filesystem -fopenmp -lboost_timer
INCLUDEPATH = $(HOME)/networkit/build_lib/include/ -I/usr/include/valgrind
PATHLIB = $(HOME)/networkit/build_lib/
SOURCES_A = $(TARGET_A).cpp Auxiliary.cpp

debug:
	$(CXX) $(DBG) -o $(TARGET_A) -I$(INCLUDEPATH) -L$(PATHLIB) $(SOURCES_A) $(OPTIONS)

all:
	$(CXX) $(DBG) -o $(TARGET_A) -I$(INCLUDEPATH) -L$(PATHLIB) $(SOURCES_A) $(OPTIONS)

all-valgrind:
	$(CXX) $(VALGRIND) -o $(TARGET_A) -I$(INCLUDEPATH) -L$(PATHLIB) $(SOURCES_A) $(OPTIONS)

release:
	$(CXX) $(OPT) -o $(TARGET_A) -I$(INCLUDEPATH) -L$(PATHLIB) $(SOURCES_A) $(OPTIONS)


all-release:
	$(CXX) $(OPT) -o $(TARGET_A) -I$(INCLUDEPATH) -L$(PATHLIB) $(SOURCES_A) $(OPTIONS)

valgrind:
	$(CXX) $(VALGRIND) -o $(TARGET_A) -I$(INCLUDEPATH) -L$(PATHLIB) $(SOURCES_A) $(OPTIONS)


clean:
	rm -rf $(TARGET_A) 
