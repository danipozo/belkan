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

int Index::coord(Coord c) const
{
  switch(c)
  {
    case Coord::X:
      return pos.first;
    case Coord::Y:
      return pos.second;
  }
}

bool Index::operator==(const Index& other) const
{
  return pos == other.pos;
}

bool Index::operator<(const Index& other) const
{
  if(coord(Coord::X) < other.coord(Coord::X)) return true;
  if(coord(Coord::X) > other.coord(Coord::X)) return false;

  if(coord(Coord::Y) < other.coord(Coord::Y)) return true;

  return false;
}

/*
 * Map IMPLEMENTATION
 * ------------------
 */

Map::Map(std::vector<std::vector<Tile>> map) : map(map) { }

std::optional<Tile> Map::at(Index pos) const
{
  bool positive_coordinates = pos.coord(Coord::X) >= 0 && pos.coord(Coord::Y) >= 0;
  bool bounded_x_coordinate = pos.coord(Coord::X) < map.size();
  bool bounded_coordinates = bounded_x_coordinate && pos.coord(Coord::Y) < map[pos.coord(Coord::X)].size();

  if(!(positive_coordinates && bounded_coordinates))
    return {};

  return map[pos.coord(Coord::X)][pos.coord(Coord::Y)];
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

Orientation State::get_compass() const
{
  return compass;
}


State operator+(State s, MoveAction a)
{
  Index pos = s.get_pos();

  if(a == MoveAction::Forward)
    pos = Index {
      pos.coord(Coord::X)+(static_cast<int32_t>(s.get_compass())-1)%2,
      pos.coord(Coord::Y)+(static_cast<int32_t>(s.get_compass())-2)%2
    };


  return State { pos, s.get_compass() + a };
}

/*
 * state_comp IMPLEMENTATION
 * --------------------------
 */


bool state_comp::operator()(const State& a, const State& b) const
{
  if(a.get_pos() < b.get_pos()) return true;
  if(b.get_pos() < a.get_pos()) return false;

  if(static_cast<uint32_t>(a.get_compass()) < static_cast<uint32_t>(b.get_compass()))
    return true;

  return false;
}

/*
 * Heuristics
 * ----------
 */


int manhattan_distance(Index x, Index y)
{
  return std::abs(x.coord(Coord::X)-y.coord(Coord::X)) +
         std::abs(x.coord(Coord::Y)-y.coord(Coord::Y));
}

int zero_distance(Index x, Index y)
{
  return 0;
}

int_infty::operator int&() { return i; }
int_infty& int_infty::operator=(int other)
  {
    i = other;
    return *this;
  }
  

int state_comp_fscore::operator()(const State& a, const State& b) const
{
  if(f_score.get()[a] != f_score.get()[b])
    return f_score.get()[a] < f_score.get()[b];
  
  return  static_cast<uint32_t>(a.get_compass()) <
          static_cast<uint32_t>(b.get_compass());
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
