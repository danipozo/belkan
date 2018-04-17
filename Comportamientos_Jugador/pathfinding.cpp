#include "pathfinding.hpp"


/*
 * This function is for testing purposes only
 */
std::vector<std::vector<Tile>> read_map(const std::string& file_name)
{
  std::ifstream f(file_name);
  std::vector<std::vector<Tile>> ret;
  std::map<char, Tile> lookup = { {'0', Tile::Free}, {'1', Tile::Occupied},
                                  {'S', Tile::Start}, {'G', Tile::Goal} };
  
  std::string line;
  int i=0;
  while(std::getline(f, line))
  {
    ret.emplace_back();
    
    for(auto c : line)
      ret[i].push_back(lookup[c]);

    i++;
  }

  return ret;
}

/*
 * Index IMPLEMENTATION
 * --------------------
 */
Index::Index(int x, int y) : pos{x,y} { }

Index::coord(Coord c) const
{
  switch(c)
  {
    case Coord::X:
      return pos.first;
    case Coord::Y:
      return pos.second;
  }
}


/*
 * Map IMPLEMENTATION
 * ------------------
 */

Map::Map(std::vector<std::vector<Tile>> map) : map(map) { }

std::optional<Tile> Map::at(Index pos) const
{
  using Coord;
  bool positive_coordinates = pos.coord(X) >= 0 && pos.coord(Y) >= 0;
  bool bounded_x_coordinate = pos.coord(X) < map.size();
  bool bounded_coordinates = bounded_x_coordinate && pos.coord(Y) < m[pos.coord(X)].size();

  if(!(positive_coordinates && bounded_coordinates))
    return {};

  return map[pos.coord(X)][pos.coord(Y)];
}

/*
 * Orientation IMPLEMENTATION
 * --------------------------
 */

Orientation operator+(Orientation o, MoveAction a)
{
  return Orientation { (static_cast<uint32_t>(o) + static_cast<uint32_t>(a)) % 4 };
}

/*
 * State IMPLEMENTATION
 * --------------------
 */

State::State(Index pos, Orientation compass) : pos(pos), compass(compass) { }

Index State::get_pos() const
{
  return pos;
}

State operator+(State s, MoveAction a)
{
  Index pos = s.get_pos();

  if(a == MoveAction::Forward)
    pos = Index {
      pos.coord(Coord::X)+(static_cast<uint32_t>(s.get_compass())-1)%2,
      pos.coord(Coord::Y)+(static_cast<uint32_t>(s.get_compass())-2)%2
    };


  return State { pos, s.get_compass() + a };
}

/*
 * index_comp IMPLEMENTATION
 * --------------------------
 */


bool index_comp::operator()(const Index& a, const Index& b) const
  {
    if(a.first < b.first) return true;
    if(a.first > b.first) return false;

    if(a.second < b.second) return true;

    return false;
  }

int manhattan_distance(Index x, Index y)
{
  return std::abs(x.first-y.first) + std::abs(x.second-y.second);
}

int_infty::operator int&() { return i; }
int_infty& int_infty::operator=(int other)
  {
    i = other;
    return *this;
  }
  

int comp::operator()(const Index& a, const Index& b)
{
  return f_score.get()[a] < f_score.get()[b];
}


Path reconstruct_path(Index goal, std::map<Index, Index, index_comp> came_from)
{
  Path ret = {goal};
  Index current = goal;
  
  while(came_from.count(current))
  {
    current = came_from[current];
    ret.push_front(current);
  }

  return ret;
}
