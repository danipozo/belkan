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
