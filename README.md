# Algorithm Engeneering - Fringe Algorithm Implementation

## Students: Guidotti Livio - Omar Trasatti

### Requirements

1. NetworKit (only c++-core, see [here](https://networkit.github.io/) for further informations).
2. gcc/g++ 8.x
3. Boost C++ Lib

### Setup

1. Update the directories on `Makefile` for building the application:

```
CXX = g++-8 -std=c++14 // Edit with your c++ / g++ versions

...

INCLUDEPATH = //your/networkit/build/path/include
PATHLIB = //your/networkit/build/path

```

2. Export your *libnetworkit.so* path before running the application.

`export LD_LIBRARY_PATH=//your/networkit/build/path` 


### Usage

`./graph_diameter_stats <options>

#### Options

- `-g`: Input graph file location.
- `(Optional) -t`:  Number of councurrent threads in concurrent execution. (By default the max number of logic cores)