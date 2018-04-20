#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>

void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}

void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}



bool ComportamientoJugador::pathFinding(const estado &origen, const estado &destino, list<Action> &plan) {
  return false;
}

int to_regular_compass(Orientation o)
{
  int lookup[] = {0, 3, 2, 1};

  return lookup[static_cast<uint32_t>(o)];
}

estado from_state(State st)
{
  return estado { st.get_pos().coord(Coord::X), st.get_pos().coord(Coord::Y), to_regular_compass(st.get_compass()) };
}

Action to_action(MoveAction a)
{
  switch(a)
  {
    case MoveAction::Forward: return actFORWARD;
    case MoveAction::Left: return actTURN_L;
    case MoveAction::Right: return actTURN_R;
  }
}

MoveAction from_action(Action a)
{
  switch(a)
  {
    case actFORWARD: return MoveAction::Forward;
    case actTURN_L: return MoveAction::Left;
    case actTURN_R: return MoveAction::Right;
  }
}

Tile to_tile(unsigned char c)
{
  unsigned char free[] = {'T', 'S', 'K'};

  return std::any_of(free, free+sizeof(free), [c](auto a){ return a == c; })
       ? Tile::Free : Tile::Occupied;
}

Orientation to_orientation(int compass)
{
  int lookup[] = {0, 3, 2, 1};
  return Orientation{ static_cast<uint32_t>(lookup[compass]) };
}

Action ComportamientoJugador::think(Sensores sensores) {
  if(first_iteration)
  {
    pos = State { Index { sensores.mensajeF, sensores.mensajeC }, Orientation::North };
    first_iteration = false;
  }

  if(goal.get_pos() != Index { sensores.destinoF, sensores.destinoC })
  {
    goal = State { Index { sensores.destinoF, sensores.destinoC}, Orientation::North};

    std::vector<std::vector<Tile>> map_v(mapaResultado.size());
    std::transform(mapaResultado.begin(), mapaResultado.end(), map_v.begin(),
		   [](auto row){
		     std::vector<Tile> map_row(row.size());
		     std::transform(row.begin(), row.end(), map_row.begin(), to_tile);

		     return map_row;
		   });

    Map map(map_v);

    plan.clear();

    auto path_ = pathfinding(map, pos, goal.get_pos(), zero_distance);
    if(!path_)
    {
      std::cout << "Error: couldn't find path to target" << std::endl;
      return actFORWARD;
    }

    auto path = path_.value();
    std::transform(path.begin(), path.end(), std::back_inserter(plan), to_action);

    VisualizaPlan(from_state(pos), plan);
  }


  Action action;
  if(!plan.empty())
  {
    action = plan.front();
    pos = pos + from_action(action);
    plan.pop_front();
  }
  else
  {
    std::cout << "Empty plan" << std::endl;
    return actIDLE;
  }

  return action;
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
