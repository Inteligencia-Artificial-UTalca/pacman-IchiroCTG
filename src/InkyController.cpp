
#include "BTGhostController.h"
#include "InkyController.h"
#include <iostream>

#define MIN 100000

using namespace std;

InkyController::InkyController(shared_ptr<Character> character):
	Controller(character), root(make_shared<Selector>()) {

	auto filter = make_shared<Filter>();
	filter->addCondition(make_shared<InkyPowerpill>());
	filter->addAction(make_shared<InkyFrightened>());
	root->addChild(filter);

	auto filter2 = make_shared<Filter>();
	filter2->addCondition(make_shared<InkyTimeOut>());
	filter2->addAction(make_shared<InkyScatter>());
	root->addChild(filter2);

	root->addChild(make_shared<InkyChase>());
}

InkyController::~InkyController() {

}

Move InkyController::getMove(const GameState& game){	
	
	Info::getInfo()->in_character = character;
	Info::getInfo()->in_gamestate = &game;
	root->tick();
	return Info::getInfo()->out_move;
}

// Condicion al Consumir una Powerpill
Status InkyPowerpill::update() {
	auto character = Info::getInfo()->in_character;
	auto ghost = dynamic_cast<Ghost*>(character.get());

	if( ghost!=nullptr && ghost->isEdible()){
		return BH_SUCCESS;
	}else{
		return BH_FAILURE;
	}

}

// TimeOut
InkyTimeOut::InkyTimeOut() : Behavior(){
 lastTime = chrono::high_resolution_clock::now();
}

Status InkyTimeOut::update(){
	
	chrono::duration<float> timeStamp = chrono::high_resolution_clock::now() - lastTime;
	
	if((int)timeStamp.count() % 27<7){ 
		return BH_SUCCESS;
	}	
	return BH_FAILURE;
}

// Huir
InkyFrightened::InkyFrightened() : Behavior(), uniform_dist(0,3){

}

Status InkyFrightened::update(){
	auto personaje = Info::getInfo()->in_character;
	auto game = Info::getInfo()->in_gamestate;

	vector<Move> Movimientos = game->getMaze().getGhostLegalMoves(personaje->getPos(),personaje->getDirection());
	bool Valido=false;
	for (auto movimiento: Movimientos){
		if(movimiento!=PASS){
			Valido=true;
			break;
		}
	}
	if(!Valido){Movimientos = game->getMaze().getPossibleMoves(personaje->getPos());}

	Info::getInfo()->out_move = Movimientos[rand()%Movimientos.size()];
	return BH_SUCCESS;

}

// Dispersion

InkyScatter::InkyScatter() : Behavior(){
	target = make_pair(-1,-1);
}

Status InkyScatter::update(){
	
	if(target==pair{-1,-1}){
		auto SuperPastillas = Info::getInfo()->in_gamestate->getMaze().getPowerPillPositions();
		target = SuperPastillas[2];
	}

	auto character = Info:: getInfo()->in_character;
	auto game = Info::getInfo()->in_gamestate;

	vector<Move> Movimientos = game->getMaze().getGhostLegalMoves(character->getPos(),character->getDirection());
	bool Valido=false;
	for (auto movimiento : Movimientos){
		if(movimiento != PASS){
			Valido=true;
			break;
		}
	}
	if (!Valido){
		Movimientos = game->getMaze().getPossibleMoves(character->getPos());
	}

	float DistanciaMinima = MIN;
	Move MinMovimiento = PASS;
	for(auto Movimiento : Movimientos){
		if(Movimiento==PASS)break;
		float Distancia = euclid2(target,game->getMaze().getNodePos(game->getMaze().getNeighbour(character->getPos(),Movimiento)));

		if (Distancia<DistanciaMinima){
			DistanciaMinima=Distancia;
			MinMovimiento = Movimiento;
		}

	}
	if(MinMovimiento== PASS && !Movimientos.empty()){
		MinMovimiento = Movimientos[0];
	}
	Info::getInfo()->out_move = MinMovimiento;
	return BH_SUCCESS;

}


// Perseguir a Pacman

Status InkyChase::update(){

	auto character = Info::getInfo()->in_character;
	auto game = Info::getInfo()->in_gamestate;

	// Revisamos Posicion de Pacman
	int PosicionPacman = game->getPacmanPos();
	Move PacmanDirecciones = static_cast<Move>(game->getPacmanDir());
	int PosicionBlinky = game->getGhostsPos(0);

	// calculamos el movimiento de pacman hacia 2 pasos adelante	

	int PosicionObjetivo = PosicionPacman;
	if(PacmanDirecciones != PASS){
		for(int i=0;i<2;i++){
			int siguiente = game->getMaze().getNeighbour(PosicionObjetivo,PacmanDirecciones);
			if(siguiente==-1)break;
			auto siguienteMovimiento = game->getMaze().getPossibleMoves(siguiente);
			bool esValido = false;
			for(auto nextMovimiento : siguienteMovimiento){
				if(nextMovimiento != PASS){
					esValido = true;
					break;
				}
			}
			if(!esValido) break;
			PosicionObjetivo = siguiente;
		}
	}
	// para calcular el vector entre Blinky y el objetivo y 
	// asi calcular el target de Inky
	auto vectorDiference = pair{ game->getMaze().getNodePos(PosicionObjetivo).first - 
						game->getMaze().getNodePos(PosicionBlinky).first,
						game->getMaze().getNodePos(PosicionObjetivo).second -
						game->getMaze().getNodePos(PosicionBlinky).second
	};

	auto target = pair{ game->getMaze().getNodePos(PosicionObjetivo).first + 
						vectorDiference.first,
				 		game->getMaze().getNodePos(PosicionObjetivo).second + 
						vectorDiference.second
	};
	
	// Movimiento del Fantasma
    std::vector<Move> moves = game->getMaze().getGhostLegalMoves(character->getPos(),character->getDirection());
    bool Valido=false;
    for(auto movimiento : moves){
        if(movimiento != PASS){
            Valido = true;
            break; 
        }
    }
    if(!Valido){moves= game->getMaze().getPossibleMoves(character->getPos());}

	float DistanciaMinima = MIN;
	Move MinMovimiento = PASS;
	for(auto Movimiento : moves){
		if(Movimiento==PASS)break;
		float Distancia = euclid2(target,
			game->getMaze().getNodePos(game->getMaze().getNeighbour(character->getPos(),
			Movimiento)));

		if (Distancia<DistanciaMinima){
			DistanciaMinima=Distancia;
			MinMovimiento = Movimiento;
		}

	}

	if (MinMovimiento == PASS && !moves.empty()) MinMovimiento = moves[0];
	Info::getInfo()->out_move = MinMovimiento;
	return BH_SUCCESS;
}