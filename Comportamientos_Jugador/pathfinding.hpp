#ifndef __PATHFINDING_HPP__
#define __PATHFINDING_HPP__


#include <vector>
#include <map>
#include <set>
#include <queue>
#include <string>
#include <list>
#include <optional>

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

// For the purpose of pathfinding
enum class Orientation : uint32_t
{
  North = 0,
  East,
  South,
  West
};

enum class MoveAction : uint32_t
{
  Forward = 0,
  Left = 1,
  Right = 3
};

Orientation operator+(Orientation o, MoveAction a);


enum class Coord {X, Y};

class Index {
  std::pair<int,int> pos;

public:
  Index() = default;
  Index(const Index& other) = default;
  Index(int x, int y);
  int coord(Coord c) const;
  bool operator==(const Index& other) const;
};


class Map {
  std::vector<std::vector<Tile>> map;

public:
  Map(std::vector<std::vector<Tile>>);
  std::optional<Tile> at(Index pos) const;
  // std::vector<Tile> row_at(int idx) const;
};



class State {
  Index pos;
  Orientation compass;

public:
  State(Index, Orientation);
  Index get_pos() const;
  Orientation get_compass() const;
};

State operator+(State s, MoveAction a);

// template<class T>
// using Map = std::vector<std::vector<T>>;
// using Index = std::pair<int, int>;
using Path = std::list<Index>;

// Compare indices.
struct index_comp {
  bool operator()(const Index& a, const Index& b) const;
};

int manhattan_distance(Index x, Index y);
int zero_distance(Index x, Index y);

struct int_infty {
  int i = std::numeric_limits<int>::max()*(5./6); // A reasonably big number,
                                               // but prevents overflow when summing smaller numbers
  operator int&();
  int_infty& operator=(int other);
};

struct comp {
  std::reference_wrapper<std::map<Index, int_infty, index_comp>> f_score;
  
  comp(std::map<Index, int_infty, index_comp>& f_score) : f_score(f_score) {}
  
  int operator()(const Index& a, const Index& b);
};

std::vector<Index> neighbors(Index i, Map m)
{
  return {};
}

Path reconstruct_path(Index goal, std::map<Index, Index, index_comp> came_from);

using Heuristic = int(*)(Index,Index);

std::optional<Path> pathfinding(Map map, Index start, Index goal, Heuristic h)
{
  std::set<Index, index_comp> closed_set;
  
  std::map<Index, Index, index_comp> came_from;
  std::map<Index, int_infty, index_comp> g_score;
  g_score[start] = 0;

  std::map<Index, int_infty, index_comp> f_score;
  f_score[start] = h(start, goal);

  std::multiset<Index, comp>
    open_set((comp(f_score)));
  // REVIEW: this seems like a bad solution to me, as it forces manually
  // inserting and removing elements from the set at every insertion
  // and removal on the priority queue.
  
  // To track tiles in the open set,
  // not allowed by the priority queue.
  std::set<Index> open_set_;
  open_set.insert(start);

  while(!open_set.empty())
  {
    auto current_it = open_set.begin(); 
    auto current = *(current_it);

    if(current == goal)
      return std::optional<Path>(reconstruct_path(goal, came_from));

    open_set.erase(current_it);
    closed_set.insert(current);

    // Ignore already evaluated neighbors
    for(auto i : neighbors(current, map))
    {
      if(closed_set.count(i))
	continue;

      // Ignore occupied tiles
      if(map.at(i) == Tile::Occupied)
      {
	closed_set.insert(i);
	continue;
      }

      if(!open_set_.count(i))
      {
	open_set.insert(i);
      }

      auto tentative_g_score = g_score[current] + manhattan_distance(current, i);
      if(tentative_g_score >= g_score[i]) continue;

      came_from[i] = current;
      g_score[i] = tentative_g_score;
      f_score[i] = tentative_g_score + h(i, goal);
    }
    
  }

  return std::nullopt;
}



#endif // __PATHFINDING_HPP__
