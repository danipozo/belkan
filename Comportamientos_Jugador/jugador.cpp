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

estado from_state(State st)
{
  return estado { st.get_pos().coord(Coord::X), st.get_pos().coord(Coord::Y), static_cast<int>(st.get_compass()) };
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

Tile to_tile(unsigned char c)
{
  unsigned char free[] = {'t', 's', 'k'};

  return std::any_of(free, free+sizeof(free), [c](auto a){ return a == c; })
       ? Tile::Free : Tile::Occupied;
}

Orientation to_orientation(int compass)
{
  return Orientation{ static_cast<uint32_t>(compass) };
}

Action ComportamientoJugador::think(Sensores sensores) {
  return actFORWARD;
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
