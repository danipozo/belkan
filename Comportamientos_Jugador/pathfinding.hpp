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
  bool operator<(const Index& other) const;
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
using Path = std::list<MoveAction>;

// Compare indices.
struct state_comp {
  bool operator()(const State& a, const State& b) const;
};

int manhattan_distance(Index x, Index y);
int zero_distance(Index x, Index y);

struct int_infty {
  int i = std::numeric_limits<int>::max()*(5./6); // A reasonably big number,
                                               // but prevents overflow when summing smaller numbers
  operator int&();
  int_infty& operator=(int other);
};

struct state_comp_fscore {
  std::reference_wrapper<std::map<State, int_infty, state_comp>> f_score;
  
  state_comp_fscore(std::map<State, int_infty, state_comp>& f_score) : f_score(f_score) {}
  
  int operator()(const State& a, const State& b) const;
};

std::vector<std::pair<State,MoveAction>> neighbors(State s, Map m)
{
  std::vector<std::pair<State, MoveAction>> ret;

  for(int i=0; i<4; i++)
  {
    MoveAction a = static_cast<MoveAction>(i);
    State neighbor = s + a;

    if(m.at(neighbor.get_pos()) && m.at(neighbor.get_pos()) != Tile::Occupied)
      ret.push_back({neighbor, a});
  }

  return ret;
}

Path reconstruct_path(State last_reached, std::map<State, std::pair<State,MoveAction>, state_comp> came_from);

using Heuristic = int(*)(Index,Index);

std::optional<Path> pathfinding(Map map, Index start, Index goal, Heuristic h)
{
  std::set<State, state_comp> closed_set;
  
  std::map<State, std::pair<State,MoveAction>, state_comp> came_from;
  std::map<State, int_infty, state_comp> g_score;
  g_score[State{start,Orientation::North}] = 0;

  std::map<State, int_infty, state_comp> f_score;
  f_score[State{start,Orientation::North}] = h(start, goal);

  std::set<State, state_comp_fscore>
    open_set((state_comp_fscore(f_score)));

  open_set.insert(State{start,Orientation::North});

  while(!open_set.empty())
  {
    auto current_it = open_set.begin(); 
    auto current = *(current_it);

    if(current.get_pos() == goal)
      return std::optional<Path>(reconstruct_path(current, came_from));

    open_set.erase(current_it);
    closed_set.insert(current);

    // Ignore already evaluated neighbors
    for(auto [i,a] : neighbors(current, map))
    {
      if(closed_set.count(i))
	continue;

      // Ignore occupied tiles
      if(map.at(i.get_pos()) == Tile::Occupied)
      {
	closed_set.insert(i);
	continue;
      }

      if(!open_set.count(i))
      {
	open_set.insert(i);
      }

      auto tentative_g_score = g_score[current] + 1;
      if(tentative_g_score >= g_score[i]) continue;

      came_from[i] = {current,a};
      g_score[i] = tentative_g_score;
      f_score[i] = tentative_g_score + h(i.get_pos(), goal);
    }
    
  }

  return std::nullopt;
}



#endif // __PATHFINDING_HPP__
