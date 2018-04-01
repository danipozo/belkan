#ifndef __PATHFINDING_HPP__
#define __PATHFINDING_HPP__


#include <vector>
#include <map>
#include <set>
#include <queue>
#include <string>
#include <list>
//#include <optional>

#include "status_value/include/nonstd/status_value.hpp"

#include <fstream>
#include <iostream>

#include <functional>
#include <limits>

#include <cmath>

/*
 * In principle, and for the sake of pathfinding, we don't care
 * about any other property of the tiles other than these.
 * 
 * I might find a counterexample and generalize this though.  
 */
enum class Tile { Free, Occupied, Start, Goal };


/*
 * This function is for testing purposes only
 */
std::vector<std::vector<Tile>> read_map(const std::string& file_name);

template<class T>
using Map = std::vector<std::vector<T>>;
using Index = std::pair<int, int>;
using Path = std::list<Index>;

// Compare indices.
struct index_comp {
  bool operator()(const Index& a, const Index& b) const;
};

int manhattan_distance(Index x, Index y);

struct int_infty {
  int i = std::numeric_limits<int>::max()*(5./6); // A reasonably big number,
                                               // but prevents overflow when summing smaller numbers
  operator int&();
  int_infty& operator=(int other);
};

struct comp {
  Index goal;
  std::reference_wrapper<std::map<Index, int_infty, index_comp>> f_score;
  
  comp(Index g, std::map<Index, int_infty, index_comp>& f_score) : goal(g), f_score(f_score) {}
  
  int operator()(const Index& a, const Index& b);
};

template<class T>
std::vector<Index> neighbors(Index i, Map<T> m);

Path reconstruct_path(Index goal, std::map<Index, Index, index_comp> came_from);

struct unit_t {};

template<class Heuristic>
nonstd::status_value<unit_t,Path> pathfinding(Map<Tile> map, Index start, Index goal, Heuristic h);


#endif // __PATHFINDING_HPP__
