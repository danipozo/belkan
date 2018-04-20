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

bool Index::operator!=(const Index& other) const
{
  return !(*this == other);
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

Map::Map(std::vector<std::vector<Tile>>&& map) : map(map) { }
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

std::string print_index(Index i)
{
  std::ostringstream os;
  os << "(" << i.coord(Coord::X) << "," << i.coord(Coord::Y) << ")";

  return os.str();
}

std::string print_compass(Orientation o)
{
  std::ostringstream os;
  std::string names[] = { "North", "West", "South", "East" };

  os << names[static_cast<uint32_t>(o)];

  return os.str();
}

std::string print_state(State st)
{
  std::ostringstream os;
  os << "State{ " << print_index(st.get_pos()) << ", "
     << print_compass(st.get_compass()) << " }";

  return os.str();
}

std::string print_action(MoveAction a)
{
  std::ostringstream os;
  std::string names[] = { "Forward", "Left", "", "Right" };

  os << names[static_cast<uint32_t>(a)];

  return os.str();
}

std::string print_tile(Tile t)
{
  switch(t)
  {
  case Tile::Free: return "Free";
  case Tile::Occupied: return "Occupied";
  case Tile::Goal: return "Goal";
  case Tile::Start: return "Start";
  }
}


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


Path reconstruct_path(State last_reached, std::map<State, std::pair<State,MoveAction>, state_comp> came_from)
{
  Path ret;
  State current = last_reached;
  
  while(came_from.count(current))
  {
    auto [c, a] = came_from[current];
    current = c;
    
    ret.push_front(a);
  }

  return ret;
}

std::optional<Path> pathfinding(Map map, State start, Index goal, Heuristic h)
{
  std::set<State, state_comp> closed_set;

  std::map<State, std::pair<State,MoveAction>, state_comp> came_from;
  std::map<State, int_infty, state_comp> g_score;
  g_score[start] = 0;

  std::map<State, int_infty, state_comp> f_score;
  f_score[start] = h(start.get_pos(), goal);

  std::set<State, state_comp_fscore>
    open_set((state_comp_fscore(f_score)));

  open_set.insert(start);

  while(!open_set.empty())
  {
    auto current_it = open_set.begin(); 
    auto current = *(current_it);

    if(current.get_pos() == goal)
      return std::optional<Path>(reconstruct_path(current, came_from));

    open_set.erase(current_it);
    closed_set.insert(current);


    for(auto [i,a] : neighbors(current, map))
    {
      // Ignore already evaluated neighbors
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
