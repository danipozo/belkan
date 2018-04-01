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
};
  

int comp::operator()(const Index& a, const Index& b)
{
  return f_score.get()[a] < f_score.get()[b];
}

template<class T>
std::vector<Index> neighbors(Index i, Map<T> m)
{
  std::vector<int> idxs = {-1,0,1};
  std::vector<Index> ret;

  for(auto x : idxs)
    for(auto y : idxs)
    {
      // Skip center and diagonals. It's a non intuitive condition,
      // but you can check it on paper.
      if(!(std::abs(x) ^ std::abs(y)))
	continue;

      // Conditions on the coordinates
      bool positive_coordinates = i.first+x >= 0 && i.second+y >= 0;
      bool bounded_x_coordinate = i.first+x < m.size();
      bool bounded_coordinates = bounded_x_coordinate && i.second+y < m[i.first+x].size();

      // Skip coordinates not in the map 
      if(!positive_coordinates || !bounded_coordinates)
	continue;

      ret.push_back(std::make_pair(i.first+x, i.second+y));
    }

  return ret;
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

template<class Heuristic>
nonstd::status_value<unit_t,Path> pathfinding(Map<Tile> map, Index start, Index goal, Heuristic h)
{
  std::set<Index> closed_set;
  
  std::map<Index, Index, index_comp> came_from;
  std::map<Index, int_infty, index_comp> g_score;
  g_score[start] = 0;

  std::map<Index, int_infty, index_comp> f_score;
  f_score[start] = h(start, goal);

  std::priority_queue<Index, std::vector<Index>, comp>
    open_set(comp(goal, f_score));
  // REVIEW: this seems like a bad solution to me, as it forces manually
  // inserting and removing elements from the set at every insertion
  // and removal on the priority queue.
  
  // To track tiles in the open set,
  // not allowed by the priority queue.
  std::set<Index> open_set_;
  open_set.push(start);
  open_set_.insert(start);

  while(!open_set.empty())
  {
    auto current = open_set.top();

    if(current == goal)
      return nonstd::status_value<unit_t,Path>(unit_t(), reconstruct_path(goal, came_from));

    open_set.pop();
    open_set_.erase(current);
    closed_set.insert(current);

    // Ignore already evaluated neighbors
    for(auto i : neighbors(current, map))
    {
      if(closed_set.count(i))
	continue;

      // Ignore occupied tiles
      if(map[i.first][i.second] == Tile::Occupied)
      {
	closed_set.insert(i);
	continue;
      }

      if(!open_set_.count(i))
      {
	open_set.push(i);
	open_set_.insert(i);
      }

      auto tentative_g_score = g_score[current] + manhattan_distance(current, i);
      if(tentative_g_score >= g_score[i]) continue;

      came_from[i] = current;
      g_score[i] = tentative_g_score;
      f_score[i] = tentative_g_score + h(i, goal);
    }
    
  }

  return nonstd::status_value<unit_t,Path>(unit_t());
}
